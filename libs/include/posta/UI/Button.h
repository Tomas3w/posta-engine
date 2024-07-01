#ifndef POSTAENGINE_BUTTON_UI_H
#define POSTAENGINE_BUTTON_UI_H
#include <functional>
#include <posta/UI/Rect.h>

namespace posta::ui {
	/// A button class, used to represent a button as a rectangle in the screen space
	class Button
	{
		public:
			/// Constructs a button
			Button(Rect _rect);
			Button() = default;

			/// Overload of loop with x_offset = y_offset = 0
			bool loop();
			/// Updates the button state
			/// \param x_offset a horizontal offset in pixels to apply to the button final position
			/// \param y_offset a vertical offset in pixels to apply to the button final position
			/// \return whether the button was pressed
			bool loop(int x_offset, int y_offset);
			
			// Overload of loop whose click is given by the bool is_click_pressed
			bool loop(int x_offset, int y_offset, bool is_click_pressed);

			/// Overload of loop that pretends that the interactive part of the button is now the intersection between the button rectangle with the rectangle given by _rect
			bool loop(int x_offset, int y_offset, Rect _rect);
			/// Overload of loop that pretends that the interactive part of the button is now the intersection between the button rectangle with the rectangle given by _rect, it also uses the bool is_click_pressed instead of just click
			bool loop(int x_offset, int y_offset, Rect _rect, bool is_click_pressed);

			Rect& get_rect();
			void set_rect(Rect _rect);
			Rect rect;

			/// Return whether the button is pressed or not
			bool is_pressed() const;
			/// Highlight state of the button
			/// True if mouse is in button space
			bool is_highlighted() const;

			/// Emulates pressing the button
			void press();
			/// Emulates releasing the button, returns true if button was already pressed
			bool release();
			/// Returns true on the next called to one of the overloads of loop
			void click();
		private:
			bool is_click_reserved();
			bool reserved_click = false;
			bool pressed = false;
			bool highlighted = false;
	};
}

#endif // POSTAENGINE_BUTTON_UI_H
