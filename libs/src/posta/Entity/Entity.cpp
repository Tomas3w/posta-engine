#include <posta/Util/General.h>
#include <posta/Entity/Entity.h>
#include <posta/App.h>
#include <posta/Util/LoggingMacro.h>

using posta::entity::Entity;
using posta::entity::EmptyType;
using posta::entity::CameraType;
using posta::entity::RigidbodyType;
//using posta::entity::MeshType;

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

posta::component::Transform EmptyType::get_transform()
{
	return *transform;
}

void EmptyType::set_transform(posta::component::Transform transform)
{
	*this->transform = transform;
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
	if (this == posta::App::app->editor.currently_selected_type_entity)
		Entity::draw_transform(*transform);

	if (*camera)
		Entity::draw_camera_outline(*transform, camera->get(), framebuffer, *width, *height);
}

posta::component::Transform CameraType::get_transform()
{
	return *transform;
}

void CameraType::set_transform(posta::component::Transform transform)
{
	*this->transform = transform;
}

posta::component::Rigidbody& RigidbodyType::get_rigidbody()
{
	return *rigidbody;
}

void RigidbodyType::draw_meshes()
{
	for (component::DrawableMesh* drawable_mesh : drawable_meshes)
		drawable_mesh->draw();
}

RigidbodyType::RigidbodyType(posta::component::Rigidbody* _rigidbody, posta::component::DrawableMesh* _drawable_mesh)
{
	rigidbody = _rigidbody;
	if (_drawable_mesh != nullptr)
		drawable_meshes.push_back(_drawable_mesh);
}

void RigidbodyType::draw_back()
{
	auto transform = rigidbody->get_transform();
	if (this == posta::App::app->editor.currently_selected_type_entity)
	{
		for (component::DrawableMesh* drawable_mesh : drawable_meshes)
			Entity::draw_outline(drawable_mesh, transform);
	}
}

void RigidbodyType::draw_front()
{
	if (this == posta::App::app->editor.currently_selected_type_entity)
		Entity::draw_transform(rigidbody->get_transform());
}

posta::component::Transform RigidbodyType::get_transform()
{
	return rigidbody->get_transform();
}

