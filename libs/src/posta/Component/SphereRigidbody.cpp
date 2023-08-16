#include <posta/Component/SphereRigidbody.h>
#include <posta/App.h>

using posta::component::SphereRigidbody;

SphereRigidbody::SphereRigidbody(glm::vec3 position, float mass, float radius, glm::vec3 velocity)
{
	btTransform t;
	t.setIdentity();
	t.setOrigin(to_btVector3(position));
	btSphereShape* sphere = new btSphereShape(radius);
	btVector3 inertia(0, 0, 0);
	sphere->calculateLocalInertia(mass, inertia);

	btMotionState* motion = new btDefaultMotionState(t);
	btRigidBody::btRigidBodyConstructionInfo info(mass, motion, sphere, inertia);
	set_body(new btRigidBody(info));
	App::app->physics->world->addRigidBody(get_body());

	get_body()->applyCentralForce(posta::to_btVector3(velocity));
}

