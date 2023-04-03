#ifndef PLANET_H
#define PLANET_H

#include "engine/include/Component/MeshStaticRigidbody.h"
#include "engine/include/Component/StaticMesh.h"
#include "engine/include/Util/Mesh.h"
#include "engine/include/Util/ResourceBag.h"
#include <engine/include/App.h>

class Planet
{
	public:
		class Resources : public Engine::ResourceBag
		{
		public:
			Resources();

			Engine::Mesh planet_mesh;
			Engine::Component::StaticMesh planet_smesh;
			//Engine::Component::Texture planet_texture;
		};
		static Resources* bag;

		Planet(glm::vec3 pos, float _radius);

		void draw();

		float radius;
		Engine::Component::MeshStaticRigidbody rb;
};

#endif // PLANET_H
