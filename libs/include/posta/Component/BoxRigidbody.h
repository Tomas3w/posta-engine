#ifndef POSTAENGINE_BOXRIGIDBODY_H
#define POSTAENGINE_BOXRIGIDBODY_H
#include <posta/Component/Rigidbody.h>

namespace posta::component {
	class BoxRigidbody : public Rigidbody
	{
		public:
			BoxRigidbody(glm::vec3 position, float mass, glm::vec3 ext, glm::vec3 velocity = glm::vec3(0, 0, 0));
	};
}

#endif // POSTAENGINE_BOXRIGIDBODY_H
