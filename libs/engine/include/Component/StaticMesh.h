#ifndef POSTAENGINE_STATICMESH_COMPONENT_H
#define POSTAENGINE_STATICMESH_COMPONENT_H
#include <engine/include/Util/Mesh.h>

namespace Engine::Component {
	/// Static mesh component
	/** Used to bind a mesh when drawing, the vertex data cannot
	 * be modified after creation */
	class StaticMesh
	{
		public:
			/** Constructs a StaticMesh from a mesh
			 * \param mesh a (locked) Mesh object
			 * */
			StaticMesh(Mesh mesh);
			StaticMesh(const StaticMesh&)=delete;
			StaticMesh operator=(const StaticMesh&)=delete;
			~StaticMesh();

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
			GLuint vbo, vao;
			size_t n_vertices;
	};
}

#endif // POSTAENGINE_STATICMESH_COMPONENT_H
