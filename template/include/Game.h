#ifndef GAME_H
#define GAME_H
#include <posta/Util/Scene.h>
#include <posta/Component/Texture.h>
#include <posta/Component/StaticMesh.h>
#include <posta/Util/Assets.h>
#include "App.h"

class Game : public posta::Scene
{
	public:
		posta::component::Texture blank_texture;
		posta::component::StaticMesh cube_smesh;
		float t;

		void update() override;
		void event(SDL_Event& event) override;

		Game();
		~Game();
};

#endif // GAME_H
