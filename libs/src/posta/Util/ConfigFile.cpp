#include <posta/Util/ConfigFile.h>
#include <posta/Util/LoggingMacro.h>

using posta::ConfigFile;

ConfigFile::Data::Data(std::string value)
{
	this->value = value;
	is_list = false;
}

ConfigFile::Data::Data(std::vector<ConfigFile::Data> values)
{
	this->values = values;
	is_list = true;
}

std::string ConfigFile::Data::to_string()
{
	return value;
}

int ConfigFile::Data::to_int()
{
	if (is_list)
		throw std::runtime_error(("tried to access int from list Data object (from ConfigFile); '" + to_string() + "'").c_str());
	return stoi(value);
}

float ConfigFile::Data::to_float()
{
	if (is_list)
		throw std::runtime_error(("tried to access float from list Data object (from ConfigFile); '" + to_string() + "'").c_str());
	return stof(value);
}

double ConfigFile::Data::to_double()
{
	if (is_list)
		throw std::runtime_error(("tried to access double from list Data object (from ConfigFile); '" + to_string() + "'").c_str());
	return stod(value);
}

ConfigFile::Data& ConfigFile::Data::operator[](size_t index)
{
	if (!is_list)
		throw std::runtime_error(("tried to access element from non-list Data object (from ConfigFile); '" + to_string() + "'").c_str());
	return values.at(index);
}

std::vector<ConfigFile::Data>::iterator ConfigFile::Data::begin()
{
	if (!is_list)
		throw std::runtime_error(("tried to access begin() from non-list Data object (from ConfigFile); '" + to_string() + "'").c_str());
	return values.begin();
}

std::vector<ConfigFile::Data>::iterator ConfigFile::Data::end()
{
	if (!is_list)
		throw std::runtime_error(("tried to access end() from non-list Data object (from ConfigFile); '" + to_string() + "'").c_str());
	return values.end();
}

size_t ConfigFile::Data::size()
{
	if (!is_list)
		throw std::runtime_error(("tried to query size from non-list Data object (from ConfigFile); '" + to_string() + "'").c_str());
	return values.size();
}

glm::vec2 ConfigFile::Data::to_vec2()
{
	if (!is_list)
		throw std::runtime_error(("tried to access glm::vec2 from non-list Data object (from ConfigFile); '" + to_string() + "'").c_str());
	if (values.size() != 2)
		throw std::runtime_error(("tried to access glm::vec2 from list Data object[with size()=" + std::to_string(size()) + " instead of 2] (from ConfigFile); '" + to_string() + "'").c_str());
	return glm::vec2(values[0].to_float(), values[1].to_float());
}
glm::vec3 ConfigFile::Data::to_vec3()
{
	if (!is_list)
		throw std::runtime_error(("tried to access glm::vec3 from non-list Data object (from ConfigFile); '" + to_string() + "'").c_str());
	if (values.size() != 3)
		throw std::runtime_error(("tried to access glm::vec3 from list Data object[with size()=" + std::to_string(size()) + " instead of 3] (from ConfigFile); '" + to_string() + "'").c_str());
	return glm::vec3(values[0].to_float(), values[1].to_float(), values[2].to_float());
}
glm::vec4 ConfigFile::Data::to_vec4()
{
	if (!is_list)
		throw std::runtime_error(("tried to access glm::vec4 from non-list Data object (from ConfigFile); '" + to_string() + "'").c_str());
	if (values.size() != 4)
		throw std::runtime_error(("tried to access glm::vec4 from list Data object[with size()=" + std::to_string(size()) + " instead of 4] (from ConfigFile); '" + to_string() + "'").c_str());
	return glm::vec4(values[0].to_float(), values[1].to_float(), values[2].to_float(), values[3].to_float());
}
glm::ivec2 ConfigFile::Data::to_ivec2()
{
	if (!is_list)
		throw std::runtime_error(("tried to access glm::ivec2 from non-list Data object (from ConfigFile); '" + to_string() + "'").c_str());
	if (values.size() != 2)
		throw std::runtime_error(("tried to access glm::ivec2 from list Data object[with size()=" + std::to_string(size()) + " instead of 2] (from ConfigFile); '" + to_string() + "'").c_str());
	return glm::ivec2(values[0].to_int(), values[1].to_int());
}
glm::ivec3 ConfigFile::Data::to_ivec3()
{
	if (!is_list)
		throw std::runtime_error(("tried to access glm::ivec3 from non-list Data object (from ConfigFile); '" + to_string() + "'").c_str());
	if (values.size() != 3)
		throw std::runtime_error(("tried to access glm::ivec3 from list Data object[with size()=" + std::to_string(size()) + " instead of 3] (from ConfigFile); '" + to_string() + "'").c_str());
	return glm::ivec3(values[0].to_int(), values[1].to_int(), values[2].to_int());
}
glm::ivec4 ConfigFile::Data::to_ivec4()
{
	if (!is_list)
		throw std::runtime_error(("tried to access glm::ivec4 from non-list Data object (from ConfigFile); '" + to_string() + "'").c_str());
	if (values.size() != 4)
		throw std::runtime_error(("tried to access glm::ivec4 from list Data object[with size()=" + std::to_string(size()) + " instead of 4] (from ConfigFile); '" + to_string() + "'").c_str());
	return glm::ivec4(values[0].to_int(), values[1].to_int(), values[2].to_int(), values[3].to_int());
}
glm::dvec2 ConfigFile::Data::to_dvec2()
{
	if (!is_list)
		throw std::runtime_error(("tried to access glm::dvec2 from non-list Data object (from ConfigFile); '" + to_string() + "'").c_str());
	if (values.size() != 2)
		throw std::runtime_error(("tried to access glm::dvec2 from list Data object[with size()=" + std::to_string(size()) + " instead of 2] (from ConfigFile); '" + to_string() + "'").c_str());
	return glm::dvec2(values[0].to_double(), values[1].to_double());
}
glm::dvec3 ConfigFile::Data::to_dvec3()
{
	if (!is_list)
		throw std::runtime_error(("tried to access glm::dvec3 from non-list Data object (from ConfigFile); '" + to_string() + "'").c_str());
	if (values.size() != 3)
		throw std::runtime_error(("tried to access glm::dvec3 from list Data object[with size()=" + std::to_string(size()) + " instead of 3] (from ConfigFile); '" + to_string() + "'").c_str());
	return glm::dvec3(values[0].to_double(), values[1].to_double(), values[2].to_double());
}
glm::dvec4 ConfigFile::Data::to_dvec4()
{
	if (!is_list)
		throw std::runtime_error(("tried to access glm::dvec4 from non-list Data object (from ConfigFile); '" + to_string() + "'").c_str());
	if (values.size() != 4)
		throw std::runtime_error(("tried to access glm::dvec4 from list Data object[with size()=" + std::to_string(size()) + " instead of 4] (from ConfigFile); '" + to_string() + "'").c_str());
	return glm::dvec4(values[0].to_double(), values[1].to_double(), values[2].to_double(), values[3].to_double());
}

