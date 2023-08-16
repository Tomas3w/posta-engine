#ifndef POSTAENGINE_MESHSTATICRIGIDBODY_H
#define POSTAENGINE_MESHSTATICRIGIDBODY_H
#include <posta/Component/Rigidbody.h>
#include <posta/Util/Mesh.h>
#include <memory>

namespace posta::component {
	/// Class used to make a Static Rigidbody from a Mesh object
	class MeshStaticRigidbody : public Rigidbody
	{
		public:
			/// Constructs a MeshStaticRigidbody from a mesh (that has indices)
			MeshStaticRigidbody(glm::vec3 position, Mesh& mesh);
			virtual ~MeshStaticRigidbody();
		private:
			std::vector<GLfloat> vertices;
			std::vector<int> indices;
			
			//btBvhTriangleMeshShape* tmesh;
			btTriangleIndexVertexArray* tarray;
	};
}

#endif // POSTAENGINE_MESHSTATICRIGIDBODY_H
