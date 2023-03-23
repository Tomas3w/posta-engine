#include "engine/include/Component/MeshStaticRigidbody.h"

using Engine::Component::MeshStaticRigidbody;

MeshStaticRigidbody::MeshStaticRigidbody(glm::vec3 position, std::vector<GLfloat>& _vertices, std::vector<int>& _indices, bool swap_data)
{
	// get vertex data and indices
	if (swap_data)
	{
		vertices.swap(_vertices);
		indices.swap(_indices);
	}
	else
	{
		vertices = _vertices;
		indices = _indices;
	}

	// make the rigidbody and shape
	btTransform t;
	t.setIdentity();
	t.setOrigin(to_btVector3(position));
	
	tarray = new btTriangleIndexVertexArray(
		indices.size() / 3, // num of triangles
		indices.data(), sizeof(int) * 3, // indices and bytes per triangle
		vertices.size() / 3, // num of vertices
		vertices.data(), 3 * sizeof(GLfloat)); // vertices and bytes per vertex
	btBvhTriangleMeshShape* tmesh = new btBvhTriangleMeshShape(tarray, true);

	btMotionState* motion = new btDefaultMotionState(t);
	btRigidBody::btRigidBodyConstructionInfo info(0, motion, tmesh);
	set_body(new btRigidBody(info));
	App::app->physics->world->addRigidBody(get_body());

}

MeshStaticRigidbody::~MeshStaticRigidbody()
{
	delete tarray;
}

