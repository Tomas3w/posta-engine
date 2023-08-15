#include <posta/Component/Camera.h>
#include <cmath>

using Engine::Component::PCamera;
using Engine::Component::OCamera;

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

OCamera::OCamera(float _near, float _far)
{
	this->__near = _near;
	this->__far = _far;
	this->__size = 2;
}

OCamera::OCamera(float _near, float _far, float _size)
{
	this->__near = _near;
	this->__far = _far;
	this->__size = _size;
}

void OCamera::update_projection_matrix(const int width, const int height)
{
	float ratio = static_cast<float>(width) / height;
	float h = __size;
	float w = ratio * h;
	projection = glm::ortho(-w, w, -h, h, __near, __far);
}

