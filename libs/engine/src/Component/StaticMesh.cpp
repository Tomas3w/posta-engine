#include <engine/include/Component/StaticMesh.h>
#include <engine/include/Util/LoggingMacro.h>

using Engine::Component::StaticMesh;

StaticMesh::StaticMesh(Mesh mesh)
{
	//LOG("here we are");
	const VertexProperties* props = mesh.get_vertex_properties();
	n_vertices = mesh.get_vertex_data().size() / props->sum_of_sizes;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, n_vertices * props->sum_of_sizes * sizeof(float), mesh.get_vertex_data().data(), GL_STATIC_DRAW);

	GLuint attribute = 0;
	GLubyte sum = 0;
	for (const GLubyte& size : props->attributes_sizes)
	{
		glVertexAttribPointer(attribute, size, GL_FLOAT, GL_FALSE, props->sum_of_sizes * sizeof(float), (void*)(sum * sizeof(float)));
		glEnableVertexAttribArray(attribute);
		sum += size;
		attribute++;
	}
}

StaticMesh::~StaticMesh()
{
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

void StaticMesh::bind()
{
	glBindVertexArray(vao);
}

void StaticMesh::draw_without_binding()
{
	glDrawArrays(GL_TRIANGLES, 0, n_vertices);
}

void StaticMesh::draw_many_without_binding(size_t x)
{
	glDrawArraysInstanced(GL_TRIANGLES, 0, n_vertices, x);
}

void StaticMesh::draw()
{
	bind();
	draw_without_binding();
}


