#ifndef POSTAENGINE_TEXTBOX_H
#define POSTAENGINE_TEXTBOX_H
#include <posta/UI/Rect.h>
#include <posta/UI/Button.h>
#include <posta/Util/Font.h>

namespace posta::ui {
	/// A box with a text on it
	class Textbox
	{
		public:
			/// Constructs a textbox from a rectangle and a font, the font must be alive for as long as it is clickable
			Textbox(Rect rect, posta::Font* _font, int _font_size) :
				has_change(true),
				font_size(_font_size),
				state(false),
				cursor_pos(0),
				font(_font),
				button(rect)
			{}
			Textbox(Textbox& textbox) :
				has_change(true),
				font_size(textbox.font_size),
				state(false),
				cursor_pos(0),
				font(textbox.get_font_ptr()),
				button(textbox.get_rect())
			{}
			Textbox& operator=(const Textbox&) = delete;

			void loop(int x_offset, int y_offset);
			void loop(int x_offset, int y_offset, Rect rect);

			void set_text(std::string _text);
			std::string get_text() const;

			/// Selects the textbox(state = true) and sets the cursor position to pos
			void select(size_t pos);
			/// Unselects the textbox(state = false)
			void unselect();
			/// True if selected, false if not
			bool is_selected();

			Rect& get_rect();
			void set_rect(Rect rect);

			posta::Font* get_font_ptr();

			/// Inserts str in the cursor_pos
			void add_text(std::string str);
			/// Moves the cursor to the next character
			void next_cursor_pos();
			/// Moves the cursor to the previous character
			void prev_cursor_pos();
			/// Erases character on cursor position
			void erase_character();
			/// Erases the character next to the cursor position
			void erase_next_character();

			/// Erases last character
			void erase_last_character();

			/// Moves cursor to the end of the string
			void move_cursor_to_the_end();

			/// Returns, in pixels, the distance to the cursor from the origin of the textbox
			int get_cursor_offset();

			/// Sets the cursor position to that of the mouse
			/// \param scale_width If set to true, the width of the text will be scaled to match the width of the rect of this textbox, if not the width of the text will be measure in pixels from the origin.
			void set_cursor_pos_to_mouse_pos(bool scale_width);

			/// True if text is modified when add_text, erase_character, or erase_next_character are called
			bool has_change;
			int font_size;
		private:
			/// True if selected, false if not
			bool state;
			/// Cursor position (in characters), 0 means the cursor is in the first character
			size_t cursor_pos;
			posta::Font* font;
			/// Stores the shape of the textbox and its responsible for making the textbox selectable, its callback should not be changed
			Button button;
			std::string text;
			/// Selects the textbox(state = true) and sets the cursor position to that of the mouse
			void select();
			int _x_offset;
	};
}

#endif // POSTAENGINE_TEXTBOX_H
