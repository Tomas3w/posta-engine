#include <posta/Util/General.h>
#include <posta/Util/LoggingMacro.h>

std::string posta::read_file(std::filesystem::path path)
{
	std::ifstream file(path, std::ios::in|std::ios::binary|std::ios::ate);
	if (!file)
		throw std::logic_error("read_file failed opening file: '" + path.string() + "', error: " + strerror(errno));

	std::streamsize size = file.tellg();
	file.seekg(0);

	std::string r(size, '\0');
	file.read(r.data(), size);

	file.close();
	return r;
}

btVector3 posta::to_btVector3(glm::vec3 vec)
{
	return btVector3(vec.x, vec.y, vec.z);
}

glm::vec3 posta::from_btVector3(btVector3 vec)
{
	return glm::vec3(vec.x(), vec.y(), vec.z());
}

std::string posta::strip(std::string str)
{
	std::string s;
	size_t first = std::string::npos;
	size_t last = std::string::npos;
	for (size_t i = 0; i < str.size(); i++)
	{
		if (first == std::string::npos)
		{
			if (!std::isspace(str[i]))
			{
				first = i;
				last = i;
			}
		}
		else if (!std::isspace(str[i]))
		{
			last = i;
		}
	}
	if (first == std::string::npos)
		first = 0;
	if (last == std::string::npos)
		last = str.size() - 1;
	//std::cout << "where?" << std::endl;
	s.insert(s.end(), str.begin() + first, str.begin() + last + 1);
	return s;
}

std::vector<std::string> posta::split(std::string str, char separator)
{
	std::stringstream str_s(str);
	std::string element;
	std::vector<std::string> elements;
	while (getline(str_s, element, separator))
		elements.push_back(element);
	return elements;
}

std::string posta::get_glError(GLenum error)
{
	switch (error)
	{
		case GL_INVALID_ENUM:
			return "GL_INVALID_ENUM";
		case GL_INVALID_VALUE:
			return "GL_INVALID_VALUE";
		case GL_INVALID_OPERATION:
			return "GL_INVALID_OPERATION";
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			return "GL_INVALID_FRAMEBUFFER_OPERATION";
		case GL_OUT_OF_MEMORY:
			return "GL_OUT_OF_MEMORY";
		case GL_STACK_UNDERFLOW:
			return "GL_STACK_UNDERFLOW";
		case GL_STACK_OVERFLOW:
			return "GL_STACK_OVERFLOW";
	}
	return "GL_NO_ERROR";
}

