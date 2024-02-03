#include "Text.h"

/*std::unordered_map<size_t, std::unique_ptr<Text::Font>> Text::fonts;

Text::Font::Font(TTF_Font* font)
{
	_font = font;
}

Text::Font::~Font()
{
	TTF_CloseFont(_font);
}
*/
Text::Text(std::string str, Engine::Font& font, size_t font_size)
{
	reload(str, font, font_size);
}

void Text::reload(std::string str, Engine::Font& font, size_t font_size)
{
	//if (!fonts[font_size])
	//	fonts[font_size].reset(new Font(TTF_OpenFont("assets/OxygenMono-Regular.ttf", font_size)));
	//SDL_Surface* img = TTF_RenderUTF8_Blended(fonts[font_size]->_font, str.c_str(), SDL_Color{255, 255, 255, 255});
	//image = std::make_unique<Engine::Component::Image>(new Engine::Component::Texture(img), img->w, img->h);
	//SDL_FreeSurface(img);
	image.reset(font.render_image(str, font_size));
	//LOG(str, " ", image->w, " ", image->h);
}

//void Text::draw(BasicShader& shader, int x, int y, glm::vec4 color)
void Text::draw(Engine::TwoDShader& shader, int x, int y, glm::vec4 color)
{
	//shader.is_2d = true;
	shader.global_color = color;
	shader.model = image->get_matrix(x, y);
	image->draw();
	//shader.is_2d = false;
}

