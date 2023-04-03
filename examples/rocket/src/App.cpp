#include "engine/include/Component/BoxRigidbody.h"
#include "engine/include/Component/Camera.h"
#include "engine/include/Component/Transform.h"
#include "engine/include/Entity/Camera.h"
#include <App.h>
#include <SDL_timer.h>
#include <glm/gtx/quaternion.hpp>
#include "engine/include/Util/General.h"
#include "engine/include/Util/ResourceBag.h"

std::unique_ptr<App> app;

using namespace std;

App::App() :
	test_texture("common/image.png"),
	blank_texture("common/blank.png"),
	cube_smesh(Engine::Assets::load_obj("common/cube.obj")),
	cube_texture("common/cube_texture.png"),
	plane_smesh(Engine::Assets::load_obj("assets/plane.obj")),
	framebuffer(4096, 4096),
	//light_camera(new Engine::Component::PCamera(0.1f, 20.0f, M_PI / 4.0f), 640, 480)
	light_camera(new Engine::Component::OCamera(0.0f, 200.0f, 100), framebuffer.w, framebuffer.h)
{
	LOG("Init app");
	// Setting default value for use_bones
	shader3d->use_bones = false;

	//dynamic_cast<Engine::Component::PCamera*>(camera->camera.get())->__near = 0.1f;
	camera->camera->__far = 1000.0f;
	camera->update_projection_matrix();
	// Moving camera a bit
	//camera->transform.set_position({0, 5, 5});
	//camera->transform.set_position({0, 5, -5});
	camera->transform.set_position({5, 5, 0});
	camera->transform.set_rotation(glm::quatLookAt(glm::normalize(-camera->transform.get_position()), glm::vec3(0, 1, 0)));
	//camera->transform.set_rotation(glm::rotate(camera->transform.get_rotation(), -static_cast<float>(M_PI / 4), {1, 0, 0}));

	// Moving light_camera
	//light_camera.transform.set_position({0, 5, 5});
	//light_camera.transform.set_rotation(glm::rotate(light_camera.transform.get_rotation(), -static_cast<float>(M_PI / 4), {1, 0, 0}));
	
	light_camera.transform.set_position({0, 30, -30});
	light_camera.transform.set_rotation(glm::quatLookAt(glm::normalize(-light_camera.transform.get_position()), glm::vec3(0, 1, 0)));
	//
	//light_camera.transform.set_rotation(glm::rotate(light_camera.transform.get_rotation(), static_cast<float>(M_PI), {0, 1, 0}));
	//light_camera.transform.set_rotation(glm::rotate(light_camera.transform.get_rotation(), -static_cast<float>(M_PI / 4), {1, 0, 0}));
	
	//light_camera.transform.set_position({0, 5, 0});
	//light_camera.transform.set_rotation(glm::rotate(light_camera.transform.get_rotation(), static_cast<float>(M_PI / 2.0f), {1, 0, 0}));
	// Setting active textures up
	shadow_shader.intexture = 0;
	shadow_shader.depthtexture = 1;

	shadow_shader.use_bones = false;

	// Setting t to 0
	t = 2;

	// physics related
	physics->world->setGravity(Engine::to_btVector3({0, 0, 0}));

	// Setting classes resource_bags
	Rocket::bag = dynamic_cast<Rocket::Resources*>(resource_bag->add_bag(new Rocket::Resources));
}

App::~App()
{
	LOG("Dest app");
}

