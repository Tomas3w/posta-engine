#include <posta/Util/General.h>
#include <posta/Util/LoggingMacro.h>

std::string Engine::read_file(std::filesystem::path path)
{
	std::ifstream file(path, std::ios::in|std::ios::ate);
	if (!file)
		throw std::logic_error("read_file failed opening file: '" + path.string() + "', error: " + strerror(errno));

	std::streamsize size = file.tellg();
	file.seekg(0);

	char* s = new char[size + 1];
	memset(s, 0, size + 1);
	file.read(s, size);

	file.close();
	std::string r = s;
	delete[] s;
	return r;
}

btVector3 Engine::to_btVector3(glm::vec3 vec)
{
	return btVector3(vec.x, vec.y, vec.z);
}

glm::vec3 Engine::from_btVector3(btVector3 vec)
{
	return glm::vec3(vec.x(), vec.y(), vec.z());
}

std::string Engine::strip(std::string str)
{
	std::string s;
	size_t first = std::string::npos;
	size_t last = std::string::npos;
	for (size_t i = 0; i < str.size(); i++)
	{
		if (first == std::string::npos)
		{
			if (!std::isspace(str[i]))
				first = i;
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

std::vector<std::string> Engine::split(std::string str, char separator)
{
	std::stringstream str_s(str);
	std::string element;
	std::vector<std::string> elements;
	while (getline(str_s, element, separator))
		elements.push_back(element);
	return elements;
}

