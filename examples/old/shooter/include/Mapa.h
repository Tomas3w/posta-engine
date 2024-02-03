#ifndef MAPA_H
#define MAPA_H
#include "WeaponItem.h"
#include <engine/include/Util/ConfigFile.h>
#include "Player.h"
#include <engine/include/Component/MeshStaticRigidbody.h>

class Mapa
{
public:
	/// Path to map folder, with files mesh.obj, texture.png, and info.config
	Mapa(std::filesystem::path path);

	void loop_for_client(std::vector<std::unique_ptr<WeaponItem>>& weapons_items);
	WeaponItemChangePackage loop(std::vector<std::unique_ptr<WeaponItem>>& weapons_items, std::unordered_map<std::string, std::unique_ptr<Player>>& players);
	void fill_items(std::vector<std::unique_ptr<WeaponItem>>& weapons_items);
	void fill_players(std::vector<std::string> nicknames, std::unordered_map<std::string, std::unique_ptr<Player>>& players);
	void spawn_player(std::string nickname, std::unordered_map<std::string, std::unique_ptr<Player>>& players);
	void draw(Engine::ThreeDShader& shader);

	void revive_weapon_item(std::vector<std::unique_ptr<WeaponItem>>& weapons_items, size_t i);
	void remove_weapon_item(std::vector<std::unique_ptr<WeaponItem>>& weapons_items, size_t i);

	std::vector<glm::vec3> spawn_positions;

	struct SpawnItem
	{
		WeaponItem weapon_item;
		float time_to_respawn = 0;
	};
	std::vector<SpawnItem> spawn_items;
private:
	Engine::Component::StaticMesh mesh;
	Engine::Component::Texture texture;
	std::unique_ptr<Engine::Component::MeshStaticRigidbody> body;

	//Engine::Mesh mash; // mesh for body
};

#endif // MAPA_H
