#include "engine/include/Util/ResourceBag.h"
#include <engine/include/App.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <engine/include/LuaAPI/LuaAPI.h>
#include <engine/include/Entity/Camera.h>
#include <engine/include/Util/LoggingMacro.h>
//#include <engine/include/Util/Shader.h>

using Engine::App;
using Engine::PhysicsGlobal;

std::unique_ptr<Engine::Component::StaticMesh> App::mesh2d;

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
	//std::cout << "Deleting app(postaengine)" << std::endl;
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
	Engine::Mesh _mesh2d(&Assets::basic_vertex_props);
	/*float mesh2d_vertices[] = {
		1.0, -1.0, 0.0, -0.0, -0.0, 1.0, 1.0, 1.0,
		-1.0, 1.0, 0.0, -0.0, -0.0, 1.0, 0.0, 0.0,
		-1.0, -1.0, 0.0, -0.0, -0.0, 1.0, 0.0, 1.0,
		1.0, -1.0, 0.0, -0.0, -0.0, 1.0, 1.0, 1.0,
		1.0, 1.0, 0.0, -0.0, -0.0, 1.0, 1.0, 0.0,
		-1.0, 1.0, 0.0, -0.0, -0.0, 1.0, 0.0, 0.0,
	};*/
	//for (short i = 0; i < 6; i++)
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
	mesh2d.reset(new Engine::Component::StaticMesh(_mesh2d));
	// Init shader2d
	shader2d.reset(new Engine::TwoDShader());

	// Init shader3d
	shader3d.reset(new Engine::ThreeDShader());

	// Init default camera
	camera.reset(new Engine::Entity::Camera(
		new Engine::Component::PCamera(0.01f, 200.0f, M_PI / 4),
		get_width(),
		get_height()
		));
	
	// Init physics
	init_physics();

	// Init lua state with api
	lua_state.reset(new Engine::LuaAPI::LuaState);
	Engine::LuaAPI::load_api(*lua_state);

	// Init resource_bag
	resource_bag = new Engine::ResourceBag;
	
	// Time to load...
	auto end_time = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
	LOG("Time to init Engine::App: ", duration, "ms");
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
	delete resource_bag;

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
	Uint32 ticks;
	SDL_Event event;

	while (!exit_loop)
	{
		ticks = SDL_GetTicks();
		clear_frame_and_flip();


		mouse_state = SDL_GetMouseState(&mouse_x, &mouse_y);
		mouse_y = get_height() - mouse_y;
		on_frame(); // runs custom code for each frame
		on_draw();
		disable_depth_test();
		shader2d->bind();
		on_draw_2d();
		enable_depth_test();
		
		if (camera)
			camera->loop();
		while (SDL_PollEvent(&event))
		{
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
				case SDL_QUIT:
					exit_loop = true;
			}
			on_event(event);
		}
		step_physics();

		// Updates delta time between frames
		Uint32 nticks = SDL_GetTicks();
		delta_time = (nticks - ticks) / 1000.0f;
		
		// corrects framerate
		const float framerate = 1 / 60.0f;
		if (delta_time < framerate)
		{
			float diff = framerate - delta_time;
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

void App::on_draw()
{
	//
}

void App::on_draw_2d()
{
	//
}

void App::on_frame()
{
	//std::cout << delta_time << std::endl;
}

void App::on_event(SDL_Event& event)
{
	//
}

void App::manage_textbox_input(SDL_Event& event, Engine::UI::Textbox& textbox)
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

void App::update_resolution(int w, int h)
{
	width = w;
	height = h;
	glViewport(0, 0, width, height);

	// updates default camera projection matrix.
	if (camera)
		camera->set_resolution(width, height);
}

