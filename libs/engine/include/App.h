#ifndef POSTAENGINE_APP_H
#define POSTAENGINE_APP_H

#include <iostream>

#include <engine/include/Entity/Camera.h>
#include <engine/include/Util/Assets.h>
#include <engine/include/Component/StaticMesh.h>
#include <engine/include/Util/Mesh.h>
#include <engine/include/Util/Assets.h>
#include <engine/include/Util/TCPConnToServer.h>
#include <engine/include/Util/Shader.h>
#include <engine/include/LuaAPI/LuaState.h>
#include <engine/include/UI/Textbox.h>
#include <engine/include/Util/ResourceBag.h>

#include <memory>
#include <vector>
#include <unordered_map>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#include <SDL2/SDL_net.h>

#include <bullet/btBulletDynamicsCommon.h>
#include <unordered_set>
#include <glm/gtx/string_cast.hpp>


namespace Engine {
	//class Shader;

	struct PhysicsGlobal {
		PhysicsGlobal();
		~PhysicsGlobal();
		btDefaultCollisionConfiguration* collisionConfiguration;
		btCollisionDispatcher* dispatcher;
		btBroadphaseInterface* overlappingPairCache;
		btSequentialImpulseConstraintSolver* solver;
		btDiscreteDynamicsWorld* world;
	};
	
	//! Application class
	/** Stores the application global variables and is used as a
	 * template to all of the applications using this engine*/
	class App
	{
		public:
			static std::unique_ptr<Engine::Component::StaticMesh> mesh2d;
			/** Global application variable */
			static App* app;

			App();
			App(const App&) = delete;
			App operator=(const App&) = delete;
			virtual ~App();

			/** Initiliazes application*/
			virtual void init() final;
			virtual void init_physics() final;
			/** Desinitializes application */
			virtual void dest() final;
			virtual void dest_physics() final;

			/** Main loop of the application */
			virtual void loop() final;
			/** Updates physics world */
			virtual void step_physics() final;
			/** Draw all of the 3d drawing stages of the application */
			virtual void on_draw();
			/** Draw all of the 2d drawing stages of the application */
			virtual void on_draw_2d();

			/** Runs every frame */
			virtual void on_frame();
			/** Runs for every SDL event */
			virtual void on_event(SDL_Event& event);
			/** Manages the input for a textbox object, should be called in the on_event method */
			virtual void manage_textbox_input(SDL_Event& event, UI::Textbox& textbox) final;
			/** Clears the screen and swaps render buffer */
			virtual void clear_frame_and_flip() final;
			/** Clears the screen (or framebuffer if one is binded) */
			virtual void clear_frame() final;

			/** \return width of the window */
			virtual const int& get_width() const final;
			/** \return height of the window */
			virtual const int& get_height() const final;

			/** Changes window's name */
			virtual void set_window_name(std::string name) final;
			/** Sets the size of the window */
			virtual void set_window_size(int w, int h) final;
			/** Sets the size of the window to that of the screen */
			virtual void set_window_size_to_screen_size() final;
			/** Hides window's frame */
			virtual void hide_window_frame() final;
			/** Shows window's frame */
			virtual void show_window_frame() final;
			/** Maximizes window */
			virtual void maximize_window() final;
			/** Minimizes window */
			virtual void minimize_window() final;
			/** Sets window to fullscreen if v == true, makes the window windowed if v == false */
			virtual void set_fullscreen_state(bool v) final;
			/** Adds a resource bag to the main resource bag, see Engine::ResourceBag for more info */
			virtual void add_bag(Engine::ResourceBag*& bag_var, Engine::ResourceBag* bag_lit) final;

			virtual bool is_click_pressed() const final;
			virtual bool is_second_click_pressed() const final;
			virtual bool is_middle_click_pressed() const final;

			/** Disable depth testing, primarly used for 2d drawing */
			void disable_depth_test();
			/** Enable depth testing, primarly used for 3d drawing */
			void enable_depth_test();

			/** Map inputs to their values, use at when accessing input */
			std::unordered_map<std::string, float> input;
			/** Keyboard state */
			const Uint8* keyboard;

			/** Mouse state */
			Uint32 mouse_state;
			int mouse_x, mouse_y;
			
			/** Set to true to (in the next frame) exit the app */
			bool exit_loop;
			/** Time in seconds since the last frame */
			float delta_time;
			
			/// Main window
			SDL_Window* window;

			/** Default camera */
			std::unique_ptr<Entity::Camera> camera;

			/** Current shader */
			Shader* current_shader = nullptr;
			std::unique_ptr<TwoDShader> shader2d;
			std::unique_ptr<ThreeDShader> shader3d;

			/// Physics world
			PhysicsGlobal* physics;

			/// LuaState
			std::unique_ptr<LuaAPI::LuaState> lua_state;

			/// Resource bag, see Engine::ResourceBag for more info
			ResourceBag* resource_bag;
			
		private:
			void update_resolution(int w, int h);

			int width, height;
			SDL_GLContext context;

			float time_step;
	};

	class ResourceBeaconParent
	{
		public:
			virtual ~ResourceBeaconParent() = default;
			virtual void init() = 0;
	};
	extern std::vector<ResourceBeaconParent*> __app_beacons;
	template<class T>
	class ResourceBeacon : public ResourceBeaconParent
	{
		public:
			ResourceBeacon()
			{
				__app_beacons.push_back(this);
			}

			T* bag;

			friend class App;
		private:
			Engine::ResourceBag* _bag;
			void init()
			{
				Engine::App::app->add_bag(_bag, new T());
				bag = dynamic_cast<T*>(_bag);
			}
	};

} // namespace Engine

#endif // POSTAENGINE_APP_H


