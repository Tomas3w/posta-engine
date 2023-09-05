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
	editor.camera.reset(new posta::entity::Camera(
		new posta::component::PCamera(0.01f, 200.0f, M_PI / 2),
		//new posta::component::OCamera(0.0f, 100.0f, 7),
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
		throw std::logic_error("current_scene was not set");
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
		if (!editor.outline_framebuffer)
			editor.outline_framebuffer.reset(new ColorDepthFramebuffer(get_width(), get_height()));
		if (!editor.draw2d_framebuffer)
			editor.draw2d_framebuffer.reset(new ColorFramebuffer(get_width(), get_height()));
		// Making the camera have a framebuffer
		dynamic_cast<posta::entity::CameraType*>(camera->get_internal_entity_type())->framebuffer = editor.draw2d_framebuffer.get();
		if (is_editor_mode_enabled())
		{
			editor.outline_framebuffer->bind();
			editor.outline_framebuffer->clear();
			editor.draw_viewport(false);
			editor.outline_framebuffer->unbind_framebuffer();
			clear_depth();
		}
		#endif
		// Runs custom code for each frame
		current_scene->update();

		// Drawing inbetween things from the editor
		#ifndef POSTA_EDITOR_DISABLED
		if (is_editor_mode_enabled())
		{
			editor.draw_inbetween();
			clear_depth();
			posta::entity::Entity::draw_whole_outline(editor.outline_framebuffer->texture.get());
		}
		#endif

		// If the editor is on, the 2d part will be drawn to a framebuffer and the editor front drawings will be present
		#ifndef POSTA_EDITOR_DISABLED
		if (is_editor_mode_enabled())
		{
			// Drawing transforms, outlines, and cameras
			editor.draw_viewport(true);
			// Binding the shader2d framebuffer
			editor.draw2d_framebuffer->bind();
			// This is to prevent unbinding
			Framebuffer::set_default(editor.draw2d_framebuffer.get());
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
			editor.draw2d_framebuffer->unbind_framebuffer();
			editor.handle_loop();
		}
		#endif


		while (SDL_PollEvent(&event))
		{
			#ifndef POSTA_EDITOR_DISABLED
			if (is_editor_mode_enabled()) // disables scene events, replacing them with the editor events only
				editor.handle_events(event);
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
							if (!editor.mode_status && camera)
							{
								if (dynamic_cast<posta::component::PCamera*>(editor.camera->camera.get()))
									dynamic_cast<posta::component::PCamera*>(editor.camera->camera.get())->__fov = M_PI / 4;
								editor.camera->update_projection_matrix();
								editor.camera->transform = camera->transform;

								editor.mode_status = !editor.mode_status;
								if (!editor.mode_status)
									set_mouse_confined_state(editor.input["prev_on_screen"]);
								for (std::pair<const std::string, float>& e : editor.input)
									e.second = 0;
								editor.input["prev_on_screen"] = get_mouse_confined_state();
							}
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
			std::this_thread::sleep_for(std::chrono::milliseconds(int(diff * 1000)));
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
	return editor.mode_status;
}

btCollisionWorld::ClosestRayResultCallback App::cast_ray(glm::vec3 src, glm::vec3 dest)
{
	btVector3 origin = posta::to_btVector3(src);
	btVector3 destination = posta::to_btVector3(dest);
	btCollisionWorld::ClosestRayResultCallback callback(origin, destination);
	physics->world->rayTest(origin, destination, callback);
	return callback;
}

btCollisionWorld::AllHitsRayResultCallback App::cast_ray_all_hits(glm::vec3 src, glm::vec3 dest)
{
	btVector3 origin = posta::to_btVector3(src);
	btVector3 destination = posta::to_btVector3(dest);
	btCollisionWorld::AllHitsRayResultCallback callback(origin, destination);
	physics->world->rayTest(origin, destination, callback);
	return callback;
}

static glm::vec3 intersect_ray_on_plane(glm::vec3 plane_pos, glm::vec3 plane_normal, glm::vec3 ray_src, glm::vec3 ray_dir)
{
	auto obj_pos = plane_pos;
	auto dotProduct = glm::dot(ray_dir, plane_normal);
	auto t = dot(plane_normal, obj_pos - ray_src) / dotProduct;
	return ray_src + t * ray_dir;
}

glm::vec3 App::Editor::get_selected_object_moving_position()
{
	posta::entity::Entity* entity = get_selected_entity();
	if (entity)
	{
		glm::vec2 pixel_position(app->mouse_x, app->mouse_y);
		glm::vec3 pos = entity->get_transform().get_position();
		auto ray_src = camera->get_point_on_near_plane(pixel_position);
		auto ray_dir = glm::normalize(camera->get_point_on_far_plane(pixel_position) - ray_src);
		switch (moving_object_axis)
		{
			case MOVING_OBJECT_AXIS::ALONG_CAMERA_PLANE:
				pos = intersect_ray_on_plane(entity->get_transform().get_position(), camera->transform.front(), ray_src, ray_dir);
				break;
			case MOVING_OBJECT_AXIS::ALONG_X:
				pos = intersect_ray_on_plane(ray_src, glm::normalize(glm::cross({0, 1, 0}, ray_dir)), entity->get_transform().get_position(), {1, 0, 0});
				break;
			case MOVING_OBJECT_AXIS::ALONG_Y:
				pos = intersect_ray_on_plane(ray_src, glm::normalize(glm::cross({1, 0, 0}, ray_dir)), entity->get_transform().get_position(), {0, 1, 0});
				break;
			case MOVING_OBJECT_AXIS::ALONG_Z:
				pos = intersect_ray_on_plane(ray_src, glm::normalize(glm::cross({0, 1, 0}, ray_dir)), entity->get_transform().get_position(), {0, 0, 1});
				break;
			case MOVING_OBJECT_AXIS::ALONG_LOCAL_X:
				pos = intersect_ray_on_plane(ray_src, glm::normalize(glm::cross(entity->get_transform().up(), ray_dir)), entity->get_transform().get_position(), entity->get_transform().right());
				break;
			case MOVING_OBJECT_AXIS::ALONG_LOCAL_Y:
				pos = intersect_ray_on_plane(ray_src, glm::normalize(glm::cross(entity->get_transform().right(), ray_dir)), entity->get_transform().get_position(), entity->get_transform().up());
				break;
			case MOVING_OBJECT_AXIS::ALONG_LOCAL_Z:
				pos = intersect_ray_on_plane(ray_src, glm::normalize(glm::cross(entity->get_transform().up(), ray_dir)), entity->get_transform().get_position(), entity->get_transform().front());
				break;
			case MOVING_OBJECT_AXIS::ALONG_YZ_PLANE:
				pos = intersect_ray_on_plane(entity->get_transform().get_position(), {1, 0, 0}, ray_src, ray_dir);
				break;
			case MOVING_OBJECT_AXIS::ALONG_XZ_PLANE:
				pos = intersect_ray_on_plane(entity->get_transform().get_position(), {0, 1, 0}, ray_src, ray_dir);
				break;
			case MOVING_OBJECT_AXIS::ALONG_XY_PLANE:
				pos = intersect_ray_on_plane(entity->get_transform().get_position(), {0, 0, 1}, ray_src, ray_dir);
				break;
			default:
				LOG("error, the MOVING_OBJECT_AXIS in this switch was not implemented or defined");
				break;
		}
		return pos;
	}
	return {0, 0, 0};
}

void App::Editor::draw_inbetween()
{
	posta::entity::Entity* entity = get_selected_entity();
	if (entity && moving_object_axis != MOVING_OBJECT_AXIS::OFF)
	{
		posta::component::Transform transform = entity->get_transform();
		transform.set_position(get_selected_object_moving_position());
		auto dm_type = dynamic_cast<posta::entity::TypeWithDrawableMeshes*>(entity->get_internal_entity_type());
		if (dm_type)
			posta::entity::Entity::draw_moving_object(dm_type, transform);
	}
}

void App::Editor::draw_viewport(bool front)
{
	for (posta::entity::Entity* entity : posta::entity::entities)
	{
		if (entity != camera.get())
			entity->__draw_entity(front);
	}
}

void App::Editor::handle_loop()
{
	// approaching camera or leaving camera
	if (dynamic_cast<posta::component::PCamera*>(camera->camera.get()) && dynamic_cast<posta::component::PCamera*>(camera->camera.get()))
	{
		if (input["editor_switch"])
		{
			auto pcamera = dynamic_cast<posta::component::PCamera*>(camera->camera.get());
			if (pcamera)
				dynamic_cast<posta::component::PCamera*>(camera->camera.get())->__fov = glm::mix(dynamic_cast<posta::component::PCamera*>(camera->camera.get())->__fov, pcamera->__fov, 0.1f);
			camera->transform = app->camera->transform.interpolate(camera->transform, input["interpolate_editor_camera"]);
			if (input["editor_switch"])
			{
				input["interpolate_editor_camera"] = glm::mix(input["interpolate_editor_camera"], 1.0f, 0.1f);
				if (input["interpolate_editor_camera"] > 0.9f || glm::distance2(camera->transform.get_position(), app->camera->transform.get_position()) < 0.1f)
					mode_status = false;
			}
		}
		else
			dynamic_cast<posta::component::PCamera*>(camera->camera.get())->__fov = glm::mix(dynamic_cast<posta::component::PCamera*>(camera->camera.get())->__fov, (float)M_PI / 2, 0.1f);
	}
	else if (input["editor_switch"])
		mode_status = false;
	camera->update_projection_matrix();

	// moving editor_camera
	auto& transform = camera->transform;
	float force = 10 * app->delta_time;
	transform.set_position(transform.get_position() + force * transform.right() * input["right"]);
	transform.set_position(transform.get_position() - force * transform.right() * input["left"]);
	transform.set_position(transform.get_position() + force * glm::vec3(0, 1, 0) * input["up"]);
	transform.set_position(transform.get_position() - force * glm::vec3(0, 1, 0) * input["down"]);
	glm::vec3 forward = transform.front();
	forward.y = 0;
	if (length(forward) == 0)
		forward = transform.up();
	else
		forward = glm::normalize(forward);
	transform.set_position(transform.get_position() - force * forward * input["forward"]);
	transform.set_position(transform.get_position() + force * forward * input["backward"]);

	for (std::string direction : {"left", "right", "forward", "backward", "up", "down"})
		input[direction] = glm::mix(input[direction], input["o" + direction], 0.1f);
}

#ifndef POSTA_EDITOR_KEY_MODIFIER1
#define POSTA_EDITOR_KEY_MODIFIER1 SDLK_LSHIFT
#endif
#ifndef POSTA_EDITOR_KEY_MODIFIER2
#define POSTA_EDITOR_KEY_MODIFIER2 SDLK_RSHIFT
#endif
#ifndef POSTA_EDITOR_KEY_X_AXIS
#define POSTA_EDITOR_KEY_X_AXIS SDLK_x
#endif
#ifndef POSTA_EDITOR_KEY_Y_AXIS
#define POSTA_EDITOR_KEY_Y_AXIS SDLK_y
#endif
#ifndef POSTA_EDITOR_KEY_Z_AXIS
#define POSTA_EDITOR_KEY_Z_AXIS SDLK_z
#endif
#ifndef POSTA_EDITOR_KEY_MOVE_OBJECT
#define POSTA_EDITOR_KEY_MOVE_OBJECT SDLK_g
#endif
#ifndef POSTA_EDITOR_KEY_MOVE_TO_OBJECT
#define POSTA_EDITOR_KEY_MOVE_TO_OBJECT SDLK_PERIOD
#endif
#ifndef POSTA_EDITOR_KEY_SWITCH_PERSPECTIVE
#define POSTA_EDITOR_KEY_SWITCH_PERSPECTIVE SDLK_o
#endif
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

void App::Editor::handle_events(SDL_Event& event)
{
	#ifndef POSTA_EDITOR_DISABLED
	switch (event.type)
	{
		case SDL_KEYUP:
			switch (event.key.keysym.sym)
			{
				case POSTA_EDITOR_KEY_X_AXIS:
					if (moving_object_axis != MOVING_OBJECT_AXIS::OFF)
					{
						if (input["modifier1"] || input["modifier2"])
							moving_object_axis = MOVING_OBJECT_AXIS::ALONG_YZ_PLANE;
						else if (moving_object_axis == MOVING_OBJECT_AXIS::ALONG_X)
							moving_object_axis = MOVING_OBJECT_AXIS::ALONG_LOCAL_X;
						else
							moving_object_axis = MOVING_OBJECT_AXIS::ALONG_X;
					}
					break;
				case POSTA_EDITOR_KEY_Y_AXIS:
					if (moving_object_axis != MOVING_OBJECT_AXIS::OFF)
					{
						if (input["modifier1"] || input["modifier2"])
							moving_object_axis = MOVING_OBJECT_AXIS::ALONG_XZ_PLANE;
						else if (moving_object_axis == MOVING_OBJECT_AXIS::ALONG_Y)
							moving_object_axis = MOVING_OBJECT_AXIS::ALONG_LOCAL_Y;
						else
							moving_object_axis = MOVING_OBJECT_AXIS::ALONG_Y;
					}
					break;
				case POSTA_EDITOR_KEY_Z_AXIS:
					if (moving_object_axis != MOVING_OBJECT_AXIS::OFF)
					{
						if (input["modifier1"] || input["modifier2"])
							moving_object_axis = MOVING_OBJECT_AXIS::ALONG_XY_PLANE;
						else if (moving_object_axis == MOVING_OBJECT_AXIS::ALONG_Z)
							moving_object_axis = MOVING_OBJECT_AXIS::ALONG_LOCAL_Z;
						else
							moving_object_axis = MOVING_OBJECT_AXIS::ALONG_Z;
					}
					break;
				case POSTA_EDITOR_KEY_MODIFIER1:
					input["modifier1"] = 0;
					break;
				case POSTA_EDITOR_KEY_MODIFIER2:
					input["modifier2"] = 0;
					break;
				case POSTA_EDITOR_KEY_MOVE_OBJECT:
					if (moving_object_axis == MOVING_OBJECT_AXIS::OFF)
						moving_object_axis = MOVING_OBJECT_AXIS::ALONG_CAMERA_PLANE;
					break;
				case POSTA_EDITOR_KEY_MOVE_LEFT:
					input["oleft"] = 0;
					break;
				case POSTA_EDITOR_KEY_MOVE_RIGHT:
					input["oright"] = 0;
					break;
				case POSTA_EDITOR_KEY_MOVE_FORWARD:
					input["oforward"] = 0;
					break;
				case POSTA_EDITOR_KEY_MOVE_BACKWARD:
					input["obackward"] = 0;
					break;
				case POSTA_EDITOR_KEY_MOVE_UP:
					input["oup"] = 0;
					break;
				case POSTA_EDITOR_KEY_MOVE_DOWN:
					input["odown"] = 0;
					break;
			}
			break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
				case POSTA_EDITOR_KEY_MODIFIER1:
					input["modifier1"] = 1;
					break;
				case POSTA_EDITOR_KEY_MODIFIER2:
					input["modifier2"] = 1;
					break;
				case POSTA_EDITOR_KEY_MOVE_TO_OBJECT:
					{
						posta::entity::Entity* entity = get_selected_entity();
						posta::entity::TypeWithRigidbody* rb_type = nullptr;
						if (entity)
							rb_type = dynamic_cast<posta::entity::TypeWithRigidbody*>(entity->get_internal_entity_type());
						if (rb_type)
						{
							auto callback = app->cast_ray_all_hits(camera->transform.get_position(), entity->get_transform().get_position());
							if (callback.hasHit())
							{
								for (int i = 0; i < callback.m_collisionObjects.size(); i++)
								{
									if (callback.m_collisionObjects.at(i) == rb_type->get_rigidbody().get_body())
									{
										auto point = posta::from_btVector3(callback.m_hitPointWorld.at(i));
										auto dir = glm::normalize(camera->transform.get_position() - entity->get_transform().get_position());
										camera->transform.set_position(point + dir * 10.0f);
										camera->transform.look_at(entity->get_transform().get_position());
									}
								}
							}
						}
					}
					break;
				case POSTA_EDITOR_KEY_SWITCH_PERSPECTIVE:
					if (dynamic_cast<component::PCamera*>(camera->camera.get()))
						camera->camera.reset(new posta::component::OCamera(0.0f, 100.0f, 7));
					else
						camera->camera.reset(new posta::component::PCamera(0.01f, 200.0f, M_PI / 2));
					break;
				case POSTA_EDITOR_KEY_SWITCH:
					input["editor_switch"] = 1;
					break;
				case POSTA_EDITOR_KEY_MOVE_LEFT:
					input["oleft"] = 1;
					break;
				case POSTA_EDITOR_KEY_MOVE_RIGHT:
					input["oright"] = 1;
					break;
				case POSTA_EDITOR_KEY_MOVE_FORWARD:
					input["oforward"] = 1;
					break;
				case POSTA_EDITOR_KEY_MOVE_BACKWARD:
					input["obackward"] = 1;
					break;
				case POSTA_EDITOR_KEY_MOVE_UP:
					input["oup"] = 1;
					break;
				case POSTA_EDITOR_KEY_MOVE_DOWN:
					input["odown"] = 1;
					break;
			}
			break;
		case SDL_MOUSEMOTION:
			if (app->get_mouse_confined_state())
			{
				float xrel = event.motion.xrel, yrel = event.motion.yrel;
				float s = -M_PI / 1000.0f;

				glm::quat rot = camera->transform.get_rotation();
				float m = (1 - (glm::dot(glm::vec3(0, -1, 0), rot * glm::vec3(0, 0, -1)) + 1) / 2.0f) * M_PI;

				glm::vec3 up = {0, 1, 0};
				glm::quat first_rotation = glm::rotate(rot, std::min(std::max(yrel * s, -m), static_cast<float>(M_PI - m)), glm::vec3(1, 0, 0));
				camera->transform.set_rotation(glm::rotate(first_rotation, xrel * s, glm::toMat3(glm::inverse(first_rotation)) * up));
			}
			break;
		case SDL_MOUSEBUTTONUP:
			if (event.button.button == SDL_BUTTON_LEFT)
			{
				if (moving_object_axis != MOVING_OBJECT_AXIS::OFF)
				{
					posta::entity::Entity* entity = get_selected_entity();
					if (entity)
						entity->set_transform(entity->get_transform().set_position(get_selected_object_moving_position()));
					moving_object_axis = MOVING_OBJECT_AXIS::OFF;
				}
				else
				{
					// cast a ray to search for the object in the mouse cursor
					glm::vec2 pixel_position(app->mouse_x, app->mouse_y);
					auto first_point = camera->get_point_on_near_plane(pixel_position);
					auto second_point = camera->get_point_on_far_plane(pixel_position);
					auto callback = app->cast_ray(first_point, second_point);
					currently_selected_type_entity = nullptr;
					float current_depth = 1;
					if (callback.hasHit())
					{
						for (entity::Entity* entity : entity::entities)
						{
							auto internal_entity_type = entity->get_internal_entity_type();
							if (auto rb_type = dynamic_cast<entity::TypeWithRigidbody*>(internal_entity_type))
							{
								if (rb_type->get_rigidbody().get_body() == callback.m_collisionObject)
								{
									currently_selected_type_entity = internal_entity_type;
									current_depth = glm::distance(first_point, posta::from_btVector3(callback.m_hitPointWorld)) / glm::distance(first_point, second_point);
									break;
								}
							}
						}
					}
					// for objects not present on the physics engine, draw them and search for collision with the mouse
					outline_framebuffer->bind();
					for (entity::Entity* entity : entity::entities)
					{
						auto internal_entity_type = entity->get_internal_entity_type();
						if (!dynamic_cast<entity::TypeWithRigidbody*>(internal_entity_type) && dynamic_cast<entity::TypeWithDrawableMeshes*>(internal_entity_type))
						{
							outline_framebuffer->clear();
							internal_entity_type->draw_back_as_selected();
							Uint32 pixel = 13;
							float depth = -2;
							glReadPixels(app->mouse_x, app->mouse_y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
							glReadPixels(app->mouse_x, app->mouse_y, 1, 1, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, &pixel);
							Uint32 data[] = {
								(pixel << 24) & 0xff,
								(pixel << 16) & 0xff,
								(pixel <<  8) & 0xff,
								(pixel <<  0) & 0xff,
							};
							Uint32 sum = 0;
							for (int i = 0; i < 4; i++)
								sum += data[i];
							// making current_depth linear in case of perspective view
							if (auto pcamera = dynamic_cast<component::PCamera*>(camera->camera.get()))
							{
								float _near = pcamera->__near;
								float _far = pcamera->__far;
								float ndc = depth * 2.0 - 1.0;
								depth = (2.0 * _near * _far) / (_far + _near - ndc * (_far - _near)) / _far;
							}
							if (sum > 0 && depth < current_depth)
							{
								current_depth = depth;
								currently_selected_type_entity = internal_entity_type;
							}
						}
					}
					outline_framebuffer->unbind_framebuffer();
				}
			}
			else if (event.button.button == SDL_BUTTON_RIGHT)
			{
				if (moving_object_axis != MOVING_OBJECT_AXIS::OFF)
					moving_object_axis = MOVING_OBJECT_AXIS::OFF;
				else
					app->set_mouse_confined_state(!app->get_mouse_confined_state());
			}
			break;
	}
	#endif
}

posta::entity::Entity* App::Editor::get_selected_entity()
{
	for (posta::entity::Entity* entity : posta::entity::entities)
	{
		if (entity->get_internal_entity_type() == currently_selected_type_entity)
			return entity;
	}
	return nullptr;
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
	if (editor.camera)
		editor.camera->set_resolution(width, height);
	editor.draw2d_framebuffer.reset();
	editor.outline_framebuffer.reset();
	#endif
}

std::vector<posta::ResourceBeaconParent*> posta::__app_beacons;

