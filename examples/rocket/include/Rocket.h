#ifndef ROCKET_H
#define ROCKET_H

#include "engine/include/Component/BoxRigidbody.h"
#include "engine/include/Component/StaticMesh.h"
#include "engine/include/Util/ResourceBag.h"
#include "engine/include/Util/Assets.h"
class Rocket
{
	public:
		class Resources : public Engine::ResourceBag
		{
			public:
				Resources();

				Engine::Component::StaticMesh rocket_smesh;
				Engine::Component::Texture rocket_texture;
		};
		static Resources* bag;

		Rocket(glm::vec3 pos);

		void draw();

		Engine::Component::BoxRigidbody rb;
};

#endif // ROCKET_H
