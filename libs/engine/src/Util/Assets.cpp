#include <engine/include/Util/Assets.h>

using Engine::Mesh;

Mesh Engine::Assets::load_obj(std::filesystem::path path)
{
	Mesh mesh(&basic_vertex_props);
	std::ifstream file(path);
	if (!file)
		throw std::logic_error("Could not find file in path '" + path.string() + "'");

	std::string s;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;

	float vertex[8];
	while (getline(file, s))
	{
		if (!s.empty())
		{
			if (s[0] == 'v')
			{
				float* array = nullptr;
				switch (s[1])
				{
					case 't':
						uvs.push_back(glm::vec2(1.0f));
						array = &uvs.back()[0];
						break;
					case 'n':
						normals.push_back(glm::vec3(1.0f));
						array = &normals.back()[0];
						break;
					case ' ':
						vertices.push_back(glm::vec3(1.0f));
						array = &vertices.back()[0];
						break;
				}
				short vp = 0;
				char* i = nullptr;
				for (char& c : s)
				{
					if (c == ' ')
					{
						if (!i)
							i = &c + 1;
						else
						{
							c = '\0';
							array[vp] = atof(i);
							i = &c + 1;
							c = ' ';
							vp += 1;
							if (vp >= 2)
								break;
						}
					}
				}
				array[vp] = atof(i);
			}
			else if (s[0] == 'f')
			{
				size_t left = 2;
				for (size_t i = left; i <= s.size(); i++)
				{
					if (s[i] == ' ' || s[i] == '\0')
					{
						int indices[3];
						size_t p = left, e = 0;
						for (size_t j = left; j < i + 1; j++)
						{
							if (s[j] == '/' || s[j] == ' ' || s[j] == '\0')
							{
								char c = s[j];
								s[j] = '\0';

								indices[e] = atoi(&s[p]);
								p = j + 1;
								e++;
								
								s[j] = c;
							}
						}
						// position
						vertex[0] = vertices[indices[0] - 1][0];
						vertex[1] = vertices[indices[0] - 1][1];
						vertex[2] = vertices[indices[0] - 1][2];
						// normals
						vertex[3] = normals[indices[2] - 1][0];
						vertex[4] = normals[indices[2] - 1][1];
						vertex[5] = normals[indices[2] - 1][2];
						// uvs
						vertex[6] = uvs[indices[1] - 1][0];
						vertex[7] = 1 - uvs[indices[1] - 1][1];

						mesh.add_vertex(vertex);
						left = i + 1;
					}
				}
			}
		}
	}

	file.close();

	/*/ DEBUG
	std::cout << "Mesh vertices(" << mesh.get_vertices().size() << "): " << std::endl;
	int i = 0;
	for (const float& e : mesh.get_vertices())
	{
		std::cout << e << ", ";
		i = (i + 1) % 8;
		if (i == 0)
			std::cout << "\n";
	}
	std::cout << "\n";
	//*/

	return mesh;
}

void Engine::Assets::load_obj_vertices_and_indices(std::filesystem::path path, std::vector<GLfloat>& vertices, std::vector<int>& indices)
{
	std::ifstream file(path);
	if (!file)
		throw std::logic_error("Could not find file in path '" + path.string() + "'");

	std::string s;

	while (getline(file, s))
	{
		if (!s.empty())
		{
			if (s[0] == 'v' && s[1] == ' ')
			{
				short vp = 0;
				char* i = nullptr;
				for (char& c : s)
				{
					if (c == ' ')
					{
						if (!i)
							i = &c + 1;
						else
						{
							c = '\0';
							vertices.push_back(atof(i));
							i = &c + 1;
							c = ' ';
							vp += 1;
							if (vp >= 2)
								break;
						}
					}
				}
				vertices.push_back(atof(i));
			}
			else if (s[0] == 'f')
			{
				size_t left = 2;
				for (size_t i = left; i <= s.size(); i++)
				{
					if (s[i] == ' ' || s[i] == '\0')
					{
						for (size_t j = left; j <= s.size(); j++)
						{
							if (s[j] == '/')
							{
								s[j] = '\0';
								indices.push_back(atoi(&s[left]) - 1);
								s[j] = '/';
								break;
							}
						}
						left = i + 1;
					}
				}
			}
		}
	}

	file.close();

	/*/
	std::cout << "Vertices:";
	for (size_t i = 0; i < vertices.size(); i++)
	{
		if (i % 3 == 0)
			std::cout << std::endl;
		std::cout << vertices[i] << ", ";
	}
	std::cout << "\nIndices: ";
	for (size_t i = 0; i < indices.size(); i++)
	{
		if (i % 3 == 0)
			std::cout << std::endl;
		std::cout << indices[i] << ", ";
	}
	std::cout << "\n";
	*/
}