void RigidbodyType::set_transform(posta::component::Transform transform)
{
	rigidbody->set_transform(transform);
	
	std::vector<int> states;
	int numObjects = App::app->physics->world->getNumCollisionObjects();
    for (int i = 0; i < numObjects; ++i) {
        btCollisionObject* collisionObject = App::app->physics->world->getCollisionObjectArray()[i];
        btRigidBody* rigidBody = btRigidBody::upcast(collisionObject);

        if (rigidBody)
		{
			states.push_back(rigidBody->getActivationState());
			rigidBody->setActivationState(DISABLE_DEACTIVATION);
		}
    }
	App::app->step_physics();
    for (int i = 0; i < numObjects; ++i) {
        btCollisionObject* collisionObject = App::app->physics->world->getCollisionObjectArray()[i];
        btRigidBody* rigidBody = btRigidBody::upcast(collisionObject);

        if (rigidBody)
			rigidBody->setActivationState(states[i]);
    }
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

posta::component::Transform Entity::get_transform()
{
	if (type)
		return type->get_transform();
	return posta::component::Transform();
}

void Entity::set_transform(posta::component::Transform transform)
{
	if (type)
		type->set_transform(transform);
}

posta::entity::Type* Entity::get_internal_entity_type()
{
	return type.get();
}

void Entity::draw_moving_object(TypeWithDrawableMeshes* dm_type, posta::component::Transform transform)
{
	graphics()->blank_texture.bind();
	graphics()->shader.bind();
	graphics()->shader.width = posta::App::app->get_width();
	graphics()->shader.height = posta::App::app->get_height();
	graphics()->shader.moving_object = true;
	graphics()->shader.line = false;
	graphics()->shader.time_passed += App::app->delta_time;
	graphics()->shader.time = graphics()->shader.time_passed;
	graphics()->shader.global_color = {1, 0, 1, 1};
	graphics()->shader.model = transform.get_matrix();
	graphics()->shader.normal_model = transform.get_normal_matrix();
	graphics()->shader.projection_view = posta::App::app->camera->get_projection_view_matrix();
	dm_type->draw_meshes();
	graphics()->shader.moving_object = false;
	draw_transform(transform);
}

void Entity::draw_outline(component::DrawableMesh* drawable_mesh, posta::component::Transform transform)
{
	transform.set_scale(transform.get_scale());
	graphics()->blank_texture.bind();
	graphics()->shader.bind();
	graphics()->shader.width = posta::App::app->get_width();
	graphics()->shader.height = posta::App::app->get_height();
	graphics()->shader.line = false;
	graphics()->shader.global_color = {1, 0.75, 0, 1};
	graphics()->shader.model = transform.get_matrix();
	graphics()->shader.normal_model = transform.get_normal_matrix();
	graphics()->shader.projection_view = posta::App::app->camera->get_projection_view_matrix();
	drawable_mesh->draw();
}

void Entity::draw_whole_outline(posta::component::Texture* texture)
{
	texture->bind();
	graphics()->shader.bind();
	graphics()->shader.width = posta::App::app->get_width();
	graphics()->shader.height = posta::App::app->get_height();
	graphics()->shader.global_color = {1, 1, 1, 1};
	graphics()->shader.outline = true;
	graphics()->shader.line = false;
	graphics()->shader.model = glm::mat4(1.0f);
	graphics()->shader.normal_model = glm::mat4(1.0f);
	graphics()->shader.projection_view = glm::mat4(1.0f);
	posta::App::app->mesh2d->draw();
	graphics()->shader.outline = false;
}

void Entity::draw_line(glm::vec3 i, glm::vec3 f, glm::vec4 color, float line_width)
{
	glm::vec3 i_real = posta::App::app->editor.camera->point_on_screen(i);
	if (i_real.z < -1 || i_real.z > 1)
		return;
	glm::vec3 f_real = posta::App::app->editor.camera->point_on_screen(f);
	if (f_real.z < -1 || f_real.z > 1)
		return;
	graphics()->blank_texture.bind();
	graphics()->shader.bind();
	graphics()->shader.width = posta::App::app->get_width();
	graphics()->shader.height = posta::App::app->get_height();
	graphics()->shader.global_color = color;
	graphics()->shader.line_width = line_width;
	graphics()->shader.line = true;
	graphics()->shader.linei = {i_real.x, i_real.y};
	graphics()->shader.linef = {f_real.x, f_real.y};
	graphics()->shader.model = glm::mat4(1.0f);
	graphics()->shader.normal_model = glm::mat4(1.0f);
	graphics()->shader.projection_view = glm::mat4(1.0f);
	posta::App::app->mesh2d->draw();
}

void Entity::draw_transform(posta::component::Transform transform)
{
	if (dynamic_cast<posta::component::OCamera*>(posta::App::app->editor.camera->camera.get()))
		transform.set_scale({1, 1, 1});
	else
		transform.set_scale(glm::vec3(glm::length(transform.get_position() - posta::App::app->editor.camera->transform.get_position()) / 10.0f));
	posta::App::app->clear_depth();
	const glm::vec3 scale = transform.get_scale();
	transform.set_scale({scale.x / 3, scale.y / 3, scale.z});
	graphics()->blank_texture.bind();
	graphics()->shader.bind();
	graphics()->shader.width = posta::App::app->get_width();
	graphics()->shader.height = posta::App::app->get_height();
	graphics()->shader.line = false;
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
	// Drawing near plane
	std::vector<glm::vec3> all_points;
	float length = glm::length(glm::vec2(width, height));
	glm::vec3 first_square_origin = {width / length / 4.0f, height / length / 4.0f, -0.01f};
	glm::vec3 second_square_origin = {width / length / 2.0f, height / length / 2.0f, -0.56f};
	for (glm::vec3 first_point : std::vector<glm::vec3>{first_square_origin, second_square_origin})
	{
		std::vector<glm::vec3> points;
		points.push_back(first_point);
		points.push_back({-first_point.x, first_point.y, first_point.z});
		points.push_back({-first_point.x, -first_point.y, first_point.z});
		points.push_back({first_point.x, -first_point.y, first_point.z});
		for (glm::vec3& point : points)
			point = transform.to_local(point) + transform.get_position();
		all_points.insert(all_points.end(), points.begin(), points.end());
		for (size_t i = 1; i < points.size() + 1; i++)
			draw_line(points[i % points.size()], points[(i - 1) % points.size()], {1, 1, 1, 1}, 1);
	}
	for (size_t i = 0; i < all_points.size() / 2; i++)
		draw_line(all_points[i], all_points[i + 4], {1, 1, 1, 1}, 1);
	draw_line(transform.to_local(second_square_origin + glm::vec3(0, 0.1f, 0)) + transform.get_position(), transform.to_local(glm::vec3{-second_square_origin.x, second_square_origin.y, second_square_origin.z} + glm::vec3(0, 0.1f, 0)) + transform.get_position(), {1, 1, 1, 1}, 1);
	
	// Drawing framebuffer
	if (framebuffer)
	{
		float length = glm::length(glm::vec2(width, height));
		transform.set_position(transform.get_position() - transform.front() * 0.56f);
		transform.set_scale({(width / length) / 2.0f, (height / length) / 2.0f, 1});

		glCullFace(GL_FRONT);
		framebuffer->texture->bind();
		graphics()->shader.global_color = {1, 1, 1, 1};
		graphics()->shader.line = false;
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



