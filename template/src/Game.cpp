#include "Game.h"
#include <posta/Util/LoggingMacro.h>

using namespace std;

Game::Game() :
	blank_texture("common/blank.png"),
	cube_smesh(posta::assets::load_obj("common/cube.obj"))
{
	LOG("Init Game");
	// Setting default value for use_bones
	app->shader3d->use_bones = false;

	// Moving camera a bit
	app->camera->transform.set_position({0, 0, 5});

	// Setting t to 0
	t = 0;
}

Game::~Game()
{
	LOG("Dest Game");
}

void Game::update()
{
	// advances time variable...
	t += app->delta_time / 2;

	// Binds default 3D shader, uses the projection view matrix from camera and sets the global_color uniform to a white color
	app->shader3d->bind();
	app->shader3d->projection_view = app->camera->get_projection_view_matrix();
	app->shader3d->global_color = {1, 1, 1, 1};

	// Draw 3D stuff here ...
	posta::component::Transform transform;
	transform.set_rotation(glm::rotate(transform.get_rotation(), static_cast<float>(M_PI * t), glm::vec3(0, 1, 0)));
	app->shader3d->model = transform.get_matrix();
	app->shader3d->normal_model = transform.get_normal_matrix();
	blank_texture.bind();
	cube_smesh.draw();
}

void Game::event(SDL_Event& event)
{
	// event handling
}

