#include <posta/Component/Rigidbody.h>
#include <posta/App.h>

using posta::component::Rigidbody;

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

posta::component::Transform Rigidbody::get_transform()
{
	btTransform t;
	body->getMotionState()->getWorldTransform(t);
	return posta::component::Transform(t);
}

void Rigidbody::set_transform(posta::component::Transform transform)
{
	btTransform tr = posta::component::Transform::tobtTransform(transform);
	body->setWorldTransform(tr);
	body->getMotionState()->setWorldTransform(tr);
}

btRigidBody* Rigidbody::get_body()
{
	return body;
}

void Rigidbody::set_body(btRigidBody* body)
{
	this->body = body;
}