Mesh Engine::Assets::load_obj_and_bones(std::filesystem::path path_without_ext)
{
	std::ifstream file;
	std::string s;
	// Reading .bones
	std::filesystem::path path = path_without_ext;
	path += ".bones";
	
	//std::cout << "reading " << path << ":\n";
	file.open(path);
	std::vector<glm::vec4> bone_influences;
	while (getline(file, s))
	{
		if (s.size() == 0)
			continue;
		//int vertex_index = -1;
		std::vector<int> bone_indices;
		std::vector<float> bone_weights;

		char* previous = nullptr;
		for (char& c : s)
		{
			if (c == ' ')
			{
				c = '\0';
				//vertex_index = atoi(&s[0]);
				c = ' ';
				previous = (&c) + 1;
			}
			else if (c == '|')
			{
				c = '\0';
				bone_indices.push_back(atoi(previous));
				c = '|';
				previous = (&c) + 1;
			}
			else if (c == ',')
			{
				c = '\0';
				bone_weights.push_back(atof(previous));
				c = ',';
				previous = (&c) + 1;
			}
		}
		//std::cout << "for vertex " << vertex_index << ": ";
		
		/*
		float max = 0;
		int index_of_max;
		for (size_t i = 0; i < bone_indices.size(); i++)
		{
			//std::cout << "bone(" << bone_indices[i] << "|" << bone_weights[i] << "), ";
			if (bone_weights[i] >= max)
			{
				max = bone_weights[i];
				index_of_max = i;
			}
		}
		if (bone_indices.size())
			bone_influences.push_back(glm::vec4{bone_indices[index_of_max] + 0.9999f, -1, -1, -1});
		else
			bone_influences.push_back(glm::vec4{-1, -1, -1, -1});//a*/
	

		std::vector<std::pair<int, float>> bone_inf;
		for (size_t i = 0; i < bone_indices.size(); i++)
			bone_inf.push_back({bone_indices[i], bone_weights[i]});
		std::sort(bone_inf.begin(), bone_inf.end(), [](std::pair<int, float> a, std::pair<int, float> b){ return a.second < b.second; });
		bone_inf.resize(std::min(bone_inf.size(), static_cast<size_t>(4)));

		glm::vec4 fn = {-1, -1, -1, -1};
		for (size_t i = 0; i < bone_inf.size(); i++)
			fn[i] = std::min(bone_inf[i].second, 0.9999f) + bone_inf[i].first;
		bone_influences.push_back(fn);//glm::vec4{bone_indices[index_of_max] + 0.9999f, -1, -1, -1});
	}
	file.close();

	// Reading .obj
	path = path_without_ext;
	path += ".obj";
	Mesh mesh(&bone_vertex_props);
	file.open(path);
	if (!file)
		throw std::logic_error("Could not find file in path '" + path.string() + "'");

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
	vertices.reserve(bone_influences.size());

	float vertex[12];
	while (getline(file, s).good())
	{
		if (!s.empty())
		{
			if (s[0] == 'v')
			{
				float* array = nullptr;
				switch (s[1])
				{
					case 't':
						uvs.push_back(glm::vec2(1.0f));
						array = &uvs.back()[0];
						break;
					case 'n':
						normals.push_back(glm::vec3(1.0f));
						array = &normals.back()[0];
						break;
					case ' ':
						vertices.push_back(glm::vec3(1.0f));
						array = &vertices.back()[0];
						break;
				}
				short vp = 0;
				char* i = nullptr;
				for (char& c : s)
				{
					if (c == ' ')
					{
						if (!i)
							i = &c + 1;
						else
						{
							c = '\0';
							array[vp] = atof(i);
							i = &c + 1;
							c = ' ';
							vp += 1;
							if (vp >= 2)
								break;
						}
					}
				}
				array[vp] = atof(i);
			}
			else if (s[0] == 'f')
			{
				size_t left = 2;
				for (size_t i = left; i <= s.size(); i++)
				{
					if (s[i] == ' ' || s[i] == '\0')
					{
						int indices[3];
						size_t p = left, e = 0;
						for (size_t j = left; j < i + 1; j++)
						{
							if (s[j] == '/' || s[j] == ' ' || s[j] == '\0')
							{
								char c = s[j];
								s[j] = '\0';

								indices[e] = atoi(&s[p]);
								p = j + 1;
								e++;
								
								s[j] = c;
							}
						}
						// position
						vertex[0] = vertices[indices[0] - 1][0];
						vertex[1] = vertices[indices[0] - 1][1];
						vertex[2] = vertices[indices[0] - 1][2];
						// normals
						vertex[3] = normals[indices[2] - 1][0];
						vertex[4] = normals[indices[2] - 1][1];
						vertex[5] = normals[indices[2] - 1][2];
						// uvs
						vertex[6] = uvs[indices[1] - 1][0];
						vertex[7] = 1 - uvs[indices[1] - 1][1];
						// bones
						vertex[8] = bone_influences[indices[0] - 1][0];
						vertex[9] = bone_influences[indices[0] - 1][1];
						vertex[10] = bone_influences[indices[0] - 1][2];
						vertex[11] = bone_influences[indices[0] - 1][3];

						mesh.add_vertex(vertex);
						left = i + 1;
					}
				}
			}
		}
	}

	file.close();

	return mesh;

}

