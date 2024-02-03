#include "Weapon.h"

std::vector<std::unique_ptr<Weapon>> Weapon::weapons;

void Weapon::load_weapons()
{
	if (weapons.empty())
	{
		for (std::filesystem::path path : {
			"assets/weapons/0",
		})
			weapons.push_back(std::make_unique<Weapon>(path, std::stoi(path.stem().string())));
	}
}

Weapon* Weapon::get_weapon(int index)
{
	load_weapons();
	if (index < 0 || static_cast<size_t>(index) >= weapons.size())
		return nullptr;
	return weapons[index].get();
}

Weapon::Weapon(std::filesystem::path path, int number) :
	mesh(Engine::Assets::load_obj(path.replace_extension(".obj"))),
	texture(path.replace_extension(".png"))
{
	this->number = number;
	switch (number)
	{
	case 0:
		max_charged_ammunition = 30;
		charge_time = 4;
		break;
	default:
		throw std::logic_error(std::string("Undefined data for weapon: ") + std::to_string(number));
	}
}

void Weapon::draw()
{
	texture.bind();
	mesh.draw();
}

