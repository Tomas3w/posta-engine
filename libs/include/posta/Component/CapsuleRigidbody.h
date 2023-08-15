#ifndef POSTAENGINE_CAPSULERIGIDBODY_H
#define POSTAENGINE_CAPSULERIGIDBODY_H
#include <posta/Component/Rigidbody.h>

namespace Engine::Component {
	class CapsuleRigidbody : public Rigidbody
	{
		public:
			/// Constructs a capsule shaped rigidbody
			/** Radius specifis the radius of the two spheres in each
			 * part of the capsule and distance specifies the distance
			 * between those two spheres */
			CapsuleRigidbody(glm::vec3 position, float mass, float radius, float distance, glm::vec3 velocity = glm::vec3(0, 0, 0));
	};
}

#endif // POSTAENGINE_CAPSULERIGIDBODY_H
