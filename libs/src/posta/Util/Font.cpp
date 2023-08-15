#include <engine/include/Util/Font.h>

using Engine::Font;

Font::Font(std::filesystem::path path)
{
	filepath = path;
}

Font::~Font()
{
	for (auto& font : fonts)
		TTF_CloseFont(font.second._font);
}

Engine::Component::Texture* Font::render_text(std::string text, size_t font_size, int* w, int* h)
{
	auto it = get_iterator_for_font(font_size);
	int u, v;
	int* width = &u;
	int* height = &v;
	if (w) width = w;
	if (h) height = h;
	return it->second.render_text(text, font_size, width, height);
}

Engine::Component::Image* Font::render_image(std::string text, size_t font_size)
{
	if (text.empty())
		text = " ";
	int w, h;
	Engine::Component::Texture* texture = render_text(text, font_size, &w, &h);
	return new Engine::Component::Image(texture, w, h);
}

int Font::get_text_size(std::string text, size_t font_size, int& w, int& h)
{
	return get_iterator_for_font(font_size)->second.get_text_size(text, w, h);
}

int Font::get_character_width(size_t font_size)
{
	return get_iterator_for_font(font_size)->second.char_width;
}

int Font::get_character_height(size_t font_size)
{
	return get_iterator_for_font(font_size)->second.char_height;
}

size_t Font::size_utf8(std::string text)
{
	const char* s = text.c_str();
	size_t len = 0;
	while (*s) len += (*s++ & 0xc0) != 0x80; // copied from somewhere else, may be wrong
	return len;
}

std::unordered_map<size_t, Font::InternalFont>::iterator Font::get_iterator_for_font(size_t font_size)
{
	auto it = fonts.find(font_size);
	if (it == fonts.end())
	{
		fonts[font_size] = InternalFont(filepath, font_size);
		it = fonts.find(font_size);
	}
	return it;
}

Font::InternalFont::InternalFont(std::filesystem::path path, size_t font_size)
{
	//std::cout << "ttf inits: " << TTF_WasInit() << std::endl;
	_font = TTF_OpenFont(path.string().c_str(), font_size);
	if (_font == nullptr)
		throw std::logic_error(std::string("couldn't open font: ") + path.string());
	TTF_SizeUTF8(_font, " ", &char_width, &char_height);
}

Engine::Component::Texture* Font::InternalFont::render_text(std::string text, size_t font_size, int* w, int* h)
{
	//std::cout << "ttf inits2: " << TTF_WasInit() << std::endl;
	//std::cout << "a " << _font << std::endl;
	SDL_Surface* img = TTF_RenderUTF8_Blended(_font, text.c_str(), SDL_Color{255, 255, 255, 255});
	(*w) = img->w;
	(*h) = img->h;
	//std::cout << "b" << std::endl;
	Engine::Component::Texture* texture = new Engine::Component::Texture(img);
	SDL_FreeSurface(img);
	//std::cout << "c" << std::endl;
	return texture;
}

int Font::InternalFont::get_text_size(std::string text, int& w, int& h)
{
	return TTF_SizeUTF8(_font, text.c_str(), &w, &h);
}

