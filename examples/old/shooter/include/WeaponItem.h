#ifndef WEAPONITEM_H
#define WEAPONITEM_H
#include "Weapon.h"
#include <engine/include/Util/NetworkPackage.h>
#include "PackagesTypes.h"
#include <engine/include/Component/Transform.h>
#include <engine/include/Util/Shader.h>

class WeaponItem
{
public:
	WeaponItem(glm::vec3 position, Weapon* w, short ammunition);

	void loop();
	void draw(Engine::ThreeDShader& shader);

	short get_ammunition();
	Weapon* get_weapon();

	Engine::Component::Transform transform;
private:
	short ammunition;
	Weapon* weapon;
};

struct WeaponItemChangePackage : public Engine::NetworkPackage
{
	WeaponItemChangePackage() = default;
	uint32_t get_type() { return static_cast<uint32_t>(NetworkPackageType::WEAPON_ITEM_CHANGE); }
	Engine::span<uint8_t> serialize();
	void deserialize(Engine::span<uint8_t>);

	std::vector<uint32_t> vec;
};

#endif // WEAPONITEM_H
