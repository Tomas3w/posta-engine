#ifndef POSTAENGINE_SMOOTHSTATICMESH_COMPONENT_H
#define POSTAENGINE_SMOOTHSTATICMESH_COMPONENT_H
#include <posta/Component/DrawableMesh.h>

namespace posta::component {
	/// Smooth static mesh component
	/** Used to bind a mesh when drawing, the vertex data cannot
	 * be modified after creation. This differs from StaticMesh because of how vertices share properties, this makes it more lightweight and it makes it look more smooth, but it doesn't work well for some shapes, like cubes for example */
	class SmoothStaticMesh : public DrawableMesh
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
			void bind() override;

			/// Draws the mesh without binding it
			/** Faster drawing than having to bind and then draw every time,
			 * useful for drawing many times the same mesh */
			void draw_without_binding() override;
			/// Draws x instances of the same mesh in one single draw call
			void draw_many_without_binding(size_t x) override;

			/// Binds and then draws the mesh
			void draw() override;
		private:
			GLuint vbo, ebo, vao;
			size_t n_indices;
			int index_type;
	};
};

#endif // POSTAENGINE_SMOOTHSTATICMESH_COMPONENT_H
