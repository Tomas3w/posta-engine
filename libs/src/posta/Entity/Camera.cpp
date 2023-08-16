#include <posta/Entity/Camera.h>

using posta::Entity::Camera;

Camera::Camera(posta::component::Camera* camera, int width, int height)
{
	this->camera.reset(camera);
	set_resolution(width, height);
}

void Camera::set_resolution(const int width, const int height)
{
	this->width = width;
	this->height = height;
	update_projection_matrix();
}

void Camera::get_resolution(int& width, int& height)
{
	width = this->width;
	height = this->height;
}

void Camera::update_projection_matrix()
{
	camera->update_projection_matrix(width, height);
}

glm::mat4 Camera::get_projection_view_matrix()
{
	return camera->projection * transform.get_view_matrix();
}

glm::mat4 Camera::get_projection_matrix()
{
	return camera->projection;
}

