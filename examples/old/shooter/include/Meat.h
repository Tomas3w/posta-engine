#ifndef MEAT_H
#define MEAT_H
#include <engine/include/App.h>
#include <engine/include/Component/Texture.h>
#include <engine/include/Component/StaticMesh.h>
#include <engine/include/Component/SphereRigidbody.h>

class Meat
{
public:
	static std::vector<std::unique_ptr<Engine::Component::StaticMesh>> variants;
	static std::unique_ptr<Engine::Component::Texture> texture;
	Meat(glm::vec3 position, glm::vec3 init_vel);

	void loop();
	void draw(Engine::ThreeDShader& shader);
	bool is_alive() const;

	Engine::Component::SphereRigidbody body;
private:
	size_t variant;
	float life;
};

#endif // MEAT_H
