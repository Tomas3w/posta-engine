#ifndef POSTAENGINE_FONT_UTILITY_H
#define POSTAENGINE_FONT_UTILITY_H
#include <SDL2/SDL_ttf.h>
#include <filesystem>
#include <posta/Component/Texture.h>
#include <posta/Component/Image.h>
#include <unordered_map>

namespace Engine {
	/// A font class, used to draw text into textures and images
	class Font
	{
		public:
			/// Creates a font from a .ttf file, the .ttf file must not be removed until all of the font objects are freed
			Font(std::filesystem::path path);
			~Font();
			Font(const Font&) = delete;
			Font& operator=(const Font&) = delete;

			/// Returns a texture with a text on it
			/// \param text text to be render onto the texture
			/// \param font_size size of the font, the final resolution of a given font size depends upon the font
			/// \param w width in pixels, optional parameter, defaults to null
			/// \param h height in pixels, optional parameter, defaults to null
			Engine::Component::Texture* render_text(std::string text, size_t font_size, int* w = nullptr, int* h = nullptr);

			/// Returns an image object with a text on it
			Engine::Component::Image* render_image(std::string text, size_t font_size);

			/// Returns 0 on success, -1 on failure
			int get_text_size(std::string text, size_t font_size, int& w, int& h);

			/// Returns the width of the space character, usefull for monospace fonts
			int get_character_width(size_t font_size);

			/// Returns the height of the space character
			int get_character_height(size_t font_size);

			/// Returns the number of characters of a utf-8 encoded string
			static size_t size_utf8(std::string);
		private:
			std::filesystem::path filepath;
			struct InternalFont
			{
				InternalFont() = default;
				InternalFont(std::filesystem::path path, size_t font_size);
				Engine::Component::Texture* render_text(std::string text, size_t font_size, int* w, int* h);
				/// Returns 0 on success, -1 on failure
				int get_text_size(std::string text, int& w, int& h);
				
				TTF_Font* _font = nullptr;
				int char_width, char_height; // size of the character ' ', usefull for monospace fonts
			};
			std::unordered_map<size_t, InternalFont>::iterator get_iterator_for_font(size_t font_size);
			std::unordered_map<size_t, InternalFont> fonts;
	};
}

#endif // POSTAENGINE_FONT_UTILITY_H
