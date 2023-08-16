#include <posta/Component/BoxRigidbody.h>
#include <posta/App.h>

using posta::component::BoxRigidbody;

BoxRigidbody::BoxRigidbody(glm::vec3 position, float mass, glm::vec3 ext, glm::vec3 velocity)
{
	btTransform t;
	t.setIdentity();
	t.setOrigin(to_btVector3(position));
	btBoxShape* box = new btBoxShape(to_btVector3(ext));
	btVector3 inertia(0, 0, 0);
	box->calculateLocalInertia(mass, inertia);

	btMotionState* motion = new btDefaultMotionState(t);
	btRigidBody::btRigidBodyConstructionInfo info(mass, motion, box, inertia);
	set_body(new btRigidBody(info));
	App::app->physics->world->addRigidBody(get_body());

	get_body()->applyCentralForce(posta::to_btVector3(velocity));
}

