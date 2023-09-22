#include <posta/Component/CylinderRigidbody.h>
#include <posta/App.h>

using posta::component::CylinderRigidbody;

CylinderRigidbody::CylinderRigidbody(glm::vec3 position, float mass, glm::vec3 ext, AxisAligned axis, glm::vec3 velocity)
{
	btTransform t;
	t.setIdentity();
	t.setOrigin(to_btVector3(position));
	btCylinderShape* cylinder = nullptr;
	switch (axis)
	{
		case AxisAligned::X:
			cylinder = new btCylinderShapeX(to_btVector3(ext));
			break;
		case AxisAligned::Y:
			cylinder = new btCylinderShape(to_btVector3(ext));
			break;
		case AxisAligned::Z:
			cylinder = new btCylinderShapeZ(to_btVector3(ext));
			break;
	}
	btVector3 inertia(0, 0, 0);
	cylinder->calculateLocalInertia(mass, inertia);

	btMotionState* motion = new btDefaultMotionState(t);
	btRigidBody::btRigidBodyConstructionInfo info(mass, motion, cylinder, inertia);
	set_body(new btRigidBody(info));
	App::app->physics->world->addRigidBody(get_body());

	get_body()->setLinearVelocity(posta::to_btVector3(velocity));
}


