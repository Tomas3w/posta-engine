#include <posta/UI/Textbox.h>
#include <posta/App.h>
#include <posta/Util/LoggingMacro.h>

using posta::ui::Textbox;

void Textbox::loop(int x_offset, int y_offset)
{
	_x_offset = x_offset;
	if (button.loop(x_offset, y_offset))
		select();
}

void Textbox::loop(int x_offset, int y_offset, Rect rect)
{
	_x_offset = x_offset;
	if (button.loop(x_offset, y_offset, rect))
		select();
}

void Textbox::set_text(std::string _text)
{
	text = _text;
	cursor_pos = std::min(cursor_pos, text.size());
}

std::string Textbox::get_text() const
{
	return text;
}

void Textbox::set_cursor_pos_to_mouse_pos(bool scale_width)
{
	if (!scale_width)
	{
		select();
		return;
	}
	int& mouse_x = posta::App::app->mouse_x;
	int nx = button.rect.x + _x_offset;
	int dnx = mouse_x - nx;
	int text_width, h;
	font->get_text_size(text, font_size, text_width, h);
	if (dnx >= 0 && dnx < button.rect.w)
	{
		state = true;
		size_t ncursor_pos = 0; // index of closest
		int dt = dnx; // distance of closest
		for (cursor_pos = 0; cursor_pos < text.size();)
		{
			int w;
			next_cursor_pos();
			std::string n = text.substr(0, cursor_pos);
			if (font->get_text_size(n, font_size, w, h) == 0)
			{
				w = static_cast<float>(w) * button.rect.w / text_width;
				if (abs(dnx - w) < dt)
				{
					ncursor_pos = cursor_pos;
					dt = abs(dnx - w);
				}
			}
		}
		cursor_pos = ncursor_pos;
	}
}

void Textbox::select()
{
	int& mouse_x = posta::App::app->mouse_x;
	int nx = button.rect.x + _x_offset;
	int dnx = mouse_x - nx;
	if (dnx >= 0 && dnx < button.rect.w)
	{
		state = true;
		size_t ncursor_pos = 0; // index of closest
		int dt = dnx; // distance of closest
		for (cursor_pos = 0; cursor_pos < text.size();)
		{
			int w, h;
			next_cursor_pos();
			std::string n = text.substr(0, cursor_pos);
			if (font->get_text_size(n, font_size, w, h) == 0)
			{
				if (abs(dnx - w) < dt)
				{
					ncursor_pos = cursor_pos;
					dt = abs(dnx - w);
				}
			}
		}
		cursor_pos = ncursor_pos;
	}
}

void Textbox::select(size_t pos)
{
	state = true;
	cursor_pos = 0;
	for (size_t i = 0; i < pos; i++)
		next_cursor_pos();
}

void Textbox::unselect()
{
	state = false;
}

bool Textbox::is_selected()
{
	return state;
}

posta::ui::Rect& Textbox::get_rect()
{
	return button.rect;
}

void Textbox::set_rect(Rect rect)
{
	button.rect = rect;
}

posta::Font* Textbox::get_font_ptr()
{
	return font;
}

void Textbox::add_text(std::string str)
{
	text.insert(cursor_pos, str);
	cursor_pos += str.size();
	has_change = true;
}

void Textbox::next_cursor_pos()
{
	for (size_t i = cursor_pos + 1; i < text.size(); i++)
	{
		if (!(((1 << 7) & text[i]) && ((1 << 6) & ~text[i])))
		{
			cursor_pos = i;
			return;
		}
	}
	cursor_pos = text.size();
}

void Textbox::prev_cursor_pos()
{
	if (cursor_pos == 0)
		return;
	for (size_t i = cursor_pos - 1; i != 0; i--)
	{
		if (!(((1 << 7) & text[i]) && ((1 << 6) & ~text[i])))
		{
			cursor_pos = i;
			return;
		}
	}
	cursor_pos = 0;
}

void Textbox::erase_character()
{
	if (cursor_pos > 0)
	{
		prev_cursor_pos();
		int tokeep_left = cursor_pos;
		next_cursor_pos();
		text = text.substr(0, tokeep_left) + text.substr(cursor_pos);
		cursor_pos = tokeep_left;
		has_change = true;
	}
}

void Textbox::erase_next_character()
{
	if (cursor_pos < text.size())
	{
		int tokeep_left = cursor_pos;
		next_cursor_pos();
		int tokeep_right = cursor_pos;
		prev_cursor_pos();
		text = text.substr(0, tokeep_left) + text.substr(tokeep_right);
		has_change = true;
	}
}

void Textbox::erase_last_character()
{
	size_t last_cursor_pos = cursor_pos;
	cursor_pos = text.size();
	erase_character();
	cursor_pos = std::min(last_cursor_pos, text.size());
}

int Textbox::get_cursor_offset()
{
	int w, h;
	font->get_text_size(text.substr(0, cursor_pos), font_size, w, h);
	return w;
}

