#ifndef POSTAENGINE_MESHSTATICRIGIDBODY_H
#define POSTAENGINE_MESHSTATICRIGIDBODY_H
#include "engine/include/Component/Rigidbody.h"
#include "engine/include/Util/Mesh.h"
#include <memory>

namespace Engine::Component {
	/// Class used to make a Static Rigidbody from a Mesh object
	class MeshStaticRigidbody : public Rigidbody
	{
		public:
			/// Constructs a MeshStaticRigidbody
			/** Each vertex is form of three floats representing x, y, and z
			 * The indices are indices of the vertices
			 * \param position of the origin of the rigidbody
			 * \param swap_data whether to swap the vertices and indices or
			 * copy them */
			MeshStaticRigidbody(glm::vec3 position, std::vector<GLfloat>& _vertices, std::vector<int>& _indices, bool swap_data);
			virtual ~MeshStaticRigidbody();
		private:
			std::vector<GLfloat> vertices;
			std::vector<int> indices;
			
			btTriangleIndexVertexArray* tarray;
	};
}

#endif // POSTAENGINE_MESHSTATICRIGIDBODY_H
