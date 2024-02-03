#ifndef IMAGE_H
#define IMAGE_H
#include <engine/include/Util/General.h>
#include <engine/include/Component/Texture.h>
#include <engine/include/Component/StaticMesh.h>
#include <engine/include/Util/Assets.h>
#include <iostream>
#include <memory>
#include <engine/include/App.h>

class Image
{
public:
	static std::unique_ptr<Engine::Component::StaticMesh> mesh2d;
	Image(Engine::Component::Texture* texture, int w, int h);

	glm::mat4 get_matrix(int x, int y);
	void draw();

	std::unique_ptr<Engine::Component::Texture> texture;
	int w, h;
};

#endif // IMAGE_H
