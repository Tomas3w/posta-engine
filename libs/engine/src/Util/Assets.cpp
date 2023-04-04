#include <engine/include/Util/Assets.h>
#include <engine/include/Util/LoggingMacro.h>

using Engine::Mesh;

Mesh Engine::Assets::load_obj(std::filesystem::path path)
{
	//LOG("loading path: ", path);
	Mesh mesh(&basic_vertex_props);
	std::ifstream file(path);
	if (!file)
		throw std::logic_error("Could not find file in path '" + path.string() + "'");

	int i_position = mesh.get_index_by_name("position");
	int i_normal = mesh.get_index_by_name("normal");
	int i_uv = mesh.get_index_by_name("uv");

	std::string s;

	//float vertex[8];
	while (getline(file, s))
	{
		if (!s.empty())
		{
			if (s[0] == 'v')
			{
				float array[3];
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
				switch (s[1])
				{
					case 't':
						mesh.add_data<2>(i_uv, {array[0], 1 - array[1]});
						break;
					case 'n':
						mesh.add_data<3>(i_normal, {array[0], array[1], array[2]});
						break;
					case ' ':
						mesh.add_data<3>(i_position, {array[0], array[1], array[2]});
						break;
				}
			}
			else if (s[0] == 'f')
			{
				size_t left = 2;
				int face_index = 0;
				std::array<std::vector<int>, 3> face;
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
						face[face_index] = {indices[0] - 1, indices[2] - 1, indices[1] - 1};//mesh.add_face({{,,}});
						//LOG("adding face! ", face_index);
						if (face_index == 2)
						{
							mesh.add_face(face);
							face_index = 0;
						}
						else
						{
							//LOG("this does not happen for some reason...");
							face_index++;
						}
						/*/ position
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
						left = i + 1;*/
						left = i + 1;
					}
				}
			}
		}
	}

	file.close();

	mesh.lock();
	return mesh;
}

Mesh Engine::Assets::load_obj_with_bones(std::filesystem::path path)
{
	Mesh mesh(&bone_vertex_props);
	int i_position = mesh.get_index_by_name("position");
	int i_normal = mesh.get_index_by_name("normal");
	int i_uv = mesh.get_index_by_name("uv");
	int i_weights = mesh.get_index_by_name("weights");

	std::ifstream file;
	std::string s;
	// Reading .bones
	std::filesystem::path path_a = path;
	path_a += ".bones";
	
	file.open(path_a);
	if (!file)
		throw std::logic_error("could not find file in path '" + path_a.string() + "'");
	while (getline(file, s))
	{
		if (s.size() == 0)
			continue;
		std::vector<int> bone_indices;
		std::vector<float> bone_weights;

		char* previous = nullptr;
		for (char& c : s)
		{
			if (c == ' ')
			{
				c = '\0';
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

		std::vector<std::pair<int, float>> bone_inf;
		for (size_t i = 0; i < bone_indices.size(); i++)
			bone_inf.push_back({bone_indices[i], bone_weights[i]});
		std::sort(bone_inf.begin(), bone_inf.end(), [](std::pair<int, float> a, std::pair<int, float> b){ return a.second < b.second; });
		bone_inf.resize(std::min(bone_inf.size(), static_cast<size_t>(4)));
		float max_value_of_bone_inf = -1;
		for (auto& b : bone_inf)
		{
			if (b.second > max_value_of_bone_inf)
				max_value_of_bone_inf = b.second;
		}
		for (auto& b : bone_inf)
			b.second /= max_value_of_bone_inf;

		glm::vec4 fn = {-1, -1, -1, -1};
		for (size_t i = 0; i < bone_inf.size(); i++)
			fn[i] = std::min(bone_inf[i].second, 0.9999f) + bone_inf[i].first;
		mesh.add_data<4>(i_weights, fn);
	}
	file.close();

	auto path_b = path;
	path_b += ".obj";
	file.open(path_b);
	if (!file)
		throw std::logic_error("could not find file in path '" + path_b.string() + "'");

	while (getline(file, s))
	{
		if (!s.empty())
		{
			if (s[0] == 'v')
			{
				float array[3];
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
				switch (s[1])
				{
					case 't':
						mesh.add_data<2>(i_uv, {array[0], 1 - array[1]});
						break;
					case 'n':
						mesh.add_data<3>(i_normal, {array[0], array[1], array[2]});
						break;
					case ' ':
						mesh.add_data<3>(i_position, {array[0], array[1], array[2]});
						break;
				}
			}
			else if (s[0] == 'f')
			{
				size_t left = 2;
				int face_index = 0;
				std::array<std::vector<int>, 3> face;
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
						face[face_index] = {indices[0] - 1, indices[2] - 1, indices[1] - 1, indices[0] - 1};//mesh.add_face({{,,}});
						if (face_index == 2)
						{
							mesh.add_face(face);
							face_index = 0;
						}
						else
						{
							face_index++;
						}
						left = i + 1;
					}
				}
			}
		}
	}

	file.close();

	mesh.lock();
	return mesh;
}

