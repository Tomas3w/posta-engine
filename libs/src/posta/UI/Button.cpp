#include <engine/include/UI/Button.h>
#include <engine/include/App.h>

using Engine::UI::Button;

Button::Button(Rect _rect)
{
	rect = _rect;
}

bool Button::loop()
{
	return loop(0, 0);
}

bool Button::loop(int x_offset, int y_offset)
{
	int& mouse_x = Engine::App::app->mouse_x;
	int mouse_y = Engine::App::app->mouse_y;
	int nx = rect.x + x_offset;
	int ny = rect.y + y_offset;
	highlighted = (mouse_x >= nx && mouse_x < nx + rect.w && mouse_y >= ny && mouse_y < ny + rect.h);
	bool r = false;
	if (highlighted)
	{
		if (Engine::App::app->is_click_pressed())
			press();
		else if (pressed)
		{
			r = true;
			pressed = false;
		}
	}
	else if (!Engine::App::app->is_click_pressed())
		pressed = false;
	return r;
}

bool Button::loop(int x_offset, int y_offset, Rect _rect)
{
	SDL_Rect a = *dynamic_cast<SDL_Rect*>(&rect);
	SDL_Rect b;
	SDL_IntersectRect(&a, dynamic_cast<SDL_Rect*>(&_rect), &b);
	rect = Engine::UI::Rect(b);
	bool r = loop(x_offset, y_offset);
	rect = Engine::UI::Rect(a);
	return r;
}

Engine::UI::Rect& Button::get_rect()
{
	return rect;
}

void Button::set_rect(Rect _rect)
{
	rect = _rect;
}

bool Button::is_pressed()
{
	return pressed;
}

bool Button::is_highlighted()
{
	return highlighted;
}

void Button::press()
{
	pressed = true;
}

bool Button::release()
{
	if (pressed)
	{
		pressed = false;
		return true;
	}
	return false;
}

