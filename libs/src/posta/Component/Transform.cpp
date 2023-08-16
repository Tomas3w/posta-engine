#include <posta/Component/Transform.h>

using posta::component::Transform;

btTransform Transform::tobtTransform(const Transform& transform)
{
	btTransform tr;
	tr.setOrigin(btVector3(transform.get_position().x, transform.get_position().y, transform.get_position().z));
	tr.setRotation(btQuaternion(transform.get_rotation().x, transform.get_rotation().y, transform.get_rotation().z, transform.get_rotation().w));
	return tr;
}

Transform::Transform(btTransform t)
{
	// TOTEST, rotation may be broken
	btVector3 pos = t.getOrigin();
	position = glm::vec3(pos.getX(), pos.getY(), pos.getZ());
	scale = glm::vec3(1.0f);
	rotation = glm::quat(t.getRotation().getW(), t.getRotation().getX(), t.getRotation().getY(), t.getRotation().getZ());
}

Transform::Transform()
{
	position = glm::vec3(0.0f);
	scale = glm::vec3(1.0f);
	rotation = glm::quat_cast(glm::mat4(1.0f));
}

Transform::Transform(glm::vec3 pos)
{
	position = pos;
	scale = glm::vec3(1.0f);
	rotation = glm::quat_cast(glm::mat4(1.0f));
}

void Transform::set_position(glm::vec3 pos)
{
	position = pos;
}

void Transform::set_rotation(glm::quat rot)
{
	rotation = rot;
}

void Transform::set_rotation(glm::vec3 rot)
{
	rotation = glm::quat_cast(glm::mat4(1.0f));
	rotation = glm::rotate(rotation, rot.x, {1, 0, 0});
	rotation = glm::rotate(rotation, rot.y, {0, 1, 0});
	rotation = glm::rotate(rotation, rot.z, {0, 0, 1});
}

void Transform::set_rotation(glm::vec4 rot)
{
	rotation = glm::rotate(glm::quat_cast(glm::mat4(1.0f)), rot.w, {rot.x, rot.y, rot.z});
}

void Transform::set_scale(glm::vec3 sca)
{
	scale = sca;
}

const glm::vec3& Transform::get_position() const
{
	return position;
}

const glm::quat& Transform::get_rotation() const
{
	return rotation;
}

const glm::vec3& Transform::get_scale() const
{
	return scale;
}

glm::mat4 Transform::get_matrix() const
{
	return glm::translate(glm::mat4(1.0f), position) * glm::toMat4(rotation) * glm::scale(glm::mat4(1.0f), scale);
}

glm::mat3 Transform::get_normal_matrix() const
{
	auto without_scale = glm::translate(glm::mat4(1.0f), position) * glm::toMat4(rotation);
	return glm::mat3(glm::transpose(glm::inverse(without_scale)));
}

glm::mat4 Transform::get_view_matrix() const
{
	return glm::toMat4(glm::inverse(rotation)) * glm::translate(glm::mat4(1.0f), -position);
}

glm::vec3 Transform::front() const
{
	return rotation * glm::vec3(0, 0, 1);
}

glm::vec3 Transform::right() const
{
	return rotation * glm::vec3(1, 0, 0);
}

glm::vec3 Transform::up() const
{
	return rotation * glm::vec3(0, 1, 0);
}

glm::vec3 Transform::to_local(glm::vec3 vec) const
{
	return vec.x * right() +
	       vec.y * up() +
		   vec.z * front();
}

glm::vec3 Transform::from_local(glm::vec3 vec) const
{
	return glm::inverse(rotation) * vec;
}

Transform Transform::as_local_to(Transform transform) const
{
	// TOTEST
	Transform r;
	r.set_position(transform.to_local(position) + transform.position);
	r.set_rotation(transform.rotation * rotation);
	r.set_scale(scale * transform.scale);
	return r;
}

Transform Transform::as_origin_to(Transform transform) const
{
	// TOTEST, already tested against as_local_to
	/*
		posta::component::Transform a({0, 0, 0});
		a.set_rotation(glm::rotate(glm::quat(1, 0, 0, 0), static_cast<float>(M_PI / 2), glm::vec3(0, 0, 1)));
		posta::component::Transform b({1, 0, 0});
		posta::component::Transform c = b.as_local_to(a);
		posta::component::Transform d = a.as_origin_to(c);
		// d should be equal to b
	*/
	Transform r;
	r.set_scale(transform.scale / scale);
	r.set_rotation(-(glm::inverse(transform.rotation) * rotation));
	r.rotation.w = -r.rotation.w;
	r.set_position(glm::inverse(rotation) * (transform.position - position));
	return r;
}

Transform Transform::interpolate(Transform transform, float factor)
{
	factor = glm::clamp(factor, 0.0f, 1.0f);
	Transform tr;
	tr.set_position(position * (1 - factor) + transform.position * factor);
	tr.set_rotation(glm::slerp(rotation, transform.rotation, factor));
	tr.set_scale(scale * (1 - factor) + transform.scale * factor);
	return tr;
}

