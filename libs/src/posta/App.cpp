#include <posta/Util/ResourceBag.h>
#include <posta/App.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <posta/LuaAPI/LuaAPI.h>
#include <posta/Entity/Camera.h>
#include <posta/Util/LoggingMacro.h>
#ifndef POSTA_EDITOR_KEY_SWITCH
#define POSTA_EDITOR_KEY_SWITCH SDLK_SLASH
#endif

using posta::App;
using posta::PhysicsGlobal;

std::unique_ptr<posta::component::StaticMesh> App::mesh2d;

PhysicsGlobal::PhysicsGlobal()
{
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	overlappingPairCache = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver();
	world = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
	world->setGravity(btVector3(0, -10, 0));
}

PhysicsGlobal::~PhysicsGlobal()
{
	delete world;
	delete solver;
	delete overlappingPairCache;
	delete dispatcher;
	delete collisionConfiguration;
}

App* App::app = nullptr;

App::App()
{
	init();
	app = this;
}

App::~App()
{
	dest();
}

void App::init()
{
	auto start_time = std::chrono::high_resolution_clock::now();

	width = 640;
	height = 480;
	SDL_assert_release(SDL_Init(SDL_INIT_EVERYTHING) == 0);
	SDL_assert_release(IMG_Init(IMG_INIT_PNG) == IMG_INIT_PNG);
	SDL_assert_release(TTF_Init() == 0);
	SDL_assert_release(SDLNet_Init() == 0);
	SDL_assert_release(Mix_Init(MIX_INIT_OGG) == MIX_INIT_OGG);
	SDL_assert_release(Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 2048) == 0);
	Mix_AllocateChannels(10);
	
	window = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	context = SDL_GL_CreateContext(window);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	glewExperimental = GL_TRUE;
	glewInit();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Enable alpha blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Enable face culling
	glEnable(GL_CULL_FACE);

	// Setting view port and background clear color
	glViewport(0, 0, width, height);
	glClearColor(0, 0, 0, 1);

	// Getting keyboard input
	keyboard = SDL_GetKeyboardState(nullptr);
	exit_loop = false;

	// Filling mesh2d with its vertices
	posta::Mesh _mesh2d(&assets::basic_vertex_props);
	
	_mesh2d.add_data<3>(_mesh2d.get_index_by_name("position"), { 1, -1, 0});
	_mesh2d.add_data<3>(_mesh2d.get_index_by_name("position"), {-1,  1, 0});
	_mesh2d.add_data<3>(_mesh2d.get_index_by_name("position"), {-1, -1, 0});
	_mesh2d.add_data<3>(_mesh2d.get_index_by_name("position"), { 1,  1, 0});
	_mesh2d.add_data<3>(_mesh2d.get_index_by_name("normal"), {0, 0, 1});
	_mesh2d.add_data<2>(_mesh2d.get_index_by_name("uv"), {1, 1});
	_mesh2d.add_data<2>(_mesh2d.get_index_by_name("uv"), {1, 0});
	_mesh2d.add_data<2>(_mesh2d.get_index_by_name("uv"), {0, 1});
	_mesh2d.add_data<2>(_mesh2d.get_index_by_name("uv"), {0, 0});
	_mesh2d.add_face({{{0, 0, 0}, {1, 0, 3}, {2, 0, 2}}});
	_mesh2d.add_face({{{0, 0, 0}, {3, 0, 1}, {1, 0, 3}}});
	_mesh2d.lock();
	mesh2d.reset(new posta::component::StaticMesh(_mesh2d));
	// Init shader2d
	shader2d.reset(new posta::TwoDShader());

	// Init shader3d
	shader3d.reset(new posta::ThreeDShader());

	// Init default camera
	camera.reset(new posta::entity::Camera(
		new posta::component::PCamera(0.01f, 200.0f, M_PI / 4),
		get_width(),
		get_height()
		));
	// Init editor camera
	editor_camera.reset(new posta::entity::Camera(
		new posta::component::PCamera(0.01f, 200.0f, M_PI / 2),
		get_width(),
		get_height()
		));
	
	// Init physics
	init_physics();

	// Init lua state with api
	lua_state.reset(new posta::LuaAPI::LuaState);
	posta::LuaAPI::load_api(*lua_state);

	// Init resource_bag
	resource_bag = new posta::ResourceBag;

	// Setting initial 60 fps delta_time
	max_delta_time = 1 / 60.0f;
	
	// Time to load...
	auto end_time = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
	LOG("Time to init posta::App: ", duration, "ms");
}

void App::init_physics()
{
#ifndef DO_NOT_USE_PHYSICS
	time_step = 1.0f / 60.0f;
	physics = new PhysicsGlobal();
#endif
}