void App::on_frame()
{
	// dia noche
	//light_camera.transform.set_position(glm::vec3(sin(SDL_GetTicks() / 1000.0f) * 30, 30, cos(SDL_GetTicks() / 1000.0f) * 30));
	//LOG(-light_camera.transform.front());
	//
	glm::vec3 light_center(0);
	if (!rockets.empty())
	{
		camera->transform.set_position(glm::mix(rockets[0]->rb.get_transform().get_position() + glm::vec3(15, 15, 0), camera->transform.get_position(), 0.7f));
		light_center = rockets[0]->rb.get_transform().get_position();
		//camera->transform.set_rotation(glm::quatLookAt(glm::normalize(rockets[0]->rb.get_transform().get_position() - camera->transform.get_position()), glm::vec3(0, 1, 0)));
	}
	light_camera.transform.set_position(light_center + glm::vec3(20, sin(t) * 100, cos(t) * 100));
	light_camera.transform.set_rotation(glm::quatLookAt(glm::normalize(-light_camera.transform.get_position()), glm::vec3(0, 1, 0)));
	light_camera.update_projection_matrix();
	if (input["/"])
	{
		float potencia = 40.0f;
		for (auto& rocket : rockets)
		{
			rocket->rb.get_body()->setActivationState(ACTIVE_TAG);
			rocket->rb.get_body()->applyCentralForce(Engine::to_btVector3(rocket->rb.get_transform().up() * potencia));
		}
	}
	float potencia_de_giro = 8;
	if (input["up"])
	{
		for (auto& rocket : rockets)
		{
			rocket->rb.get_body()->setActivationState(ACTIVE_TAG);
			rocket->rb.get_body()->applyTorque(Engine::to_btVector3(camera->transform.up() * potencia_de_giro));
		}
	}
	if (input["down"])
	{
		for (auto& rocket : rockets)
		{
			rocket->rb.get_body()->setActivationState(ACTIVE_TAG);
			rocket->rb.get_body()->applyTorque(Engine::to_btVector3(-camera->transform.up() * potencia_de_giro));
		}
	}
	if (input["right"])
	{
		for (auto& rocket : rockets)
		{
			rocket->rb.get_body()->setActivationState(ACTIVE_TAG);
			rocket->rb.get_body()->applyTorque(Engine::to_btVector3(-camera->transform.front() * potencia_de_giro));
		}
	}
	if (input["left"])
	{
		for (auto& rocket : rockets)
		{
			rocket->rb.get_body()->setActivationState(ACTIVE_TAG);
			rocket->rb.get_body()->applyTorque(Engine::to_btVector3(camera->transform.front() * potencia_de_giro));
		}
	}
	/*if (input["r"])
		t += delta_time;
	if (input["l"])
		t -= delta_time;*/
	float vel = 1;
	if (input["w"])
		camera->transform.set_position(camera->transform.get_position() - camera->transform.front() * vel);
	if (input["s"])
		camera->transform.set_position(camera->transform.get_position() + camera->transform.front() * vel);
	if (input["a"])
		camera->transform.set_position(camera->transform.get_position() - camera->transform.right() * vel);
	if (input["d"])
		camera->transform.set_position(camera->transform.get_position() + camera->transform.right() * vel);
	if (is_click_pressed())
	{
		/*Engine::Component::PCamera* cam = dynamic_cast<Engine::Component::PCamera*>(camera->camera.get());
		float fov = cam->__fov;
		float __near = cam->__near;
		float ratio = get_width() / (float)get_height();
		float screen_h = tan(fov / 2) * __near * 2;//radius * __near;
		float screen_w = ratio * screen_h;
		glm::vec3 mouse_point((mouse_x / (float)get_width() - 0.5f) * screen_w, (mouse_y / (float)get_height() - 0.5f) * screen_h, -__near);
		glm::vec3 mouse_dir = camera->transform.to_local(glm::normalize(mouse_point));
		float factor = abs((plane_rb.get_transform().get_position().y + 1 - camera->transform.get_position().y) / mouse_dir.y);
		box_rb.emplace_back(new Box(new Engine::Component::BoxRigidbody(camera->transform.get_position() + mouse_dir * factor, 1, glm::vec3(1.0f))));*/
	}
	for (auto it = box_rb.begin(); it != box_rb.end(); it++)
	{
		(*it)->time -= delta_time;
		if ((*it)->time < 0)
			it = box_rb.erase(it) - 1;
	}
}

void App::on_draw()
{
	// Binds default 3D shader, uses the projection view matrix from camera and sets the global_color uniform to a white color
	shadow_shader.bind();
	shadow_shader.global_color = {1, 1, 1, 1};

	//
	for (int i = 0; i < 2; i++)
	{
		if (i == 1)
		{
			shadow_shader.projection_view = camera->get_projection_view_matrix();
			glCullFace(GL_BACK);
		}
		if (i == 0)
		{
			shadow_shader.projection_view = light_camera.get_projection_view_matrix();
			framebuffer.bind();
			framebuffer.clear(); // THIS IS VERY IMPORTANT
			glCullFace(GL_FRONT);
		}

		// Draw rotating cube
		shadow_shader.intexture = 0;
		shadow_shader.depthtexture = 1;
		shadow_shader.sun_light_projection_view = light_camera.get_projection_view_matrix();
		shadow_shader.sun_light_position = light_camera.transform.get_position();
		shadow_shader.sun_light_direction = -light_camera.transform.front();//glm::vec3(0, 0, -1) * light_camera.transform.get_rotation();
		shadow_shader.sun_depth_distance_ratio = light_camera.camera->__far - light_camera.camera->__near;

		/*Engine::Component::Transform transform;
		transform.set_rotation(glm::rotate(transform.get_rotation(), static_cast<float>(-M_PI * t), glm::vec3(0, 1, 0)));
		shadow_shader.model = transform.get_matrix();
		shadow_shader.normal_model = transform.get_normal_matrix();
		framebuffer.texture->bind(1);
		cube_texture.bind(0);
		cube_smesh.draw();

		Engine::Component::Transform tr;
		tr.set_scale({0.25, 0.25, 0.25});
		tr.set_position({1.25, 0, 0});
		tr = tr.as_local_to(transform);
		shadow_shader.model = tr.get_matrix();
		shadow_shader.normal_model = tr.get_normal_matrix();
		framebuffer.texture->bind(1);
		cube_texture.bind(0);
		cube_smesh.draw();*/

		framebuffer.texture->bind(1);
		for (auto& box : box_rb)
		{
			Engine::Component::Transform transform = box->rb->get_transform();
			transform.set_scale(glm::vec3(0.25f));
			transform.set_rotation(glm::rotate(transform.get_rotation(), static_cast<float>(-M_PI * t), glm::vec3(0, 1, 0)));
			shadow_shader.model = transform.get_matrix();
			shadow_shader.normal_model = transform.get_normal_matrix();
			cube_texture.bind(0);
			cube_smesh.draw();
		}

		for (auto& rocket : rockets)
			rocket->draw();

		// Draw plane
		/*Engine::Component::Transform _tr(plane_rb.get_transform().get_position());
		shadow_shader.model = _tr.get_matrix();
		shadow_shader.normal_model = _tr.get_normal_matrix();
		blank_texture.bind();
		plane_smesh.draw();*/

		if (i == 0)
			framebuffer.unbind_framebuffer();
	}
}

