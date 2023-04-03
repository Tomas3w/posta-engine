#include <engine/include/Util/Mesh.h>
#include <engine/include/Util/LoggingMacro.h>

using Engine::Mesh;

Mesh::Mesh(const VertexProperties* properties)
{
	vertex_properties = properties;
	is_locked = false;
	data.assign(vertex_properties->attributes_sizes.size(), {});
	faces_data.assign(vertex_properties->attributes_sizes.size(), {});
}

const std::vector<GLfloat>& Mesh::get_vertex_data()
{
	check_if_locked();
	if (vertex_data.empty())
	{
		size_t n_indices = faces_data.at(0).size();
		vertex_data.resize(vertex_properties->sum_of_sizes * n_indices);
		GLfloat* ptr = vertex_data.data();
		for (size_t i = 0; i < n_indices; i++)
		{
			for (int j = 0; j < static_cast<int>(vertex_properties->attributes_sizes.size()); j++)
			{
				memcpy(ptr, &data[j][faces_data[j][i] * vertex_properties->attributes_sizes[j]], vertex_properties->attributes_sizes[j] * sizeof(GLfloat));
				ptr += vertex_properties->attributes_sizes[j];
			}
		}
	}
	//LOG(faces_data.size(), " ", faces_data.at(0).size(), " ", vertex_data.size());
	return vertex_data;
}

const std::vector<GLfloat>& Mesh::get_data(int attribute_index)
{
	return data.at(attribute_index);
}

int Mesh::get_index_by_name(std::string name)
{
	int i = 0;
	for (; i < static_cast<int>(vertex_properties->attribute_name.size()); i++)
	{
		if (vertex_properties->attribute_name[i] == name)
			return i;
	}
	throw std::out_of_range("[Mesh::get_index_by_name] couldn't find vertex attribute: " + name);
}

void Mesh::add_face(std::array<std::vector<int>, 3> faces_indices)
{
	check_if_not_locked();
	for (auto& attributes_indices : faces_indices)
	{
		if (attributes_indices.size() != vertex_properties->attributes_sizes.size())
			throw std::logic_error("one of the faces_indices does not contain the exact amount of attributes indices, " + std::to_string(vertex_properties->attributes_sizes.size()) + " expected, got " + std::to_string(attributes_indices.size()));
		for (auto i = 0ul; i < attributes_indices.size(); i++)
			faces_data[i].push_back(attributes_indices[i]);
	}
}

const std::vector<int>& Mesh::get_indices(int attribute_index)
{
	return faces_data.at(attribute_index);
}

const Engine::VertexProperties* Mesh::get_vertex_properties() const
{
	return vertex_properties;
}

void Mesh::check_if_locked() const
{
	if (!is_locked)
		throw std::logic_error("mesh should be locked for this mesh operation");
}

void Mesh::check_if_not_locked() const
{
	if (is_locked)
		throw std::logic_error("mesh should not be locked for this mesh operation");
}

void Mesh::lock()
{
	is_locked = true;
}

