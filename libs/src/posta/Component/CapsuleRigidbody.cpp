#include <posta/Component/CapsuleRigidbody.h>
#include <posta/App.h>

using Engine::Component::CapsuleRigidbody;

CapsuleRigidbody::CapsuleRigidbody(glm::vec3 position, float mass, float radius, float distance, glm::vec3 velocity)
{
	btTransform t;
	t.setIdentity();
	t.setOrigin(to_btVector3(position));
	btCapsuleShape* capsule = new btCapsuleShape(radius, distance);
	btVector3 inertia(0, 0, 0);
	capsule->calculateLocalInertia(mass, inertia);

	btMotionState* motion = new btDefaultMotionState(t);
	btRigidBody::btRigidBodyConstructionInfo info(mass, motion, capsule, inertia);
	//info.m_friction = 3.5f;
	set_body(new btRigidBody(info));
	App::app->physics->world->addRigidBody(get_body());

	get_body()->applyCentralForce(Engine::to_btVector3(velocity));
}

