#include "engine/include/Component/BoxRigidbody.h"
#include "engine/include/Component/Camera.h"
#include "engine/include/Component/Transform.h"
#include "engine/include/Entity/Camera.h"
#include <App.h>
#include <glm/gtx/quaternion.hpp>

std::unique_ptr<App> app;

using namespace std;

App::App() :
	test_texture("common/image.png"),
	blank_texture("common/blank.png"),
	cube_smesh(Engine::Assets::load_obj("common/cube.obj")),
	//cube_smesh(Engine::Assets::load_obj("assets/tifa.obj")),
	cube_texture("common/cube_texture.png"),
	//cube_texture("assets/tifa.png"),
	plane_smesh(Engine::Assets::load_obj("assets/plane.obj")),
	framebuffer(1024, 1024),
	//light_camera(new Engine::Component::PCamera(0.1f, 20.0f, M_PI / 4.0f), 640, 480)
	light_camera(new Engine::Component::OCamera(0.0f, 100.0f, 50), framebuffer.w, framebuffer.h),
	plane_rb({0, -1.1f, 0}, {0, 1, 0})
{
	LOG("Init app");
	// Setting default value for use_bones
	shader3d->use_bones = false;

	//dynamic_cast<Engine::Component::PCamera*>(camera->camera.get())->__near = 0.1f;
	dynamic_cast<Engine::Component::PCamera*>(camera->camera.get())->__far = 1000.0f;
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

	// Setting t to 0
	t = 0;
}

App::~App()
{
	LOG("Dest app");
}

void App::on_frame()
{
	light_camera.update_projection_matrix();
	if (input["r"])
		t += delta_time;
	if (input["l"])
		t -= delta_time;
	float vel = 1;
	if (input["w"])
		camera->transform.set_position(camera->transform.get_position() - camera->transform.front() * vel);
	if (input["s"])
		camera->transform.set_position(camera->transform.get_position() + camera->transform.front() * vel);
	if (input["a"])
		camera->transform.set_position(camera->transform.get_position() - camera->transform.right() * vel);
	if (input["d"])
		camera->transform.set_position(camera->transform.get_position() + camera->transform.right() * vel);
	//light_camera.transform.set_rotation(glm::rotate(light_camera.transform.get_rotation(), static_cast<float>(M_PI / 2.0f * delta_time), {1, 0, 0}));
	
	//light_camera.transform.set_rotation(glm::rotate(light_camera.transform.get_rotation(), static_cast<float>(M_PI / 2 * delta_time), {0, 1, 0}));
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
		shadow_shader.sun_light_direction = glm::vec3(0, 0, -1) * light_camera.transform.get_rotation();
		shadow_shader.sun_depth_distance_ratio = dynamic_cast<Engine::Component::CameraNearFar*>(light_camera.camera.get())->__far - dynamic_cast<Engine::Component::CameraNearFar*>(light_camera.camera.get())->__near;

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

		for (auto& box : box_rb)
		{
			Engine::Component::Transform transform = box->get_transform();
			transform.set_rotation(glm::rotate(transform.get_rotation(), static_cast<float>(-M_PI * t), glm::vec3(0, 1, 0)));
			shadow_shader.model = transform.get_matrix();
			shadow_shader.normal_model = transform.get_normal_matrix();
			framebuffer.texture->bind(1);
			cube_texture.bind(0);
			cube_smesh.draw();
		}

		// Draw plane
		Engine::Component::Transform _tr(plane_rb.get_transform().get_position());
		shadow_shader.model = _tr.get_matrix();
		shadow_shader.normal_model = _tr.get_normal_matrix();
		blank_texture.bind();
		plane_smesh.draw();

		if (i == 0)
			framebuffer.unbind_framebuffer();
	}
}

void App::on_draw_2d()
{
	depth_shader.bind();
	depth_shader.near_plane = dynamic_cast<Engine::Component::CameraNearFar*>(light_camera.camera.get())->__near;
	depth_shader.far_plane = dynamic_cast<Engine::Component::CameraNearFar*>(light_camera.camera.get())->__far;

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
					box_rb.emplace_back(new Engine::Component::BoxRigidbody({0, 4, 0}, 1, glm::vec3(1.0f)));
					box_rb.emplace_back(new Engine::Component::BoxRigidbody({0, 4.1f, 0}, 1, glm::vec3(1.0f)));
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
				case SDLK_RIGHT:
					input["r"] = 1;
					break;
				case SDLK_LEFT:
					input["l"] = 1;
					break;
			}
			break;
		case SDL_KEYUP:
			switch (event.key.keysym.sym)
			{
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
				case SDLK_RIGHT:
					input["r"] = 0;
					break;
				case SDLK_LEFT:
					input["l"] = 0;
					break;
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
			if (event.button.button == SDL_BUTTON_LEFT)
			{
				//LOG("hola!");
				//glm::vec3 camera_front = camera->transform.front();
				//auto camera_center_pixel = camera_front * dynamic_cast<Engine::Component::CameraNearFar*>(camera->camera.get())->__near;
				Engine::Component::PCamera* cam = dynamic_cast<Engine::Component::PCamera*>(camera->camera.get());
				float fov = cam->__fov;
				float __near = cam->__near;
				float radius = sin(fov);
				float ratio = get_width() / (float)get_height();
				float screen_h = radius * __near;
				float screen_w = ratio * screen_h;
				glm::vec3 mouse_point((mouse_x / (float)get_width() - 0.5f) * screen_w, (mouse_y / (float)get_height() - 0.5f) * screen_h, -__near);
				glm::vec3 mouse_dir = camera->transform.to_local(glm::normalize(mouse_point));
				LOG(to_string(mouse_dir));
				float factor = abs((plane_rb.get_transform().get_position().y + 1 - camera->transform.get_position().y) / mouse_dir.y);
				box_rb.emplace_back(new Engine::Component::BoxRigidbody(camera->transform.get_position() + mouse_dir * factor, 1, glm::vec3(1.0f)));
				
				//glm::vec4 screen_space(mouse_x, mouse_y, -1, 1);
				/*
				glm::vec4 clip_space(mouse_x / (float)get_width() * 2 - 1, mouse_y / (float)get_height() * 2 - 1, 1, 1);
				glm::vec4 world_space = glm::inverse(camera->get_projection_view_matrix()) * clip_space;
				LOG(glm::to_string(world_space));
				glm::vec3 global_position(world_space.x, world_space.y, world_space.z);
				glm::vec3 dir = glm::normalize(camera->transform.get_position() - global_position);
				box_rb.emplace_back(new Engine::Component::BoxRigidbody(camera->transform.get_position() - dir * 1.0f, 1, glm::vec3(1.0f)));
				*/
			}
			break;
	}
}


