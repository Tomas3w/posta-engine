#include <posta/Util/Shader.h>
#include <posta/App.h> // this needs to be here
#include <posta/Util/LoggingMacro.h>

using posta::Shader;

Shader::Shader(std::filesystem::path path)
{
	int success;
	char info_log[512];

	// vertex
	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	std::string vertex_shader_file = posta::read_file(path / "vertex.glsl");
	const char* vertex_shader_source = vertex_shader_file.c_str();
	glShaderSource(vertex_shader, 1, &vertex_shader_source, nullptr);
	glCompileShader(vertex_shader);

	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex_shader, sizeof(info_log), nullptr, info_log);
		throw std::logic_error(std::string("Shader(string path) -> vertex_shader: \n") + info_log);
	}

	// fragment
	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	std::string fragment_shader_file = posta::read_file(path / "fragment.glsl");
	//LOG(fragment_shader_file);
	const char* fragment_shader_source = fragment_shader_file.c_str();
	glShaderSource(fragment_shader, 1, &fragment_shader_source, nullptr);
	glCompileShader(fragment_shader);

	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment_shader, sizeof(info_log), nullptr, info_log);
		throw std::logic_error(std::string("Shader(string path) -> fragment_shader: \n") + info_log);
	}

	// program
	shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);

	glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shader_program, sizeof(info_log), nullptr, info_log);
		throw std::logic_error(std::string("Shader(string path) -> program_shader: \n") + info_log);
	}

	// deleting shaders
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

}

Shader::~Shader()
{
	glDeleteProgram(shader_program);
}

GLuint Shader::get_program() const
{
	return shader_program;
}

void Shader::bind()
{
	glUseProgram(shader_program);
	posta::App::app->current_shader = this;
}