ConfigFile::Data ConfigFile::to_data(std::string str)
{
	std::string stripped = posta::strip(str);
	if (stripped.at(0) == '{' && stripped.at(stripped.size() - 1) == '}')
		return to_list(stripped);
	return to_value(str);
}

ConfigFile::Data ConfigFile::to_value(std::string str)
{
	return Data(str);
}

ConfigFile::Data ConfigFile::to_list(std::string str)
{
	str = posta::strip(str);
	str.insert(str.end() - 1, ',');

	int parenthesis = 0;
	char* left = str.data() + 1;
	std::vector<std::string> elements;
	for (auto& c : str)
	{
		if (c == '{')
			parenthesis++;
		else if (c == '}')
			parenthesis--;
		else if (c == ',' && parenthesis == 1)
		{
			char r = c;
			c = '\0';
			elements.push_back(left);
			c = r;
			left = (&c) + 1;
		}
	}
	
	std::vector<ConfigFile::Data> nelements;
	for (auto& s : elements)
	{
		if (!s.empty())
			nelements.push_back(ConfigFile::to_data(s));
	}
	return Data(nelements);
}

ConfigFile::ConfigFile(std::filesystem::path path)
{
	std::ifstream file(path);
	if (!file)
		throw std::logic_error(std::string("Couldn't load file '") + path.string() + "'");
	std::string line;
	size_t line_pos = 0;
	while (getline(file, line))
	{
		// Skipping comments
		size_t comment_pos = line.find('#');
		if (comment_pos != std::string::npos)
			line.resize(comment_pos);
		// Skipping lines that only contain whitespaces
		bool is_other_than_whitespace = false;
		for (char& c : line)
		{
			if (!std::isspace(c))
			{
				is_other_than_whitespace = true;
				break;
			}
		}
		if (!is_other_than_whitespace)
			continue;

		// Dividing the string in two parts
		size_t pos = line.find('=');
		if (pos == std::string::npos)
			throw std::logic_error(std::string("couldn't find the '=' in one of the config pairs, in line ") + std::to_string(line_pos + 1));
		std::string left, right;
		left.insert(left.begin(), line.begin(), line.begin() + pos);
		right.insert(right.begin(), line.begin() + pos + 1, line.end());

		// Adding this to the raw data
		data[posta::strip(left)] = right;
		line_pos++;
	}
	file.close();
}

ConfigFile::Data ConfigFile::operator[](std::string key)
{
	if (data.count(key) == 0)
		throw std::out_of_range(std::string("couldn't get key '") + key + "'");
	return ConfigFile::to_data(data.at(key));
}

bool ConfigFile::contains(std::string key)
{
	return data.count(key);
}

std::string ConfigFile::to_string()
{
	std::string s;
	for (auto line : data)
		s += line.first + "=" + line.second + "\n";
	return s;
}

