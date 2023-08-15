#include <posta/Component/Image.h>
#include <posta/App.h>

using Engine::Component::Image;

glm::mat4 Image::matrix_for_rect(Engine::UI::Rect rect, int screen_w, int screen_h)
{
	float width = screen_w;
	float height = screen_h;
	glm::mat4 r = glm::translate(glm::mat4(1.0f), glm::vec3(rect.x / (width / 2.0f) - 1 + (rect.w / 2.0f / (width / 2.0f)), rect.y / (height / 2.0f) - 1 + (rect.h / 2.0f / (height / 2.0f)), 0)) * glm::scale(glm::mat4(1.0f), glm::vec3(rect.w / width, rect.h / height, 1.0f));
	return r;
}

void Image::draw_rect(Engine::Component::Texture* texture)
{
	texture->bind();
	App::mesh2d->draw();
}

Image::Image(std::filesystem::path path)
{
	SDL_Surface* img = IMG_Load(path.string().c_str());
	if (img == nullptr)
		throw std::logic_error(std::string("couldn't find texture file (while constructing Image): '") + path.string().c_str() + "'");
	w = img->w;
	h = img->h;
	texture.reset(new Engine::Component::Texture(img));
	SDL_FreeSurface(img);
}

Image::Image(Engine::Component::Texture* texture, int w, int h)
{
	this->texture.reset(texture);
	this->w = w;
	this->h = h;
}

glm::mat4 Image::get_matrix(int x, int y)
{
	return matrix_for_rect(Engine::UI::Rect(x, y, w, h), Engine::App::app->get_width(), Engine::App::app->get_height());
}

glm::mat4 Image::get_matrix_with_size(int x, int y, int _w, int _h)
{
	return matrix_for_rect(Engine::UI::Rect(x, y, _w, _h), Engine::App::app->get_width(), Engine::App::app->get_height());
}

glm::mat4 Image::get_matrix_with_size(Engine::UI::Rect rect)
{
	return get_matrix_with_size(rect.x, rect.y, rect.w, rect.h);
}

int Image::get_w()
{
	return w;
}

int Image::get_h()
{
	return h;
}

void Image::set_w(int _w)
{
	w = _w;
}

void Image::set_h(int _h)
{
	h = _h;
}

void Image::draw()
{
	texture->bind();
	App::mesh2d->draw();
}

void Image::only_bind()
{
	texture->bind();
	App::mesh2d->bind();
}

void Image::only_draw()
{
	App::mesh2d->draw_without_binding();
}

