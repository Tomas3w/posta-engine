#include <posta/Component/MeshStaticRigidbody.h>
#include <posta/Util/LoggingMacro.h>
#include <posta/App.h>

using posta::component::MeshStaticRigidbody;

MeshStaticRigidbody::MeshStaticRigidbody(glm::vec3 position, Mesh& mesh)
{
	// get vertex data and indices
	vertices = mesh.get_data(mesh.get_index_by_name("position"));
	indices = mesh.get_indices(mesh.get_index_by_name("position"));

	// make the rigidbody and shape
	btTransform t;
	t.setIdentity();
	t.setOrigin(to_btVector3(position));

	if (indices.size() == 0 || vertices.size() == 0)
		throw std::logic_error("couldn't create MeshStaticRigidbody with a mesh with 0 indices or vertices");
	
	tarray = new btTriangleIndexVertexArray(
		indices.size() / 3, // num of triangles
		indices.data(), sizeof(int) * 3, // indices and bytes per triangle
		vertices.size() / 3, // num of vertices
		vertices.data(), 3 * sizeof(GLfloat)); // vertices and bytes per vertex
	auto tmesh = new btBvhTriangleMeshShape(tarray, true);

	btMotionState* motion = new btDefaultMotionState(t);
	btRigidBody::btRigidBodyConstructionInfo info(0, motion, tmesh);
	set_body(new btRigidBody(info));
	App::app->physics->world->addRigidBody(get_body());
}

MeshStaticRigidbody::~MeshStaticRigidbody()
{
	/*LOG("deleting tmesh");
	delete tmesh;*/
	//LOG("deleting tarray");
	delete tarray;
	//LOG("all deleted (not really)!");
}

