#ifndef POSTAENGINE_SPHERERIGIDBODY_H
#define POSTAENGINE_SPHERERIGIDBODY_H
#include <posta/Component/Rigidbody.h>

namespace posta::component {
	class SphereRigidbody : public Rigidbody
	{
		public:
			SphereRigidbody(glm::vec3 position, float mass, float radius, glm::vec3 velocity = glm::vec3(0, 0, 0));
	};
}

#endif // POSTAENGINE_SPHERERIGIDBODY_H
