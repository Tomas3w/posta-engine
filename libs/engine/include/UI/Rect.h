#ifndef POSTAENGINE_RECT_UI_H
#define POSTAENGINE_RECT_UI_H
#include <SDL2/SDL.h>

namespace Engine::UI {
	class Rect : public SDL_Rect
	{
	public:
		Rect()
		{
			x = y = w = h = 0;
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
			return Engine::UI::Rect(result);
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
		void set_x(int _x)
		{
			x = _x;
		}
		void set_y(int _y)
		{
			y = _y;
		}
		void set_w(int _w)
		{
			w = _w;
		}
		void set_h(int _h)
		{
			h = _h;
		}
	};
}

#endif // POSTAENGINE_RECT_UI_H
