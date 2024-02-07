#include <posta/Network/NetworkPackage.h>
#include <cstring>
#include <posta/Util/Mesh.h>
#include <posta/Util/LoggingMacro.h>
#include <limits>

using posta::Mesh;

bool posta::VertexProperties::operator==(const VertexProperties& other)
{
	return attribute_name == other.attribute_name
		&& interpolation_type == other.interpolation_type
		&& attributes_sizes == other.attributes_sizes
		&& sum_of_sizes == other.sum_of_sizes;
}

glm::vec3 Mesh::calculate_normal(std::array<glm::vec3, 3> points)
{
	auto dir = glm::cross(points[1] - points[0], points[2] - points[0]);
	auto norm = glm::normalize(dir);
	//LOG("a ", glm::to_string(points[0]), " b ", glm::to_string(points[1]), " c ", glm::to_string(points[2]));
	//LOG("ag ", glm::to_string(norm));
	return norm;//glm::vec3(0, 1, 0);//norm;
}

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
		return;
	//	throw std::logic_error("elements for this mesh have already been generated");
	
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
	for (auto __it = vertices.begin(), __ft = vertices.end(); __it != __ft; __it++)//auto& v : vertices)
	{
		auto& v = *__it;
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

template<> inline uint32_t posta::NetworkPackageTypeSize::operator<<(const posta::NetworkPackageTypeSize::type&, const std::vector<posta::VertexProperties::INTERPOLATION_TYPE>& value) { return sizeof(uint32_t) + value.size() * sizeof(uint32_t); }
namespace posta {
	void operator<<(posta::NetworkPackage::Writer& writer, const std::vector<posta::VertexProperties::INTERPOLATION_TYPE>& value)
	{
		uint32_t size = value.size();
		writer << size;
		for (auto& v : value)
			writer << static_cast<uint32_t>(v);
	}
	void operator>>(posta::NetworkPackage::Writer& writer, std::vector<posta::VertexProperties::INTERPOLATION_TYPE>& value)
	{
		uint32_t size;
		writer >> size;
		value.resize(size);
		
		for (size_t i = 0; i < size; i++)
		{
			uint32_t v;
			writer >> v;
			value[i] = static_cast<posta::VertexProperties::INTERPOLATION_TYPE>(v);
		}
	}
}

void Mesh::dump_to_filestream(std::fstream& stream)
{
	if (!stream.is_open())
		throw std::logic_error("mesh dump_to_filestream cannot work with a closed stream");

	auto size = posta::NetworkPackageTypeSize::size_many(
		vertex_properties->attribute_name,
		vertex_properties->interpolation_type,
		vertex_properties->attributes_sizes,
		vertex_properties->sum_of_sizes,
		vertex_data,
		elements_vertex_data,
		elements_indices_ushort,
		elements_indices_uint,
		data,
		faces_data,
		(uint8_t&)is_locked
	);
	uint8_t* dump = new uint8_t[size];

	//LOG("output size ", size);
	posta::NetworkPackage::Writer writer(dump);
	writer.write_many(
		vertex_properties->attribute_name,
		vertex_properties->interpolation_type,
		vertex_properties->attributes_sizes,
		vertex_properties->sum_of_sizes,
		vertex_data,
		elements_vertex_data,
		elements_indices_ushort,
		elements_indices_uint,
		data,
		faces_data,
		(uint8_t&)is_locked
	);

	stream.write(reinterpret_cast<char*>(dump), size);
}

void Mesh::read_dumped_filestream(std::fstream& stream)
{
	if (!stream || !stream.is_open())
		throw std::logic_error("mesh read_dumped_filestream cannot work with a closed (or failed) stream");

	stream.seekg(0, std::ios::end);
	uint32_t size = stream.tellg();
	stream.seekg(0, std::ios::beg);

	//LOG("input size ", size);
	uint8_t* dump = new uint8_t[size];
	stream.read(reinterpret_cast<char*>(dump), size);

	posta::NetworkPackage::Writer writer(dump);
	VertexProperties* aux_vertex_properties = new VertexProperties;
	//aux_vertex_properties->attribute_name = std::vector<std::string>();
	writer.read_many(
		aux_vertex_properties->attribute_name,
		aux_vertex_properties->interpolation_type,
		aux_vertex_properties->attributes_sizes,
		aux_vertex_properties->sum_of_sizes,
		vertex_data,
		elements_vertex_data,
		elements_indices_ushort,
		elements_indices_uint,
		data,
		faces_data,
		(uint8_t&)is_locked
	);
	vertex_properties = aux_vertex_properties;

	VertexProperties* found = nullptr;
	for (auto& v : __dumped_vertex_properties)
	{
		if ((*aux_vertex_properties) == (*v))
		{
			found = v.get();
			break;
		}
	}
	if (found)
	{
		vertex_properties = found;
		delete aux_vertex_properties;
	}
	else
		__dumped_vertex_properties.emplace_back(aux_vertex_properties);
}

void Mesh::lock()
{
	is_locked = true;
}

