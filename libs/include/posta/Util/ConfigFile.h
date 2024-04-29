#ifndef POSTAENGINE_CONFIGFILE_UTILITY_H
#define POSTAENGINE_CONFIGFILE_UTILITY_H
#include <filesystem>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <fstream>
#include <posta/Util/General.h>

namespace posta {
	/// ConfigFile is a very simple class capable of loading named attributes from a file
	/// The format of the configuration file is the following:
	/// - Each line represents a named value (or named list of values)
	/// - Each line starts with a name (alphanumeric name) and equals sign (=) and at the right side one of two possibilities:
	///   - A value, which can be anything, each value is interpreted as the program suggest and there is no need to use the utilities of the class.
	///   - A list, a list is an enclosed space with ',' as separators and curly braces as the enclosing parenthesis, anything other than '{', '}', and ',' is interpreted as part of one of the strings contained within the list.
	/// example.txt:
	/// a = b
	/// hello = 54.32
	/// ls = {a, b, c}
	/// Let config_file = ConfigFile("example.txt"), then to access 'hello' as a string use config_file["hello"].to_string(), to access the first element of 'ls' do config_file["ls"][0]
	class ConfigFile
	{
		struct Data
		{
			Data(std::string value);
			Data(std::vector<Data> values);
			// If data is an individual value
			std::string to_string();
			int to_int();
			float to_float();
			double to_double();
			// If data is a list
			Data& operator[](size_t index);
			std::vector<Data>::iterator begin();
			std::vector<Data>::iterator end();
			size_t size();
			glm::vec2 to_vec2();
			glm::vec3 to_vec3();
			glm::vec4 to_vec4();
			glm::ivec2 to_ivec2();
			glm::ivec3 to_ivec3();
			glm::ivec4 to_ivec4();
			glm::dvec2 to_dvec2();
			glm::dvec3 to_dvec3();
			glm::dvec4 to_dvec4();

			bool is_list;
			std::string value;
			std::vector<Data> values;
		};
		static Data to_data(std::string str);
	public:
		static Data to_value(std::string str);
		static Data to_list(std::string str);
		ConfigFile(std::filesystem::path path);

		Data operator[](std::string key);
		bool contains(std::string key);
		std::string to_string();

		std::unordered_map<std::string, std::string> data;
	};
}

#endif // POSTAENGINE_CONFIGFILE_UTILITY_H
