#include "Planet.h"
#include "engine/include/Component/StaticMesh.h"
#include "engine/include/Util/Assets.h"
#include "App.h"
#include "engine/include/Util/Mesh.h"

Planet::Resources::Resources() :
	planet_mesh(Engine::Assets::load_obj("assets/plane.obj")),
	planet_smesh(planet_mesh)
{}

Planet::Resources* Planet::bag;

Planet::Planet(glm::vec3 pos, float _radius) :
	rb(pos, bag->planet_mesh)
{
	radius = _radius;
}

void Planet::draw()
{
	app->blank_texture.bind();
	app->shadow_shader.global_color = {1, 1, 1, 1};
	app->shadow_shader.model = rb.get_transform().get_matrix();
	app->shadow_shader.normal_model = rb.get_transform().get_normal_matrix();
	bag->planet_smesh.draw();
}

