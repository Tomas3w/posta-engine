#include "ConfigFile.h"

ConfigFile::Data::Data(std::string value)
{
	this->value = value;
}

ConfigFile::Data::Data(std::vector<ConfigFile::Data> values)
{
	this->values = values;
}

std::string ConfigFile::Data::to_string()
{
	return value;
}

int ConfigFile::Data::to_int()
{
	return stoi(value);
}

float ConfigFile::Data::to_float()
{
	return stof(value);
}

ConfigFile::Data& ConfigFile::Data::operator[](size_t index)
{
	return values.at(index);
}

std::vector<ConfigFile::Data>::iterator ConfigFile::Data::begin()
{
	return values.begin();
}

std::vector<ConfigFile::Data>::iterator ConfigFile::Data::end()
{
	return values.end();
}

size_t ConfigFile::Data::size()
{
	return values.size();
}

ConfigFile::Data ConfigFile::to_data(std::string str)
{
	str = Engine::strip(str);
	if (str.empty() || str[0] != '(' || str[str.size() - 1] != ')')
		return Data(str);
	str.insert(str.end() - 1, ',');

	int parenthesis = 0;
	char* left = str.data() + 1;
	std::vector<std::string> elements;
	for (auto& c : str)
	{
		if (c == '(')
			parenthesis++;
		else if (c == ')')
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
	size_t line_pos;
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
			throw std::logic_error(std::string("Couldn't find the '=' in one of the config pairs, in line ") + std::to_string(line_pos + 1));
		std::string left, right;
		left.insert(left.begin(), line.begin(), line.begin() + pos);
		right.insert(right.begin(), line.begin() + pos + 1, line.end());

		// Adding this to the raw data
		data[Engine::strip(left)] = Engine::strip(right);
		line_pos++;
	}
	file.close();
}

std::string& ConfigFile::operator[](std::string key)
{
	if (data.count(key) == 0)
		throw std::out_of_range(std::string("couldn't get key '") + key + "'");
	return data.at(key);
}

std::string ConfigFile::to_string()
{
	std::string s;
	for (auto line : data)
		s += line.first + "=" + line.second + "\n";
	return s;
}

