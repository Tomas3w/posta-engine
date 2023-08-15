#ifndef POSTAENGINE_MESH_H
#define POSTAENGINE_MESH_H
#include <glm/detail/qualifier.hpp>
#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <posta/Util/General.h>
#include <cstring>
#include <map>

namespace Engine {
	/// Structure containing the vertex properties for meshes
	struct VertexProperties
	{
		enum class INTERPOLATION_TYPE
		{
			SLERP,
			LERP,
		};
		const std::vector<std::string> attribute_name;
		const std::vector<INTERPOLATION_TYPE> interpolation_type; // tells how should attributes be interpolated
		const std::vector<GLubyte> attributes_sizes; // sizes of each component of the vertex (not in bytes, but in floats)
		const GLubyte sum_of_sizes; // sum of the sizes of attributes_sizes (not in bytes, but in floats)
	};

	/// Utility class representing a mesh
	/** A mesh contains the vertices (and sometimes the indices) of a 3d model. 
	 * A mesh instance can represent any mesh */
	class Mesh
	{
		public:
			/// Constructs an empty mesh using properties as the format
			/** \param properties a VertexProperties pointer, the 
			 * reference must be valid as long as the Mesh is being used */
			Mesh(const VertexProperties* properties);

			/// Adds data to one of the list of attributes
			template<int C>
			void add_data(int component_index, glm::vec<C, float, glm::defaultp> data)
			{
				check_if_not_locked();
				this->data.at(component_index).insert(this->data.at(component_index).end(), &data[0], &data[0] + C);
			}
			/// Returns the data needed to make a StaticMesh
			const std::vector<GLfloat>& get_vertex_data();
			/// Returns data from one attribute
			const std::vector<GLfloat>& get_data(int attribute_index);
			/// Returns the attribute_index for get_data by name
			int get_index_by_name(std::string name);

			/// Adds a face to the mesh
			void add_face(std::array<std::vector<int>, 3> faces_indices);
			/// Returns the indices of the vertices for an attribute
			const std::vector<int>& get_indices(int attribute_index);

			/// Same as get_indices but without const qualifier
			//std::vector<int>& get_non_const_indices();
			/// Returns the vertices attributes properties
			const VertexProperties* get_vertex_properties() const;

			/// Generates data needed to draw elements instead of drawing arrays
			void generate_elements_data(std::vector<std::string> anchor_attribute);
			/// Returns the vertex data of the elements
			const std::vector<GLfloat>& get_elements_vertex_data();
			/// Returns the indices for the vertex data as a vector of GLushort
			/// This function may throw an exception if the indices cannot be returned as a vector of GLushorts
			const std::vector<GLushort>& get_elements_indices_ushort();
			/// Returns the indices for the vertex data as a vector of GLuint
			/// This function may throw an exception if the indices can be returned as a vector of GLushorts
			const std::vector<GLuint>& get_elements_indices_uint();
			/// Returns true if the indices can fit in a vector of GLushort's.
			bool are_elements_indices_short();

			/// Locks the mesh from further changes
			/// A mesh that is locked cannot be modified in the future, certain methods (like get_vertices_without_indices) can still alter
			/// the object to some degree, but these changes will no be noticeable by the callers
			void lock();
		private:
			void check_if_locked() const;
			void check_if_not_locked() const;

			const VertexProperties* vertex_properties;
			std::vector<GLfloat> vertex_data;

			std::vector<GLfloat> elements_vertex_data;
			std::vector<GLushort> elements_indices_ushort;
			std::vector<GLuint> elements_indices_uint;

			std::vector<std::vector<GLfloat>> data; // one vector<GLfloat> for each attribute
			std::vector<std::vector<int>> faces_data; // one vector<int> for each attribute

			bool is_locked;
	};
}

#endif // POSTAENGINE_MESH_H
