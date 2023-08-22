#include <posta/Entity/Entity.h>
#include <posta/App.h>
#include <posta/Util/LoggingMacro.h>

using posta::entity::Entity;
using posta::entity::EmptyType;
using posta::entity::CameraType;

std::unordered_set<Entity*> posta::entity::entities;

#ifndef POSTA_EDITOR_DISABLED

EmptyType::EmptyType(posta::component::Transform* _transform)
{
	transform = _transform;
}

void EmptyType::draw_front()
{
	Entity::draw_transform(*transform);
}

CameraType::CameraType(posta::component::Transform* _transform, std::unique_ptr<posta::component::Camera>* _camera, int* _width, int* _height)
{
	transform = _transform;
	camera = _camera;
	width = _width;
	height = _height;
}

void CameraType::draw_front()
{
	if (*camera)
		Entity::draw_camera_outline(*transform, camera->get(), framebuffer, *width, *height);
}

Entity::EditorGraphics::EditorGraphics() :
	arrow(posta::assets::load_obj("common/editor/arrow.obj")),
	blank_texture("common/blank.png")
{}

std::unique_ptr<Entity::EditorGraphics> Entity::__graphics;
Entity::EditorGraphics* Entity::graphics()
{
	if (!__graphics)
		__graphics.reset(new EditorGraphics);
	return __graphics.get();
}

void Entity::__free_graphics()
{
	__graphics.reset();
}

posta::entity::Type* Entity::get_internal_entity_type()
{
	return type.get();
}

void Entity::draw_transform(posta::component::Transform transform)
{
	transform.set_scale(glm::vec3(glm::length(transform.get_position() - posta::App::app->editor_camera->transform.get_position()) / 10.0f));
	posta::App::app->clear_depth();
	const glm::vec3 scale = transform.get_scale();
	transform.set_scale({scale.x / 3, scale.y / 3, scale.z});
	graphics()->blank_texture.bind();
	graphics()->shader.bind();
	graphics()->shader.width = posta::App::app->get_width();
	graphics()->shader.height = posta::App::app->get_height();
	graphics()->shader.outline = false;
	graphics()->shader.global_color = {0, 0, 1, 1};
	graphics()->shader.model = transform.get_matrix();
	graphics()->shader.normal_model = transform.get_normal_matrix();
	graphics()->shader.projection_view = posta::App::app->camera->get_projection_view_matrix();
	graphics()->arrow.draw();
	transform.set_rotation(glm::rotate(transform.get_rotation(), (float)M_PI / 2, {0, 1, 0}));
	graphics()->shader.global_color = {1, 0, 0, 1};
	graphics()->shader.model = transform.get_matrix();
	graphics()->shader.normal_model = transform.get_normal_matrix();
	graphics()->shader.projection_view = posta::App::app->camera->get_projection_view_matrix();
	graphics()->arrow.draw();
	transform.set_rotation(glm::rotate(transform.get_rotation(), -(float)M_PI / 2, {1, 0, 0}));
	graphics()->shader.global_color = {0, 1, 0, 1};
	graphics()->shader.model = transform.get_matrix();
	graphics()->shader.normal_model = transform.get_normal_matrix();
	graphics()->shader.projection_view = posta::App::app->camera->get_projection_view_matrix();
	graphics()->arrow.draw();
}

void Entity::draw_camera_outline(posta::component::Transform transform, posta::component::Camera* camera, posta::Framebuffer* framebuffer, int width, int height)
{
	draw_transform(transform);

	glDisable(GL_CULL_FACE);
	// Drawing near plane
	transform.set_position(transform.get_position() - transform.front() * 0.01f);
	float length = glm::length(glm::vec2(width, height));
	transform.set_scale({(width / length) / 4.0f, (height / length) / 4.0f, 1});
	graphics()->blank_texture.bind();
	graphics()->shader.bind();
	graphics()->shader.width = posta::App::app->get_width();
	graphics()->shader.height = posta::App::app->get_height();
	graphics()->shader.global_color = {1, 1, 1, 1};
	graphics()->shader.outline = true;
	graphics()->shader.model = transform.get_matrix();
	graphics()->shader.normal_model = transform.get_normal_matrix();
	graphics()->shader.projection_view = posta::App::app->camera->get_projection_view_matrix();
	posta::App::app->mesh2d->draw();

	// Drawing far plane
	transform.set_position(transform.get_position() - transform.front() * 0.55f);
	transform.set_scale(transform.get_scale() * 2.0f);
	graphics()->shader.model = transform.get_matrix();
	graphics()->shader.normal_model = transform.get_normal_matrix();
	graphics()->shader.projection_view = posta::App::app->camera->get_projection_view_matrix();
	posta::App::app->mesh2d->draw();
	// Drawing framebuffer and enabling culling faces
	glEnable(GL_CULL_FACE);
	if (framebuffer)
	{
		glCullFace(GL_FRONT);
		framebuffer->texture->bind();
		graphics()->shader.global_color = {1, 1, 1, 1};
		graphics()->shader.outline = false;
		transform.set_scale({transform.get_scale().x, -transform.get_scale().y, transform.get_scale().z});
		graphics()->shader.model = transform.get_matrix();
		graphics()->shader.normal_model = transform.get_normal_matrix();
		graphics()->shader.projection_view = posta::App::app->camera->get_projection_view_matrix();
		posta::App::app->mesh2d->draw();
		glCullFace(GL_BACK);
	}

}

Entity::Entity()
{
	__entity_name = std::to_string(reinterpret_cast<uintptr_t>(this));
	entities.insert(this);
}

Entity::~Entity()
{
	entities.erase(this);
}

void Entity::__draw_entity(bool front)
{
	if (type)
	{
		if (front)
			type->draw_front();
		else
			type->draw_back();
	}
}

void Entity::__register(posta::entity::Type* _type)
{
	type.reset(_type);
}

#endif



