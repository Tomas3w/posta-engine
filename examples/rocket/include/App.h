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
#include "engine/include/Util/Framebuffer.h"
#include <fstream>
#include <memory>
#include "DepthShader.h"
#include "ShadowShader.h"
#include "Rocket.h"

using namespace std;

class App : public Engine::App
{
	public:
		Engine::Component::Texture test_texture;
		Engine::Component::Texture blank_texture;
		Engine::Component::StaticMesh cube_smesh;
		Engine::Component::Texture cube_texture;
		Engine::Component::StaticMesh plane_smesh;
		Engine::DepthFramebuffer framebuffer;
		Engine::Entity::Camera light_camera;
		float t;

		struct Box
		{
			Box(Engine::Component::BoxRigidbody* _rb) : rb(_rb), time(1) {}
			~Box() {delete rb;}
			Engine::Component::BoxRigidbody* rb;
			float time;
		};
		std::vector<std::unique_ptr<Box>> box_rb;
		std::vector<std::unique_ptr<Rocket>> rockets;

		ShadowShader shadow_shader;
		DepthShader depth_shader;

		App();
		~App();
		void on_frame();
		void on_draw();
		void on_draw_2d();
		void on_event(SDL_Event& event);
};

extern std::unique_ptr<App> app;

