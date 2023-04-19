#ifndef POSTAENGINE_SMOOTHSTATICMESH_COMPONENT_H
#define POSTAENGINE_SMOOTHSTATICMESH_COMPONENT_H
#include <engine/include/Util/Mesh.h>

namespace Engine::Component {
	class SmoothStaticMesh
	{
		public:
			/** Constructs a SmoothStaticMesh from a mesh
			 * \param mesh a (locked) Mesh object
			 * */
			SmoothStaticMesh(Mesh mesh);
			SmoothStaticMesh(const SmoothStaticMesh&)=delete;
			SmoothStaticMesh operator=(const SmoothStaticMesh&)=delete;
			~SmoothStaticMesh();

			/// Binds the mesh, following draw calls will then use this mesh
			void bind();

			/// Draws the mesh without binding it
			/** Faster drawing than having to bind and then draw every time,
			 * useful for drawing many times the same mesh */
			void draw_without_binding();
			/// Draws x instances of the same mesh in one single draw call
			void draw_many_without_binding(size_t x);

			/// Binds and then draws the mesh
			void draw();
		private:
			GLuint vbo, ebo, vao;
			size_t n_indices;
			int index_type;
	};
};

#endif // POSTAENGINE_SMOOTHSTATICMESH_COMPONENT_H
