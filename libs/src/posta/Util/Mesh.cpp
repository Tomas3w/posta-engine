#include <cstring>
#include <posta/Util/Mesh.h>
#include <posta/Util/LoggingMacro.h>
#include <limits>

using posta::Mesh;

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

const posta::VertexProperties* Mesh::get_vertex_properties() const
{
	return vertex_properties;
}

//std::vector<GLfloat> elements_vertex_data;
//std::vector<GLushort> elements_indices_ushort;
//std::vector<GLuint> elements_indices_uint;

void Mesh::generate_elements_data(std::vector<std::string> anchor_attributes)
{
	check_if_locked();
	if (!elements_vertex_data.empty())
		throw std::logic_error("elements for this mesh have already been generated");
	
	// Obtaining anchors indices
	std::vector<int> anchors;
	anchors.reserve(anchor_attributes.size());
	for (auto& anchor : anchor_attributes)
		anchors.push_back(get_index_by_name(anchor));
	//
	std::map<std::vector<int>, std::pair<std::vector<std::vector<int>>, std::vector<GLfloat>>> vertices;
	size_t n_indices = faces_data.at(0).size();

	std::vector<int> key(anchors.size());
	for (size_t i = 0; i < n_indices; i++)
	{
		int k = 0;
		for (int j = 0; j < static_cast<int>(vertex_properties->attributes_sizes.size()); j++)
		{
			for (auto& a : anchors)
			{
				if (j == a)
				{
					key[k] = faces_data[j][i];
					k++;
				}
			}
		}
		vertices[key].first.push_back(std::vector<int>(vertex_properties->attributes_sizes.size()));
		for (int j = 0; j < static_cast<int>(vertex_properties->attributes_sizes.size()); j++)
			vertices[key].first.back()[j] = faces_data[j][i];
	}
	//
	for (auto& v : vertices)
	{
		auto& combinations = v.second.first;
		auto& vertex_data = v.second.second;
		vertex_data.resize(vertex_properties->sum_of_sizes);

		GLfloat* ptr = vertex_data.data();
		for (int j = 0; j < static_cast<int>(vertex_properties->attributes_sizes.size()); j++)
		{
			bool is_anchor = false;
			for (auto& a : anchors)
			{
				if (j == a)
				{
					is_anchor = true;
					break;
				}
			}
			if (is_anchor)
			{
				memcpy(ptr, &data[j][combinations[0][j] * vertex_properties->attributes_sizes[j]], vertex_properties->attributes_sizes[j] * sizeof(GLfloat));
			}
			else
			{
				if (vertex_properties->interpolation_type[j] == VertexProperties::INTERPOLATION_TYPE::LERP)
				{
					memset(ptr, 0, vertex_properties->attributes_sizes[j]);
					for (auto& comb : combinations)
					{
						for (size_t i = 0; i < vertex_properties->attributes_sizes[j]; i++)
							ptr[i] += data[j][comb[j] * vertex_properties->attributes_sizes[j] + i] / combinations.size();
					}
				}
				else
				{
					memset(ptr, 0, vertex_properties->attributes_sizes[j]);
					for (auto& comb : combinations)
					{
						for (size_t i = 0; i < vertex_properties->attributes_sizes[j]; i++)
							ptr[i] += data[j][comb[j] * vertex_properties->attributes_sizes[j] + i];
					}
					float len = 0;
					for (size_t i = 0; i < vertex_properties->attributes_sizes[j]; i++)
						len += ptr[i] * ptr[i];
					len = sqrt(len);
					for (size_t i = 0; i < vertex_properties->attributes_sizes[j]; i++)
						ptr[i] /= len;
				}
			}
			ptr += vertex_properties->attributes_sizes[j];
		}
		elements_vertex_data.insert(elements_vertex_data.end(), vertex_data.begin(), vertex_data.end());
	}
	//
	if (n_indices < std::numeric_limits<GLushort>::max())
	{
		for (size_t i = 0; i < n_indices; i++)
		{
			int k = 0;
			for (int j = 0; j < static_cast<int>(vertex_properties->attributes_sizes.size()); j++)
			{
				for (auto& a : anchors)
				{
					if (j == a)
					{
						key[k] = faces_data[j][i];
						k++;
					}
				}
			}
			elements_indices_ushort.push_back(std::distance(vertices.begin(), vertices.find(key)));
		}
	}
	else
	{
		for (size_t i = 0; i < n_indices; i++)
		{
			int k = 0;
			for (int j = 0; j < static_cast<int>(vertex_properties->attributes_sizes.size()); j++)
			{
				for (auto& a : anchors)
				{
					if (j == a)
					{
						key[k] = faces_data[j][i];
						k++;
					}
				}
			}
			elements_indices_uint.push_back(std::distance(vertices.begin(), vertices.find(key)));
		}
	}
}

const std::vector<GLfloat>& Mesh::get_elements_vertex_data()
{
	check_if_locked();
	if (elements_vertex_data.empty())
		throw std::logic_error("elements have not been generated in this mesh");
	return elements_vertex_data;
}

const std::vector<GLushort>& Mesh::get_elements_indices_ushort()
{
	if (!are_elements_indices_short())
		throw std::logic_error("elements indices are not short and cannot be converted to ushort");
	return elements_indices_ushort;
}

const std::vector<GLuint>& Mesh::get_elements_indices_uint()
{
	if (are_elements_indices_short())
		throw std::logic_error("elements indices are short and cannot be converted to uint");
	return elements_indices_uint;
}

bool Mesh::are_elements_indices_short()
{
	if (elements_vertex_data.empty())
		throw std::logic_error("elements have not been generated in this mesh");
	return !elements_indices_ushort.empty();
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