void App::on_draw_2d()
{
	depth_shader.bind();
	depth_shader.near_plane = light_camera.camera->__near;
	depth_shader.far_plane = light_camera.camera->__far;

	float ratio = framebuffer.w / static_cast<float>(framebuffer.h);
	int w = 320;
	int h = ratio * w;
	depth_shader.model = Engine::Component::Image::matrix_for_rect(Engine::UI::Rect(0, 0, w, h), get_width(), get_height());
	depth_shader.global_color = {1, 1, 1, 1};
	Engine::Component::Image::draw_rect(framebuffer.texture.get());
	//depth_shader.model = Engine::Component::Image::matrix_for_rect(Engine::UI::Rect(0, 0, 640 / 4, 480 / 4), get_width(), get_height());
	//Engine::Component::Image::draw_rect(&test_texture);
}

void App::on_event(SDL_Event& event)
{
	//
	//
	switch (event.type)
	{
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
				case SDLK_f:
					LOG("delta_time: ", delta_time);
					break;
				case SDLK_q:
					box_rb.emplace_back(new Box(new Engine::Component::BoxRigidbody({0, 4, 0}, 1, glm::vec3(1.0f))));
					box_rb.emplace_back(new Box(new Engine::Component::BoxRigidbody({0, 4.1f, 0}, 1, glm::vec3(1.0f))));
					break;
				case SDLK_SLASH:
					input["/"] = 1;
					break;
				case SDLK_UP:
					input["up"] = 1;
					break;
				case SDLK_DOWN:
					input["down"] = 1;
					break;
				case SDLK_RIGHT:
					input["right"] = 1;
					break;
				case SDLK_LEFT:
					input["left"] = 1;
					break;
				case SDLK_w:
					input["w"] = 1;
					break;
				case SDLK_s:
					input["s"] = 1;
					break;
				case SDLK_a:
					input["a"] = 1;
					break;
				case SDLK_d:
					input["d"] = 1;
					break;
				case SDLK_r:
					{
						/*Engine::Component::PCamera* cam = dynamic_cast<Engine::Component::PCamera*>(camera->camera.get());
						float fov = cam->__fov;
						float __near = cam->__near;
						float ratio = get_width() / (float)get_height();
						float screen_h = tan(fov / 2) * __near * 2;//radius * __near;
						float screen_w = ratio * screen_h;
						glm::vec3 mouse_point((mouse_x / (float)get_width() - 0.5f) * screen_w, (mouse_y / (float)get_height() - 0.5f) * screen_h, -__near);
						glm::vec3 mouse_dir = camera->transform.to_local(glm::normalize(mouse_point));
						float factor = abs((plane_rb.get_transform().get_position().y + 5 - camera->transform.get_position().y) / mouse_dir.y);
						rockets.emplace_back(new Rocket(camera->transform.get_position() + mouse_dir * factor));*/
						rockets.emplace_back(new Rocket({0, 0, 0}));
					}
					break;
			}
			break;
		case SDL_KEYUP:
			switch (event.key.keysym.sym)
			{
				case SDLK_SLASH:
					input["/"] = 0;
					break;
				case SDLK_UP:
					input["up"] = 0;
					break;
				case SDLK_DOWN:
					input["down"] = 0;
					break;
				case SDLK_RIGHT:
					input["right"] = 0;
					break;
				case SDLK_LEFT:
					input["left"] = 0;
					break;
				case SDLK_w:
					input["w"] = 0;
					break;
				case SDLK_s:
					input["s"] = 0;
					break;
				case SDLK_a:
					input["a"] = 0;
					break;
				case SDLK_d:
					input["d"] = 0;
					break;
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
			if (event.button.button == SDL_BUTTON_LEFT)
			{
			}
			break;
	}
}