void App::dest()
{
	// deleting all scenes
	next_scene.reset();
	current_scene.reset();

	delete resource_bag;
	// deleting editor graphics
	#ifndef POSTA_EDITOR_DISABLED
	posta::entity::Entity::__free_graphics();
	#endif

	lua_state.reset(); // it's important that the lua state is deleted before the context and physics

	TCPConnToServer::connections.clear();

	dest_physics();

	SDL_DestroyWindow(window);
	SDL_GL_DeleteContext(context);

	Mix_CloseAudio();

	Mix_Quit();
	SDLNet_Quit();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

void App::dest_physics()
{
#ifndef DO_NOT_USE_PHYSICS
	delete physics;
	physics = nullptr;
#endif
}

void App::loop()
{
	// final initialization for resource bags
	for (ResourceBeaconParent* beacon : __app_beacons)
		beacon->init();
	// doing custom start function
	current_scene.reset(start());
	// current_scene start
	if (!current_scene)
		throw std::logic_error("Well, this isn't right");
	current_scene->start();

	Uint32 ticks = SDL_GetTicks();
	SDL_Event event;

	while (!exit_loop)
	{
		ticks = SDL_GetTicks();
		clear_frame_and_flip();

		mouse_state = SDL_GetMouseState(&mouse_x, &mouse_y);
		mouse_y = get_height() - mouse_y;

		// If the editor is on, the backside of the editor viewport will be drawn
		#ifndef POSTA_EDITOR_DISABLED
		if (is_editor_mode_enabled())
		{
			draw_editor_viewport(false);
			clear_depth();
		}
		#endif
		// Runs custom code for each frame
		current_scene->update(); 

		// If the editor is on, the 2d part will be drawn to a framebuffer and the editor front drawings will be present
		#ifndef POSTA_EDITOR_DISABLED
		if (is_editor_mode_enabled())
		{
			// Drawing transforms, outlines, and cameras
			draw_editor_viewport(true);
			// Binding the shader2d framebuffer
			if (!editor_2d_framebuffer)
				editor_2d_framebuffer.reset(new ColorFramebuffer(get_width(), get_height()));
			editor_2d_framebuffer->bind();
			// This is to prevent unbinding
			Framebuffer::set_default(editor_2d_framebuffer.get());
			// Making the camera have a framebuffer
			dynamic_cast<posta::entity::CameraType*>(camera->get_internal_entity_type())->framebuffer = editor_2d_framebuffer.get();
		}
		#endif
		// Binds shader2d and disables depth test for drawing 2D stuff
		disable_depth_test();
		shader2d->bind();
		current_scene->draw2d();
		enable_depth_test();

		// Unbinding the 2d framebuffer and running editor loop
		#ifndef POSTA_EDITOR_DISABLED
		if (is_editor_mode_enabled())
		{
			Framebuffer::set_default(nullptr);
			editor_2d_framebuffer->unbind_framebuffer();
			handle_editor_loop();
		}
		#endif


		while (SDL_PollEvent(&event))
		{
			#ifndef POSTA_EDITOR_DISABLED
			if (is_editor_mode_enabled()) // disables scene events, replacing them with the editor events only
				handle_editor_events(event);
			else
			#endif
				current_scene->event(event);
			switch (event.type)
			{
				case SDL_WINDOWEVENT:
					switch (event.window.event)
					{
						case SDL_WINDOWEVENT_RESIZED:
							update_resolution(event.window.data1, event.window.data2);
							break;
					}
					break;
				#ifndef POSTA_EDITOR_DISABLED
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym)
					{
						case POSTA_EDITOR_KEY_SWITCH:
							if (!editor_mode_status && camera)
							{
								editor_input["prev_on_screen"] = get_mouse_confined_state();
								dynamic_cast<posta::component::PCamera*>(editor_camera->camera.get())->__fov = M_PI / 4;
								editor_camera->update_projection_matrix();
								editor_camera->transform = camera->transform;
							}
							editor_mode_status = !editor_mode_status;
							if (!editor_mode_status)
								set_mouse_confined_state(editor_input["prev_on_screen"]);
							for (std::pair<const std::string, float>& e : editor_input)
								e.second = 0;
							break;
					}
					break;
				#endif
				case SDL_QUIT:
					exit_loop = true;
			}
		}
		// Updates physics world
		step_physics();

		// Changes scene if neccessary
		if (next_scene)
		{
			current_scene.reset(next_scene.release());
			current_scene->start();
		}

		// Updates delta time between frames
		Uint32 nticks = SDL_GetTicks();
		delta_time = (nticks - ticks) / 1000.0f;
		
		// corrects max_delta_time
		if (delta_time < max_delta_time)
		{
			float diff = max_delta_time - delta_time;
			while ((SDL_GetTicks() - nticks) / 1000.0f < diff);
			delta_time = (SDL_GetTicks() - ticks) / 1000.0f;
		}
	}

}

