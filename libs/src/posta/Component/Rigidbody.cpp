#include <posta/Component/Rigidbody.h>
#include <posta/App.h>
#include <posta/Util/LoggingMacro.h>

using posta::component::Rigidbody;

Rigidbody::~Rigidbody()
{
	delete body->getMotionState();
	delete body->getCollisionShape();
	App::app->physics->world->removeRigidBody(body);
	delete body;

	for (auto c : constraints)
		c->remove_rigidbodies();
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

void Rigidbody::add_constraint(Constraint* constraint)
{
	constraints.push_back(constraint);
}

void Rigidbody::remove_constraint(Constraint* constraint)
{
	for (auto it = constraints.begin(); it != constraints.end(); it++)
	{
		if ((*it) == constraint)
		{
			constraints.erase(it);
			break;
		}
	}
}

