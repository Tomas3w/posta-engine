#include <posta/Util/EventEmitter.h>

using posta::EventEmitter;
using EventCallback = posta::App::EventCallback;

EventEmitter::~EventEmitter()
{
	for (auto event : events)
		remove_event(event);
}

void EventEmitter::register_event(SDL_Keycode keycode, bool keydown, EventCallback event)
{
	posta::App::app->register_keypress_event(keycode, keydown, event, this);
}

void EventEmitter::register_event(MouseButton mouse_button, bool mousedown, EventCallback event)
{
	switch (mouse_button)
	{
		case MouseButton::LEFT:
			posta::App::app->register_mouse_left_button_event(event, this, mousedown);
			break;
		case MouseButton::RIGHT:
			posta::App::app->register_mouse_right_button_event(event, this, mousedown);
			break;
		case MouseButton::MIDDLE:
			posta::App::app->register_mouse_middle_button_event(event, this, mousedown);
			break;
	}
}

void EventEmitter::register_event(SDL_EventType type, EventCallback event)
{
	posta::App::app->register_general_event(type, event, this);
}

void EventEmitter::remove_event(EventCallback event)
{
	for (auto& p : posta::App::app->key_events)
	{
		if (p.second[0][this].erase(event))
		{
			events.erase(event);
			return;
		}
		if (p.second[1][this].erase(event))
		{
			events.erase(event);
			return;
		}
	}
	for (auto& s : posta::App::app->mouse_left_button_events)
	{
		if (s[this].erase(event))
		{
			events.erase(event);
			return;
		}
	}
	for (auto& s : posta::App::app->mouse_right_button_events)
	{
		if (s[this].erase(event))
		{
			events.erase(event);
			return;
		}
	}
	for (auto& s : posta::App::app->mouse_middle_button_events)
	{
		if (s[this].erase(event))
		{
			events.erase(event);
			return;
		}
	}
	for (auto& p : posta::App::app->general_events)
	{
		if (p.second[this].erase(event))
		{
			events.erase(event);
			return;
		}
	}
}

