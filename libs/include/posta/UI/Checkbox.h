#ifndef POSTAENGINE_CHECKBOX_UI_H
#define POSTAENGINE_CHECKBOX_UI_H
#include <engine/include/UI/Button.h>
#include <optional>

namespace Engine::UI {
	/// A checkbox class
	class Checkbox
	{
		public:
			void toggle()
			{
				state = !state;
			}
			
			Checkbox(Rect rect) :
				state(false),
				button(rect)
			{}
			Checkbox(const Checkbox& other) :
				state(other.state),
				button(other.button)
			{}
			Checkbox& operator=(const Checkbox& other) = delete;

			bool is_checked()
			{
				return state;
			}

			bool loop()
			{
				return loop(0, 0);
			}

			bool loop(int x_offset, int y_offset)
			{
				if (button.loop(x_offset, y_offset))
				{
					this->toggle();
					return true;
				}
				return false;
			}

			bool loop(int x_offset, int y_offset, Rect rect)
			{
				if (button.loop(x_offset, y_offset, rect))
				{
					this->toggle();
					return true;
				}
				return false;
			}

			Rect& get_rect()
			{
				return button.rect;
			}

			void set_rect(Rect rect)
			{
				button.rect = rect;
			}

			bool is_highlighted()
			{
				return button.is_highlighted();
			}

			bool state;
		private:
			Button button;
	};

	/// A group of checkbox, only one of the checkbox can be checked at the same time
	class CheckboxGroup
	{
		public:
			void loop()
			{
				loop(0, 0);
			}

			void loop(int x_offset, int y_offset)
			{
				auto checked_it = get_checked();
				if (checked_it != checkboxes.end())
					checked_it->state = false;

				for (auto& item : checkboxes)
					item.loop(x_offset, y_offset);

				auto checked_jt = get_checked();
				if (checked_jt == checkboxes.end())
				{
					if (checked_it != checkboxes.end())
						checked_it->state = true;
					return;
				}
				for (auto& item : checkboxes)
					item.state = false;
				checked_jt->state = true;
			}

			void loop(int x_offset, int y_offset, Rect rect)
			{
				auto checked_it = get_checked();
				if (checked_it != checkboxes.end())
					checked_it->state = false;

				for (auto& item : checkboxes)
					item.loop(x_offset, y_offset, rect);

				auto checked_jt = get_checked();
				if (checked_jt == checkboxes.end())
				{
					if (checked_it != checkboxes.end())
						checked_it->state = true;
					return;
				}
				for (auto& item : checkboxes)
					item.state = false;
				checked_jt->state = true;
			}

			void add_checkbox(Checkbox checkbox)
			{
				checkboxes.push_back(checkbox);
			}

			/// Returns a iterator of checkbox that points to the location of the checkbox checked, if no checkbox was checked returns checkboxes.end()
			std::vector<Checkbox>::iterator get_checked()
			{
				for (auto it = checkboxes.begin(); it != checkboxes.end(); it++)
				{
					if (it->is_checked())
						return it;
				}
				return checkboxes.end();
			}
			/// Returns an optional that may or may not contain the index of the checked checkbox
			std::optional<size_t> get_checked_index()
			{
				for (size_t i = 0; i < checkboxes.size(); i++)
				{
					if (checkboxes[i].is_checked())
						return i;
				}
				return std::optional<size_t>();
			}
			std::vector<Checkbox> checkboxes;
	};

}

#endif // POSTAENGINE_CHECKBOX_UI_H