void App::step_physics()
{
#ifndef DO_NOT_USE_PHYSICS
	physics->world->stepSimulation(time_step);
#endif
}

void App::manage_textbox_input(SDL_Event& event, posta::ui::Textbox& textbox)
{
	switch (event.type)
	{
		case SDL_TEXTINPUT:
			textbox.add_text(event.text.text);
			break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
				case SDLK_RIGHT:
					textbox.next_cursor_pos();
					break;
				case SDLK_LEFT:
					textbox.prev_cursor_pos();
					break;
				case SDLK_DELETE:
					textbox.erase_next_character();
					break;
				case SDLK_BACKSPACE:
					textbox.erase_character();
					break;
			}
	}
}

void App::clear_frame()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
}

void App::clear_frame_and_flip()
{
	SDL_GL_SwapWindow(window);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
}

void App::clear_depth()
{
	glClear(GL_DEPTH_BUFFER_BIT);
}

const int& App::get_width() const
{
	return width;
}

const int& App::get_height() const
{
	return height;
}

void App::set_window_name(std::string name)
{
	SDL_SetWindowTitle(window, name.c_str());
}

void App::set_window_size(int w, int h)
{
	SDL_SetWindowSize(window, w, h);
	update_resolution(w, h);
}

void App::set_window_size_to_screen_size()
{
	SDL_DisplayMode dm;
	SDL_assert_release(SDL_GetDesktopDisplayMode(0, &dm) == 0);
	set_window_size(dm.w, dm.h);
}

void App::hide_window_frame()
{
	SDL_SetWindowBordered(window, SDL_FALSE);
}

void App::show_window_frame()
{
	SDL_SetWindowBordered(window, SDL_TRUE);
}

void App::maximize_window()
{
	SDL_MaximizeWindow(window);
}

void App::minimize_window()
{
	SDL_MinimizeWindow(window);
}

void App::set_fullscreen_state(bool v)
{
	SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN * v);
}

void App::add_bag(posta::ResourceBag*& bag_var, posta::ResourceBag* bag_lit)
{
	bag_var = resource_bag->add_bag(bag_lit);
}

bool App::is_click_pressed() const
{
	return SDL_BUTTON(SDL_BUTTON_LEFT) & mouse_state;
}

bool App::is_second_click_pressed() const
{
	return SDL_BUTTON(SDL_BUTTON_RIGHT) & mouse_state;
}

bool App::is_middle_click_pressed() const
{
	return SDL_BUTTON(SDL_BUTTON_MIDDLE) & mouse_state;
}

void App::disable_depth_test()
{
	glDisable(GL_DEPTH_TEST);
}

void App::enable_depth_test()
{
	glEnable(GL_DEPTH_TEST);
}

void App::set_framerate_limit(float maximum_delta_time)
{
	max_delta_time = maximum_delta_time;
}

float App::get_framerate_limit()
{
	return max_delta_time;
}

void App::set_mouse_confined_state(bool confined)
{
	SDL_SetRelativeMouseMode(confined ? SDL_TRUE : SDL_FALSE);
}

bool App::get_mouse_confined_state()
{
	return SDL_GetRelativeMouseMode();
}

bool App::is_editor_mode_enabled()
{
	return editor_mode_status;
}

static float approach(float& o, float n, float speed = 1 / 2.0f)
{
	return o = (o * (1 - speed) + n * speed);
}

void App::draw_editor_viewport(bool front)
{
	for (posta::entity::Entity* entity : posta::entity::entities)
	{
		if (entity != editor_camera.get())
			entity->__draw_entity(front);
	}
}

void App::handle_editor_loop()
{
	approach(dynamic_cast<posta::component::PCamera*>(editor_camera->camera.get())->__fov, M_PI / 2, 1 / 10.0f);
	editor_camera->update_projection_matrix();

	auto& transform = editor_camera->transform;
	float force = 10 * delta_time;
	transform.set_position(transform.get_position() + force * transform.right() * editor_input["right"]);
	transform.set_position(transform.get_position() - force * transform.right() * editor_input["left"]);
	transform.set_position(transform.get_position() + force * glm::vec3(0, 1, 0) * editor_input["up"]);
	transform.set_position(transform.get_position() - force * glm::vec3(0, 1, 0) * editor_input["down"]);
	glm::vec3 forward = transform.front();
	forward.y = 0;
	if (length(forward) == 0)
		forward = transform.up();
	else
		forward = glm::normalize(forward);
	transform.set_position(transform.get_position() - force * forward * editor_input["forward"]);
	transform.set_position(transform.get_position() + force * forward * editor_input["backward"]);

	for (std::string direction : {"left", "right", "forward", "backward", "up", "down"})
		approach(editor_input[direction], editor_input["o" + direction], 1 / 10.0f);
}

