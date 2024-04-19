#ifndef POSTAENGINE_APP_H
#define POSTAENGINE_APP_H

#include <iostream>

#include <posta/Util/Framebuffer.h>
#include <posta/Entity/Camera.h>
#include <posta/Util/Assets.h>
#include <posta/Component/StaticMesh.h>
#include <posta/Util/Mesh.h>
#include <posta/Util/Assets.h>
#include <posta/Network/TCPConnToServer.h>
#include <posta/Util/Shader.h>
#include <posta/LuaAPI/LuaState.h>
#include <posta/UI/Textbox.h>
#include <posta/Util/ResourceBag.h>

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

#include <posta/Util/Scene.h>


namespace posta {
	class EventEmitter;

	struct PhysicsGlobal {
		PhysicsGlobal();
		~PhysicsGlobal();
		btDefaultCollisionConfiguration* collisionConfiguration;
		btCollisionDispatcher* dispatcher;
		btBroadphaseInterface* overlappingPairCache;
		btSequentialImpulseConstraintSolver* solver;
		btDiscreteDynamicsWorld* world;
	};

	class ResourceBeaconParent
	{
		public:
			virtual ~ResourceBeaconParent() = default;
			virtual void init() = 0;
	};

	struct S__app_beacons {
		std::vector<ResourceBeaconParent*> beacons;
	};
	
	//! Application class
	/** Stores the application global variables and is used as a
	 * template to all of the applications using this engine*/
	class App
	{
		public:
			using EventCallback = void (*)(SDL_Event, EventEmitter*);
			/*struct Event
			{
				Event();
				void call(SDL_Event event) const
				{
					callback(event, emitter);
				}
				EventCallback callback = nullptr;
				EventEmitter* emitter  = nullptr;
			};*/

			static std::unique_ptr<posta::component::StaticMesh> mesh2d;
			/// Internal use.
			// This shouldn't be a pointer, but that causes a segfault for some unknown reason in linux
			static S__app_beacons* __app_beacons;
			/** Global application variable */
			static App* app;

			App();
			App(const App&) = delete;
			App& operator=(const App&) = delete;
			virtual ~App();

			/** Initiliazes application*/
			virtual void init() final;
			virtual void init_physics() final;
			/** Desinitializes application */
			virtual void dest() final;
			virtual void dest_physics() final;

			/** First function called in app, it returns the first scene*/
			virtual Scene* start() { return nullptr; }
			/** Main loop of the application */
			virtual void loop() final;
			/** Updates physics world */
			virtual void step_physics() final;

			/** Manages the input for a textbox object, should be called in the on_event method */
			virtual void manage_textbox_input(SDL_Event& event, ui::Textbox& textbox) final;
			/** Clears the screen and swaps render buffer */
			virtual void clear_frame_and_flip() final;
			/** Clears the screen (or framebuffer if one is binded) */
			virtual void clear_frame() final;
			/** Clears only the depth component of the frame (or framebuffer if one is binded) */
			virtual void clear_depth() final;

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
			/** Adds a resource bag to the main resource bag, see posta::ResourceBag for more info */
			virtual void add_bag(posta::ResourceBag*& bag_var, posta::ResourceBag* bag_lit) final;

			virtual bool is_click_pressed() const final;
			virtual bool is_second_click_pressed() const final;
			virtual bool is_middle_click_pressed() const final;

			/** Returns a path to a user-specific folder, it creates it if it doesn't exists */
			std::filesystem::path get_user_folder(std::string folder_name) const;

			/** Disable depth testing, primarly used for 2d drawing */
			void disable_depth_test();
			/** Enable depth testing, primarly used for 3d drawing */
			void enable_depth_test();

			/** Sets the max framerate of the application */
			void set_framerate_limit(float maximum_delta_time);
			/** Gets the max framerate of the applciation */
			float get_framerate_limit();

			/** Set mouse confined state to the window */
			void set_mouse_confined_state(bool confined);
			bool get_mouse_confined_state();

			/** Returns true if the application is in editor mode */
			bool is_editor_mode_enabled();

			/** Casts a ray from src to dest and returns a callback object from Bullet physics */
			btCollisionWorld::ClosestRayResultCallback cast_ray(glm::vec3 src, glm::vec3 dest);
			/** Casts a ray from src to dest and returns a callback object from Bullet physics, the ray can hit multiple objects in its way */
			btCollisionWorld::AllHitsRayResultCallback cast_ray_all_hits(glm::vec3 src, glm::vec3 dest);

			/** Registers an input from a key */
			void register_keypress_input(SDL_Keycode key, std::string name);

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
			float delta_time = 1;
			
			/// Main window
			SDL_Window* window;

