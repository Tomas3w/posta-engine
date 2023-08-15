#include "engine/include/Component/Rigidbody.h"
#include <engine/include/App.h>

using Engine::Component::Rigidbody;

Rigidbody::~Rigidbody()
{
	delete body->getMotionState();
	delete body->getCollisionShape();
	App::app->physics->world->removeRigidBody(body);
	delete body;
}

glm::mat4 Rigidbody::get_matrix()
{
	glm::mat4 matrix;
	btTransform t;
	body->getMotionState()->getWorldTransform(t);
	t.getOpenGLMatrix(&matrix[0][0]);
	return matrix;
}

Engine::Component::Transform Rigidbody::get_transform()
{
	btTransform t;
	body->getMotionState()->getWorldTransform(t);
	return Engine::Component::Transform(t);
}

void Rigidbody::set_transform(Engine::Component::Transform transform)
{
	btTransform tr = Engine::Component::Transform::tobtTransform(transform);
	body->setCenterOfMassTransform(tr);
}

btRigidBody* Rigidbody::get_body()
{
	return body;
}

void Rigidbody::set_body(btRigidBody* body)
{
	this->body = body;
}