#ifndef POSTA_EDITOR_KEY_MOVE_UP
#define POSTA_EDITOR_KEY_MOVE_UP SDLK_e
#endif
#ifndef POSTA_EDITOR_KEY_MOVE_DOWN
#define POSTA_EDITOR_KEY_MOVE_DOWN SDLK_q
#endif
#ifndef POSTA_EDITOR_KEY_MOVE_LEFT
#define POSTA_EDITOR_KEY_MOVE_LEFT SDLK_a
#endif
#ifndef POSTA_EDITOR_KEY_MOVE_RIGHT
#define POSTA_EDITOR_KEY_MOVE_RIGHT SDLK_d
#endif
#ifndef POSTA_EDITOR_KEY_MOVE_FORWARD
#define POSTA_EDITOR_KEY_MOVE_FORWARD SDLK_w
#endif
#ifndef POSTA_EDITOR_KEY_MOVE_BACKWARD
#define POSTA_EDITOR_KEY_MOVE_BACKWARD SDLK_s
#endif
void App::handle_editor_events(SDL_Event& event)
{
	#ifndef POSTA_EDITOR_DISABLED
	switch (event.type)
	{
		case SDL_KEYUP:
			switch (event.key.keysym.sym)
			{
				case POSTA_EDITOR_KEY_MOVE_LEFT:
					editor_input["oleft"] = 0;
					break;
				case POSTA_EDITOR_KEY_MOVE_RIGHT:
					editor_input["oright"] = 0;
					break;
				case POSTA_EDITOR_KEY_MOVE_FORWARD:
					editor_input["oforward"] = 0;
					break;
				case POSTA_EDITOR_KEY_MOVE_BACKWARD:
					editor_input["obackward"] = 0;
					break;
				case POSTA_EDITOR_KEY_MOVE_UP:
					editor_input["oup"] = 0;
					break;
				case POSTA_EDITOR_KEY_MOVE_DOWN:
					editor_input["odown"] = 0;
					break;
			}
			break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
				case POSTA_EDITOR_KEY_MOVE_LEFT:
					editor_input["oleft"] = 1;
					break;
				case POSTA_EDITOR_KEY_MOVE_RIGHT:
					editor_input["oright"] = 1;
					break;
				case POSTA_EDITOR_KEY_MOVE_FORWARD:
					editor_input["oforward"] = 1;
					break;
				case POSTA_EDITOR_KEY_MOVE_BACKWARD:
					editor_input["obackward"] = 1;
					break;
				case POSTA_EDITOR_KEY_MOVE_UP:
					editor_input["oup"] = 1;
					break;
				case POSTA_EDITOR_KEY_MOVE_DOWN:
					editor_input["odown"] = 1;
					break;
			}
			break;
		case SDL_MOUSEMOTION:
			if (get_mouse_confined_state())
			{
				float xrel = event.motion.xrel, yrel = event.motion.yrel;
				float s = -M_PI / 1000.0f;

				glm::quat rot = editor_camera->transform.get_rotation();
				float m = (1 - (glm::dot(glm::vec3(0, -1, 0), rot * glm::vec3(0, 0, -1)) + 1) / 2.0f) * M_PI;

				glm::vec3 up = {0, 1, 0};
				glm::quat first_rotation = glm::rotate(rot, std::min(std::max(yrel * s, -m), static_cast<float>(M_PI - m)), glm::vec3(1, 0, 0));
				editor_camera->transform.set_rotation(glm::rotate(first_rotation, xrel * s, glm::toMat3(glm::inverse(first_rotation)) * up));
					/*
				else if (scene == CLIENT)
				{
					player_move_data.mov_x = xrel * s * 2;
					player_move_data.mov_y = yrel * s * 2;
				}
				else
					players[controlling]->rotate_view(yrel * s, xrel * s);*/
			}
			break;
		case SDL_MOUSEBUTTONUP:
			if (event.button.button == SDL_BUTTON_LEFT)
				set_mouse_confined_state(!get_mouse_confined_state());
			break;
	}
	#endif
}

void App::update_resolution(int w, int h)
{
	width = w;
	height = h;
	glViewport(0, 0, width, height);

	// updates default camera projection matrix.
	if (camera)
		camera->set_resolution(width, height);
	#ifndef POSTA_EDITOR_DISABLED
	// updates editor_camera
	if (editor_camera)
		editor_camera->set_resolution(width, height);
	#endif
}

std::vector<posta::ResourceBeaconParent*> posta::__app_beacons;

