#ifndef TEXT_H
#define TEXT_H
#include <engine/include/App.h>
#include <engine/include/Component/Image.h>
#include <engine/include/Util/Font.h>
#include <engine/include/Util/LoggingMacro.h>

class Text
{
public:
	/*
	struct Font
	{
		Font(TTF_Font* font);
		~Font();
		TTF_Font* _font;
	};

	static std::unordered_map<size_t, std::unique_ptr<Font>> fonts;*/
	/// Creates a text with the resolution provided by the font size
	Text(std::string str, Engine::Font& font, size_t font_size = 24);

	void reload(std::string str, Engine::Font& font, size_t font_size = 24);

	//void draw(BasicShader& shader, int x, int y, glm::vec4 color = {1, 1, 1, 1});
	void draw(Engine::TwoDShader& shader, int x, int y, glm::vec4 color = {1, 1, 1, 1});
	std::unique_ptr<Engine::Component::Image> image;
};

#endif // TEXT_H
