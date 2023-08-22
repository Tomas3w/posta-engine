#include <posta/Entity/Camera.h>
#include <posta/App.h>

using posta::entity::Camera;

Camera::Camera(posta::component::Camera* camera, int width, int height)
{
	this->camera.reset(camera);
	set_resolution(width, height);

	POSTA_REGISTER_ENTITY(new posta::entity::CameraType(&transform, &this->camera, &this->width, &this->height));
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
	#ifndef POSTA_EDITOR_DISABLED
	if (posta::App::app->is_editor_mode_enabled() && this == posta::App::app->camera.get())
		return posta::App::app->editor_camera->get_projection_view_matrix();
	#endif
	return camera->projection * transform.get_view_matrix();
}

glm::mat4 Camera::get_projection_matrix()
{
	#ifndef POSTA_EDITOR_DISABLED
	if (posta::App::app->is_editor_mode_enabled() && this == posta::App::app->camera.get())
		return posta::App::app->editor_camera->get_projection_matrix();
	#endif
	return camera->projection;
}

