#include <posta/Component/PlaneRigidbody.h>
#include <posta/App.h>

using posta::component::PlaneRigidbody;

PlaneRigidbody::PlaneRigidbody(glm::vec3 position, glm::vec3 normal)
{
	btTransform t;
	t.setIdentity();
	t.setOrigin(posta::to_btVector3(position));

	btStaticPlaneShape* plane = new btStaticPlaneShape(posta::to_btVector3(normal), 0);
	btMotionState* motion = new btDefaultMotionState(t);
	btRigidBody::btRigidBodyConstructionInfo info(0.0, motion, plane);
	set_body(new btRigidBody(info));
	App::app->physics->world->addRigidBody(get_body());
}

