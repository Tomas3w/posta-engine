#include <posta/Entity/Camera.h>
#include <posta/App.h>
#include <posta/Util/LoggingMacro.h>

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

glm::vec3 Camera::point_on_screen(glm::vec3 position)
{
	camera->update_projection_matrix(width, height);
	glm::vec4 rposition = {position.x, position.y, position.z, 1.0f};
	glm::vec4 fpoint = get_projection_matrix() * transform.get_view_matrix() * rposition;
	fpoint /= fpoint.w;
	glm::vec2 point(fpoint.x, fpoint.y);
	point /= 2.0f;
	point += glm::vec2(0.5f);
	point.x *= width;
	point.y *= height;
	return glm::vec3(point, fpoint.z);
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

