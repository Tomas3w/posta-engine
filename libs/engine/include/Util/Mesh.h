#ifndef POSTAENGINE_MESH_H
#define POSTAENGINE_MESH_H
#include <iostream>
#include <vector>
#include <GL/glew.h>

namespace Engine {
	/// Structure containing the vertex properties for meshes
	struct VertexProperties
	{
		const std::vector<GLubyte> attributes_sizes; // sizes of each component of the vertex (not in bytes, but in floats)
		const GLubyte sum_of_sizes; // sum of the sizes of attributes_sizes (not in bytes, but in floats)
	};

	/// Utility class representing a mesh
	/** A mesh contains the vertices of a 3d model. 
	 * A mesh instance can represent any mesh */
	class Mesh
	{
		public:
			/// Constructs an empty mesh using properties as the format
			/** \param properties a VertexProperties pointer, the 
			 * reference must be valid as long as the Mesh is being used */
			Mesh(const VertexProperties* properties);
			/// Reserves memory for saving vertices
			void reserve_vertices(size_t vertices);
			/// Adds a vertix to the internal array of vertices
			void add_vertex(const float* vertex);

			/// Returns the vertices as a vector of floats
			const std::vector<GLfloat>& get_vertices() const;
			/// Swaps the vertices data with another vertices data
			/** This effectively moves the data from vs to the internal
			 * vector of vertices and visceversa */
			void swap_vertices(std::vector<GLfloat>& vs);
			/// Returns the vertices attributes properties
			const VertexProperties* get_vertex_properties() const;
		private:
			const VertexProperties* vertex_properties;
			std::vector<GLfloat> vertices;
	};
}

#endif // POSTAENGINE_MESH_H
