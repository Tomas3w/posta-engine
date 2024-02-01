#include <posta/Component/SmoothStaticMesh.h>
#include <posta/Util/LoggingMacro.h>

using posta::component::SmoothStaticMesh;

SmoothStaticMesh::SmoothStaticMesh(Mesh mesh)
{
	mesh.generate_elements_data({"position", "uv"});

	/*LOG("testing... ", mesh.get_elements_vertex_data().size());
	for (auto vertex : mesh.get_elements_vertex_data())
	{
		LOG(vertex);
	}
	LOG("tasting... ", mesh.get_elements_indices_ushort().size());
	for (auto i : mesh.get_elements_indices_ushort())
	{
		LOG(i);
	}*/

	const VertexProperties* props = mesh.get_vertex_properties();
	size_t n_vertices = mesh.get_elements_vertex_data().size() / props->sum_of_sizes;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, n_vertices * props->sum_of_sizes * sizeof(float), mesh.get_elements_vertex_data().data(), GL_STATIC_DRAW);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	if (mesh.are_elements_indices_short())
	{
		index_type = GL_UNSIGNED_SHORT;
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * mesh.get_elements_indices_ushort().size(), mesh.get_elements_indices_ushort().data(), GL_STATIC_DRAW);
		n_indices = mesh.get_elements_indices_ushort().size();
	}
	else
	{
		index_type = GL_UNSIGNED_INT;
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * mesh.get_elements_indices_uint().size(), mesh.get_elements_indices_uint().data(), GL_STATIC_DRAW);
		n_indices = mesh.get_elements_indices_uint().size();
	}

	GLuint attribute = 0;
	GLubyte sum = 0;
	for (const GLubyte& size : props->attributes_sizes)
	{
		glVertexAttribPointer(attribute, size, GL_FLOAT, GL_FALSE, props->sum_of_sizes * sizeof(float), (void*)(sum * sizeof(float)));
		glEnableVertexAttribArray(attribute);
		sum += size;
		attribute++;
	}
	/*/ DEBUG
	const VertexProperties* props = mesh.get_vertex_properties();

	std::vector<GLfloat> vertex_data;
	for (auto i : mesh.get_elements_indices_ushort())
	{
		//mesh.get_elements_vertex_data()
		vertex_data.insert(vertex_data.end(), mesh.get_elements_vertex_data().begin() + i * props->sum_of_sizes, mesh.get_elements_vertex_data().begin() + (i + 1) * props->sum_of_sizes);
	}

	n_indices = vertex_data.size() / props->sum_of_sizes;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, n_indices * props->sum_of_sizes * sizeof(float), vertex_data.data(), GL_STATIC_DRAW);

	GLuint attribute = 0;
	GLubyte sum = 0;
	for (const GLubyte& size : props->attributes_sizes)
	{
		glVertexAttribPointer(attribute, size, GL_FLOAT, GL_FALSE, props->sum_of_sizes * sizeof(float), (void*)(sum * sizeof(float)));
		glEnableVertexAttribArray(attribute);
		sum += size;
		attribute++;
	}*/
}

SmoothStaticMesh::~SmoothStaticMesh()
{
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteVertexArrays(1, &vao);
}

void SmoothStaticMesh::bind() const
{
	glBindVertexArray(vao);
}

void SmoothStaticMesh::draw_without_binding() const
{
	glDrawElements(GL_TRIANGLES, n_indices, index_type, 0);
	//glDrawArrays(GL_TRIANGLES, 0, n_indices); // DEBUG
}

void SmoothStaticMesh::draw_many_without_binding(size_t x) const
{
	glDrawElementsInstanced(GL_TRIANGLES, n_indices, index_type, 0, x);
}

void SmoothStaticMesh::draw() const
{
	bind();
	draw_without_binding();
}

