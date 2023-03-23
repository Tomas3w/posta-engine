#include <engine/include/Util/Mesh.h>

using Engine::Mesh;

Mesh::Mesh(const VertexProperties* properties)
{
	vertex_properties = properties;
}

void Mesh::reserve_vertices(size_t vertices)
{
	this->vertices.reserve(vertices);
}

void Mesh::add_vertex(const float* vertex)
{
	vertices.insert(vertices.end(), vertex, vertex + vertex_properties->sum_of_sizes);
}

const std::vector<GLfloat>& Mesh::get_vertices() const
{
	return vertices;
}

void Mesh::swap_vertices(std::vector<GLfloat>& vs)
{
	vertices.swap(vs);
}

const Engine::VertexProperties* Mesh::get_vertex_properties() const
{
	return vertex_properties;
}

