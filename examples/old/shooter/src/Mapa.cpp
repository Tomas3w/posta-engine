#include "Mapa.h"

Mapa::Mapa(std::filesystem::path path) :
	mesh(Engine::Assets::load_obj(path / "mesh.obj")),
	texture(path / "texture.png")
{
	// Rigidbody
	std::vector<GLfloat> vertices;
	std::vector<int> indices;
	
	auto mash = Engine::Assets::load_obj(path / "mesh.obj");
	body.reset(new Engine::Component::MeshStaticRigidbody(glm::vec3(0, 0, 0), mash));
	
	// Reading .config
	Engine::ConfigFile config(path / "info.config");

	Engine::ConfigFile::Data spawns = Engine::ConfigFile::to_data(config["spawns"]);
	for (auto& spawn_point : spawns)
		spawn_positions.push_back(glm::vec3(spawn_point[0].to_float(), spawn_point[1].to_float(), spawn_point[2].to_float()));

	Engine::ConfigFile::Data items = Engine::ConfigFile::to_data(config["items"]);
	for (auto& item : items)
		spawn_items.push_back(SpawnItem{WeaponItem(glm::vec3(item[0].to_float(), item[1].to_float(), item[2].to_float()), Weapon::get_weapon(0), 30)});

	//for (auto& p : spawn_items)
	//	std::cout << to_string(p.weapon_item.transform.get_position()) << std::endl;
}

void Mapa::loop_for_client(std::vector<std::unique_ptr<WeaponItem>>& weapons_items)
{
	for (auto& item : weapons_items)
	{
		if (item)
			item->loop();
	}
}

WeaponItemChangePackage Mapa::loop(std::vector<std::unique_ptr<WeaponItem>>& weapons_items, std::unordered_map<std::string, std::unique_ptr<Player>>& players)
{
	WeaponItemChangePackage r;
	for (size_t i = 0; i < weapons_items.size(); i++)
	{
		auto& item = weapons_items[i];
		if (item)
		{
			item->loop();
			for (auto& player : players)
			{
				if (glm::distance(item->transform.get_position(), player.second->transform.get_position()) < 3.8f)
				{
					Player::weapon_pick_up_sound->play(0.8f);
					player.second->replenish(*item.get());
					item.reset();
					spawn_items[i].time_to_respawn = 10;

					r.vec.push_back(i);
					break;
				}
			}
		}
	}
	for (size_t i = 0; i < weapons_items.size(); i++)
	{
		if (spawn_items[i].time_to_respawn > 0)
			spawn_items[i].time_to_respawn -= Engine::App::app->delta_time;
		else if (!weapons_items[i])
		{
			weapons_items[i].reset(new WeaponItem(spawn_items[i].weapon_item));
			r.vec.push_back(i);
			r.vec.back() |= 1ul << 31;
		}
	}
	return r;
}

void Mapa::fill_items(std::vector<std::unique_ptr<WeaponItem>>& weapons_items)
{
	for (auto& p : spawn_items)
		weapons_items.push_back(std::make_unique<WeaponItem>(p.weapon_item));
}

void Mapa::fill_players(std::vector<std::string> nicknames, std::unordered_map<std::string, std::unique_ptr<Player>>& players)
{
	std::vector<glm::vec3> positions = spawn_positions;
	for (auto& nick : nicknames)
	{
		if (positions.empty())
			positions = spawn_positions;
		glm::vec3& position = *positions.begin();
		players[nick] = std::make_unique<Player>(position, nick);
		positions.erase(positions.begin());
	}
}

void Mapa::spawn_player(std::string nickname, std::unordered_map<std::string, std::unique_ptr<Player>>& players)
{
	if (!players.count(nickname))
		players[nickname] = std::make_unique<Player>(spawn_positions[rand() % spawn_positions.size()], nickname);
}

void Mapa::draw(Engine::ThreeDShader& shader)
{
	shader.model = glm::mat4(1.0f);
	shader.normal_model = glm::mat4(1.0f);
	shader.global_color = {1, 1, 1, 1};
	texture.bind();
	mesh.draw();
}

void Mapa::revive_weapon_item(std::vector<std::unique_ptr<WeaponItem>>& weapons_items, size_t i)
{
	weapons_items[i].reset(new WeaponItem(spawn_items[i].weapon_item));
}

void Mapa::remove_weapon_item(std::vector<std::unique_ptr<WeaponItem>>& weapons_items, size_t i)
{
	weapons_items[i].reset();
}

