#include <App.h>

std::unique_ptr<App> app;

using namespace std;

App::App() :
	blank_texture("common/blank.png"),
	cube_smesh(Engine::Assets::load_obj("common/cube.obj"))
{
	LOG("Init app");
	// Setting default value for use_bones
	shader3d->use_bones = false;

	// Moving camera a bit
	camera->transform.set_position({0, 0, 5});

	// Setting t to 0
	t = 0;
}

App::~App()
{
	LOG("Dest app");
}

void App::on_frame()
{
	t += delta_time;
}

void App::on_draw()
{
	// Binds default 3D shader, uses the projection view matrix from camera and sets the global_color uniform to a white color
	shader3d->bind();
	shader3d->projection_view = camera->get_projection_view_matrix();
	shader3d->global_color = {1, 1, 1, 1};

	// Draw 3D stuff here ...
	Engine::Component::Transform transform;
	transform.set_rotation(glm::rotate(transform.get_rotation(), static_cast<float>(M_PI * t), glm::vec3(0, 1, 0)));
	shader3d->model = transform.get_matrix();
	shader3d->normal_model = transform.get_normal_matrix();
	blank_texture.bind();
	cube_smesh.draw();
}

void App::on_draw_2d()
{
	// A 2D shader is bind automatically here
	// Draw 2D stuff here ...
}

void App::on_event(SDL_Event& event)
{
	//
}


