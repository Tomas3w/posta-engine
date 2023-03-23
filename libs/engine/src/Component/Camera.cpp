#include <engine/include/Component/Camera.h>

using Engine::Component::PCamera;

PCamera::PCamera(float _near, float _far)
{
	this->__near = _near;
	this->__far = _far;
	this->__fov = M_PI / 2.0f;
}

PCamera::PCamera(float _near, float _far, float _fov)
{
	this->__near = _near;
	this->__far = _far;
	this->__fov = _fov;
}

void PCamera::update_projection_matrix(const int width, const int height)
{
	float ratio = static_cast<float>(width) / height;
	projection = glm::perspective(__fov, ratio, __near, __far);
}

