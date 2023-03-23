#include <engine/include/App.h>
#include <engine/include/Util/General.h>
#include <engine/include/Util/Assets.h>
#include <engine/include/Component/StaticMesh.h>
#include <engine/include/Util/Shader.h>
#include <engine/include/Component/Transform.h>
#include <engine/include/Entity/Camera.h>
#include <engine/include/Component/Texture.h>
#include "engine/include/Component/SphereRigidbody.h"
#include "engine/include/Component/PlaneRigidbody.h"
#include "engine/include/Component/BoxRigidbody.h"
#include "engine/include/Component/CapsuleRigidbody.h"
#include "engine/include/Component/MeshStaticRigidbody.h"
#include "engine/include/Component/Skeleton.h"
#include "engine/include/Util/LoggingMacro.h"
#include <fstream>
#include <memory>

using namespace std;

class App : public Engine::App
{
	public:
		Engine::Component::Texture blank_texture;
		Engine::Component::StaticMesh cube_smesh;
		float t;

		App() :
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

		~App()
		{
			LOG("Dest app");
		}

		void on_frame() override
		{
			t += delta_time;
		}

		void on_draw() override
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

		void on_draw_2d() override
		{
			// A 2D shader is bind automatically here
			// Draw 2D stuff here ...
		}

		void on_event(SDL_Event& event) override
		{
			//
		}

};

std::unique_ptr<App> app = nullptr;

///
int main(int argc, char* argv[])
{
	app.reset(new App());
	app->loop();
	return EXIT_SUCCESS;
}

int _main(int argc, char* argv[])
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		cout << "error: " << SDL_GetError() << endl;
		exit(EXIT_FAILURE);
	}
	SDL_Window* window = SDL_CreateWindow("H", 100, 100, 640, 480, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);

	//initpart
	SDL_GLContext context = SDL_GL_CreateContext(window);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	glewExperimental = GL_TRUE;
	glewInit();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);

	//glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Enable alpha blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Enable face culling
	//glEnable(GL_CULL_FACE);

	glViewport(0, 0, 640, 480);

	glClearColor(0, 0, 0, 1);

	/// meshpart
	float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f
	};
	GLuint vao, vbo;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	///

	Engine::Component::StaticMesh static_mesh(Engine::Assets::load_obj("assets/cube.obj"));

	Engine::Shader shader("assets/shaders/sbasic");
	shader.bind();
	GLint model_uniform = glGetUniformLocation(shader.get_program(), "model");
	cout << "model_uniform = " << model_uniform << endl;
	GLint projection_view_uniform = glGetUniformLocation(shader.get_program(), "projection_view");
	cout << "projection_view_uniform = " << projection_view_uniform << ", also... " << shader.get_program() << endl;
	
	SDL_Event event;
	bool salir = false;
	while (!salir)
	{
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		//glBindVertexArray(vao);
		//glDrawArrays(GL_TRIANGLES, 0, 3);

		//shader.bind();
		static_mesh.draw();
		cout << "here" << endl;
		while (SDL_PollEvent(&event))
		{
			cout << "past here" << endl;
			switch (event.type)
			{
				case SDL_QUIT:
					salir = true;
					break;
			}
		}
		SDL_GL_SwapWindow(window);
	}

	SDL_DestroyWindow(window);
	SDL_GL_DeleteContext(context);
	SDL_Quit();
	return EXIT_SUCCESS;
}

