#include "Image.h"

std::unique_ptr<Engine::Component::StaticMesh> Image::mesh2d;

Image::Image(Engine::Component::Texture* texture, int w, int h)
{
	this->texture.reset(texture);
	this->w = w;
	this->h = h;
	if (!mesh2d)
		mesh2d.reset(new Engine::Component::StaticMesh(Engine::Assets::load_obj("assets/mesh2d.obj")));
}

glm::mat4 Image::get_matrix(int x, int y)
{
	float width = Engine::App::app->get_width();
	float height = Engine::App::app->get_height();
	return glm::translate(glm::mat4(1.0f), glm::vec3(x / width, y / height, 0)) * glm::scale(glm::mat4(1.0f), glm::vec3(w / width, h / height, 1.0f));
}

void Image::draw()
{
	texture->bind();
	mesh2d->draw();
}

