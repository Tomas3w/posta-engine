#ifndef WEAPON_H
#define WEAPON_H
#include <engine/include/Component/Texture.h>
#include <engine/include/Component/StaticMesh.h>
#include <engine/include/Util/Assets.h>
#include <engine/include/App.h>
#include <memory>
#define WEAPONS_AVAILABLE 4

class Weapon
{
public:
	static std::vector<std::unique_ptr<Weapon>> weapons;
	static void load_weapons();
	static Weapon* get_weapon(int index);
	
	/// Path to filename (with no extension) of both .obj and .png
	Weapon(std::filesystem::path path, int number);

	void draw();

	int number;
	short max_charged_ammunition;
	float charge_time;
private:
	Engine::Component::StaticMesh mesh;
	Engine::Component::Texture texture;
};

#endif // WEAPON_H