			/** Default camera */
			std::unique_ptr<entity::Camera> camera;

			/** Current shader */
			Shader* current_shader = nullptr;
			std::unique_ptr<TwoDShader> shader2d;
			std::unique_ptr<ThreeDShader> shader3d;

			/// Physics world
			PhysicsGlobal* physics = nullptr;

			/// LuaState
			std::unique_ptr<LuaAPI::LuaState> lua_state;

			/// Resource bag, see posta::ResourceBag for more info
			ResourceBag* resource_bag;

			/// Modify this to get to the next scene in the next frame
			std::unique_ptr<Scene> next_scene;

			#ifndef EDITOR_DISABLED
			struct Editor
			{
				public:
					posta::component::Transform get_camera_transform()
					{
						if (!camera)
							return posta::component::Transform();
						return camera->transform;
					}

					/// This pointer may be invalid, it's just use for comparison, not for getting the actual type entity
					entity::Type* currently_selected_type_entity = nullptr;

					enum class MOVING_OBJECT_AXIS
					{
						OFF,
						ALONG_CAMERA_PLANE,
						ALONG_X,
						ALONG_Y,
						ALONG_Z,
						ALONG_LOCAL_X,
						ALONG_LOCAL_Y,
						ALONG_LOCAL_Z,
						ALONG_XY_PLANE,
						ALONG_YZ_PLANE,
						ALONG_XZ_PLANE,
					} moving_object_axis;
				private:
					glm::vec3 get_selected_object_moving_position();
					void draw_inbetween();
					void draw_viewport(bool front);
					void handle_loop();
					void handle_events(SDL_Event& event);

					/// Gets the selected entity, if any.
					entity::Entity* get_selected_entity();

					std::unordered_map<std::string, float> input;

					/// Internal framebuffer to store the 2D output of the scene's draw2d method when in editor mode
					std::unique_ptr<ColorDepthFramebuffer> draw2d_framebuffer;
					/// Internal fraembuffer to store the 3D output of the background of the editor scene
					std::unique_ptr<ColorDepthFramebuffer> outline_framebuffer;

					/// Editor camera
					std::unique_ptr<entity::Camera> camera;

					/// True if the user has entered editor_mode
					bool mode_status = false;

					friend class App;
					friend class entity::Entity;
					friend class entity::Camera;
			} editor;
			#endif

			friend class entity::Camera;
			friend class entity::Entity;
			friend class EventEmitter;
		private:
			/// Current scene
			std::unique_ptr<Scene> current_scene;

			void update_resolution(int w, int h);

			int width, height;
			SDL_GLContext context;

			float time_step;
			float max_delta_time;

			/// Inputs from keyboard
			std::unordered_map<SDL_Keycode, std::string> keypress_inputs;

			/// Event callbacks
			std::unordered_map<SDL_Keycode, std::array<std::unordered_map<EventEmitter*, std::unordered_set<EventCallback>>, 2>> key_events;
			std::array<std::unordered_map<EventEmitter*, std::unordered_set<EventCallback>>, 2> mouse_left_button_events;
			std::array<std::unordered_map<EventEmitter*, std::unordered_set<EventCallback>>, 2> mouse_right_button_events;
			std::array<std::unordered_map<EventEmitter*, std::unordered_set<EventCallback>>, 2> mouse_middle_button_events;
			std::unordered_map<SDL_EventType, std::unordered_map<EventEmitter*, std::unordered_set<EventCallback>>> general_events;
			void register_keypress_event(SDL_Keycode keycode, bool down, EventCallback event, EventEmitter* emitter);
			void register_mouse_left_button_event(EventCallback event, EventEmitter* emitter, bool down);
			void register_mouse_right_button_event(EventCallback event, EventEmitter* emitter, bool down);
			void register_mouse_middle_button_event(EventCallback event, EventEmitter* emitter, bool down);
			void register_general_event(SDL_EventType type, EventCallback event, EventEmitter* emitter);
			void manage_event_callbacks(SDL_Event& event);
	};

	// ResourceBag section
	template<class T>
	class ResourceBeacon : public ResourceBeaconParent
	{
		public:
			ResourceBeacon()
			{
				if (App::__app_beacons == nullptr)
					App::__app_beacons = new S__app_beacons; // this pointer is never freed
				App::__app_beacons->beacons.push_back(this);
			}

			T* bag = nullptr;

			friend class App;
		private:
			posta::ResourceBag* _bag = nullptr;
			void init()
			{
				_bag = new T();
				posta::App::app->resource_bag->add_bag(_bag);
				bag = dynamic_cast<T*>(_bag);
			}
	};

} // namespace posta

#endif // POSTAENGINE_APP_H


