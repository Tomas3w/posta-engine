#include "engine/include/Component/Image.h"
#include <engine/include/App.h>

using Engine::Component::Image;

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
	/*if (!App::mesh2d)
	{
		throw std::logic_error("this shouldn't be ran"); // DEBUG
		App::mesh2d.reset(new Engine::Component::StaticMesh(Engine::Assets::load_obj("assets/mesh2d.obj")));
	}*/
}

glm::mat4 Image::get_matrix(int x, int y)
{
	float width = Engine::App::app->get_width();
	float height = Engine::App::app->get_height();
	// 1 --- width / 2
	/*bool is_w_odd = w % 2 != 0;
	bool is_h_odd = h % 2 != 0;
	if (is_w_odd) w += 1;
	if (is_h_odd) h += 1;*/
	//x = y = 0;
	glm::mat4 r = glm::translate(glm::mat4(1.0f), glm::vec3(x / (width / 2.0f) - 1 + (w / 2.0f / (width / 2.0f)), y / (height / 2.0f) - 1 + (h / 2.0f / (height / 2.0f)), 0)) * glm::scale(glm::mat4(1.0f), glm::vec3(w / width, h / height, 1.0f));
	/*if (is_w_odd) w -= 1;
	if (is_h_odd) h -= 1;*/
	return r;
}

glm::mat4 Image::get_matrix_with_size(int x, int y, int _w, int _h)
{
	int aux_w = w;
	int aux_h = h;
	w = _w;
	h = _h;
	auto r = get_matrix(x, y);
	w = aux_w;
	h = aux_h;
	return r;
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

