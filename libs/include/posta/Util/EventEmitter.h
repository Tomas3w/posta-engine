#ifndef POSTAENGINE_EVENTEMITTER_UTILITY_H
#define POSTAENGINE_EVENTEMITTER_UTILITY_H
#include <posta/App.h>

namespace posta {
	/// Derive from this class to register event callbacks
	class EventEmitter
	{
		public:
			using EventCallback = App::EventCallback;
			enum class MouseButton
			{
				LEFT,
				RIGHT,
				MIDDLE,
			};
			~EventEmitter();

			/// Registers a key press from the keyboard
			/// \param keydown true if the event should be triggered at keydown or false in case of triggering at keyup
			void register_event(SDL_Keycode keycode, bool keydown, EventCallback event);
			/// Registers a mouse button press
			/// \param keydown true if the event should be triggered at button press or false in case of triggering at button release
			void register_event(MouseButton mouse_button, bool mousedown, EventCallback event);
			/// Registers any event
			void register_event(SDL_EventType type, EventCallback event);

			/// Removes an event, the event must be the same one that was registered
			void remove_event(EventCallback event);
		private:
			std::unordered_set<EventCallback> events;
	};
}

#endif // POSTAENGINE_EVENTEMITTER_UTILITY_H
