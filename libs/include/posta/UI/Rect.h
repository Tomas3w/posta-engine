#ifndef POSTAENGINE_RECT_UI_H
#define POSTAENGINE_RECT_UI_H
#include <SDL2/SDL.h>
#include <glm/glm.hpp>

namespace posta::ui {
	class Rect : public SDL_Rect
	{
	public:
		Rect()
		{
			x = y = w = h = 0;
		}
		Rect(glm::vec4 rect)
		{
			x = static_cast<int>(rect.x);
			y = static_cast<int>(rect.y);
			w = static_cast<int>(rect.z);
			h = static_cast<int>(rect.w);
		}
		Rect(int _x, int _y, int _w, int _h)
		{
			x = _x;
			y = _y;
			w = _w;
			h = _h;
		}
		Rect(SDL_Rect r)
		{
			x = r.x;
			y = r.y;
			w = r.w;
			h = r.h;
		}

		Rect intersect(Rect rect)
		{
			SDL_Rect result;
			SDL_IntersectRect(this, &rect, &result);
			return posta::ui::Rect(result);
		}

		int get_x()
		{
			return x;
		}
		int get_y()
		{
			return y;
		}
		int get_w()
		{
			return w;
		}
		int get_h()
		{
			return h;
		}
		Rect& set_x(int _x)
		{
			x = _x;
			return *this;
		}
		Rect& set_y(int _y)
		{
			y = _y;
			return *this;
		}
		Rect& set_w(int _w)
		{
			w = _w;
			return *this;
		}
		Rect& set_h(int _h)
		{
			h = _h;
			return *this;
		}
		bool has_inside(int x, int y){
			return (x >= this->x && x <= (this->x + this->w) && y >= this->y && y <= (this->y + this->h));
		}
	};
}

#endif // POSTAENGINE_RECT_UI_H
