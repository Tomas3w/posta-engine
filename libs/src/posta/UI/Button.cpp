#include <posta/UI/Button.h>
#include <posta/App.h>

using posta::ui::Button;

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
	int& mouse_x = posta::App::app->mouse_x;
	int mouse_y = posta::App::app->mouse_y;
	int nx = rect.x + x_offset;
	int ny = rect.y + y_offset;
	highlighted = (mouse_x >= nx && mouse_x < nx + rect.w && mouse_y >= ny && mouse_y < ny + rect.h);
	bool r = false;
	if (highlighted)
	{
		if (posta::App::app->is_click_pressed())
			press();
		else if (pressed)
		{
			r = true;
			pressed = false;
		}
	}
	else if (!posta::App::app->is_click_pressed())
		pressed = false;
	return r || is_click_reserved();
}

bool Button::loop(int x_offset, int y_offset, Rect _rect)
{
	SDL_Rect a = *dynamic_cast<SDL_Rect*>(&rect);
	SDL_Rect b;
	SDL_IntersectRect(&a, dynamic_cast<SDL_Rect*>(&_rect), &b);
	rect = posta::ui::Rect(b);
	bool r = loop(x_offset, y_offset);
	rect = posta::ui::Rect(a);
	return r;
}

posta::ui::Rect& Button::get_rect()
{
	return rect;
}

void Button::set_rect(Rect _rect)
{
	rect = _rect;
}

bool Button::is_pressed() const
{
	return pressed;
}

bool Button::is_highlighted() const
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

void Button::click()
{
	reserved_click = true;
}

bool Button::is_click_reserved()
{
	if (reserved_click)
	{
		reserved_click = false;
		return true;
	}
	return false;
}

