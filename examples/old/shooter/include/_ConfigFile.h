#ifndef CONFIGFILE_H
#define CONFIGFILE_H
#include <filesystem>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <fstream>
#include <engine/include/Util/General.h>

class ConfigFile
{
public:
	struct Data
	{
		Data(std::string value);
		Data(std::vector<Data> values);
		// If data is an individual value
		std::string to_string();
		int to_int();
		float to_float();
		// If data is a list
		Data& operator[](size_t index);
		std::vector<Data>::iterator begin();
		std::vector<Data>::iterator end();
		size_t size();

		std::string value;
		std::vector<Data> values;
	};
	static Data to_data(std::string str);
	ConfigFile(std::filesystem::path path);

	std::string& operator[](std::string key);
	std::string to_string();

	std::unordered_map<std::string, std::string> data;
};

#endif // CONFIGFILE_H
