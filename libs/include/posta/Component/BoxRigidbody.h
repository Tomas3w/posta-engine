#ifndef POSTAENGINE_BOXRIGIDBODY_H
#define POSTAENGINE_BOXRIGIDBODY_H
#include "engine/include/Component/Rigidbody.h"

namespace Engine::Component {
	class BoxRigidbody : public Rigidbody
	{
		public:
			BoxRigidbody(glm::vec3 position, float mass, glm::vec3 ext, glm::vec3 velocity = glm::vec3(0, 0, 0));
	};
}

#endif // POSTAENGINE_BOXRIGIDBODY_H
