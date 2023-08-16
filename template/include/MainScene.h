#ifndef MAINSCENE_H
#define MAINSCENE_H
#include <posta/Util/Scene.h>
#include <posta/Component/Texture.h>
#include <posta/Component/StaticMesh.h>
#include <posta/Util/Assets.h>
#include "App.h"

class MainScene : public posta::Scene
{
	public:
		posta::component::Texture blank_texture;
		posta::component::StaticMesh cube_smesh;
		float t;

		void update() override;
		void event(SDL_Event& event) override;

		MainScene();
		~MainScene();
};

#endif // MAINSCENE_H
