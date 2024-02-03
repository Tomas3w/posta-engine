#include "WeaponItem.h"

WeaponItem::WeaponItem(glm::vec3 position, Weapon* w, short ammunition)
{
	transform.set_position(position);
	weapon = w;
	this->ammunition = ammunition;
}

void WeaponItem::loop()
{
	transform.set_rotation(glm::rotate(transform.get_rotation(), static_cast<float>(M_PI * Engine::App::app->delta_time), glm::vec3(0, 1, 0)));
}

void WeaponItem::draw(Engine::ThreeDShader& shader)
{
	shader.model = transform.get_matrix();
	shader.normal_model = transform.get_normal_matrix();
	shader.global_color = {1, 1, 1, 1};
	weapon->draw();
}

short WeaponItem::get_ammunition()
{
	return ammunition;
}

Weapon* WeaponItem::get_weapon()
{
	return weapon;
}

Engine::span<uint8_t> WeaponItemChangePackage::serialize()
{
	data.resize(Engine::NetworkPackageTypeSize::size(vec));
	Writer writer(data.data());
	writer << vec;
	return Engine::make_span(data);
}

void WeaponItemChangePackage::deserialize(Engine::span<uint8_t> _data)
{
	Writer writer(_data.data());
	writer >> vec;
}

