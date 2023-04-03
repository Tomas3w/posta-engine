#include "Rocket.h"
#include "App.h"

Rocket::Resources::Resources() :
	rocket_smesh(Engine::Assets::load_obj("assets/rocket.obj")),
	rocket_texture("assets/rocket.png")
{}

Rocket::Resources* Rocket::bag = nullptr;

Rocket::Rocket(glm::vec3 pos) :
	rb(pos, 1, {1.091f, 5.980f, 1.091f})
{}

void Rocket::draw()
{
	Engine::Component::Transform transform = rb.get_transform();
	transform.set_position(transform.get_position() + transform.to_local({0, -0.65219f, 0}));
	app->shadow_shader.model = transform.get_matrix();
	app->shadow_shader.normal_model = transform.get_normal_matrix();
	bag->rocket_texture.bind();
	bag->rocket_smesh.draw();
}

