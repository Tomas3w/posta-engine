#include "Meat.h"

std::vector<std::unique_ptr<Engine::Component::StaticMesh>> Meat::variants = std::vector<std::unique_ptr<Engine::Component::StaticMesh>>();
std::unique_ptr<Engine::Component::Texture> Meat::texture = std::unique_ptr<Engine::Component::Texture>();

Meat::Meat(glm::vec3 position, glm::vec3 init_vel) :
	body(position, 0.001f, 0.5f)
{
	// loading models and texture
	if (variants.empty())
	{
		for (auto const& dir_entry : std::filesystem::directory_iterator{"assets/meats"}) 
		{
			auto path = dir_entry.path();
			if (path.has_filename() && path.extension() == ".obj")
			{
				//std::cout << "loading " << path << std::endl;
				variants.push_back(std::make_unique<Engine::Component::StaticMesh>(Engine::Assets::load_obj(path)));
			}
		}
	}
	variant = rand() % variants.size();
	if (!texture)
		texture.reset(new Engine::Component::Texture("assets/meats/meat.png"));
	// setting lifetime
	life = 100.0f;

	// setting initial velocity
	glm::vec3 vel;
	vel.x = (rand() % 101 - 50) / 50.0f;
	vel.y = (rand() % 101 - 50) / 50.0f;
	vel.z = (rand() % 101 - 50) / 50.0f;
	if (vel.x == 0 && vel.y == 0 && vel.z == 0)
		vel.x = 1;
	vel = glm::normalize(vel) * (rand() % 100 / 10.0f);
	body.get_body()->setLinearVelocity(Engine::to_btVector3(vel + init_vel));
}

void Meat::loop()
{
	life -= Engine::App::app->delta_time * 10.0f;
	//if (life < 90.0f)
	//	body.get_body()->setMassProps(0.0f, Engine::to_btVector3(glm::vec3(0, 0, 0)));
}

void Meat::draw(Engine::ThreeDShader& shader)
{
	shader.global_color = {1, 1, 1, life / 50.0f};
	texture->bind();
	glm::mat4 matrix = body.get_matrix();
	shader.model = matrix;
	shader.normal_model = glm::mat3(glm::transpose(glm::inverse(matrix)));
	variants[variant]->draw();
	shader.global_color = {1, 1, 1, 1};
}

bool Meat::is_alive() const
{
	return life > 0;
}

