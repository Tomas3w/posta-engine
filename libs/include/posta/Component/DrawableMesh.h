#ifndef POSTAENGINE_DRAWABLEMESH_COMPONENT_H
#define POSTAENGINE_DRAWABLEMESH_COMPONENT_H
#include <posta/Util/Mesh.h>

namespace posta::component {
	/// Abstract class for mesh types that can be drawn
	class DrawableMesh
	{
		public:
			virtual ~DrawableMesh() = default;
			/// Binds the mesh, following draw calls will then use this mesh
			virtual void bind() = 0;

			/// Draws the mesh without binding it
			/** Faster drawing than having to bind and then draw every time,
			 * useful for drawing many times the same mesh */
			virtual void draw_without_binding() = 0;
			/// Draws x instances of the same mesh in one single draw call
			virtual void draw_many_without_binding(size_t x) = 0;

			/// Binds and then draws the mesh
			virtual void draw() = 0;
	};
}

#endif
