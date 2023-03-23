#ifndef POSTAENGINE_PLANERIGIDBODY_H
#define POSTAENGINE_PLANERIGIDBODY_H
#include "engine/include/Component/Rigidbody.h"

namespace Engine::Component {
	class PlaneRigidbody : public Rigidbody
	{
		public:
			PlaneRigidbody(glm::vec3 position, glm::vec3 normal);
	};
}

#endif // POSTAENGINE_PLANERIGIDBODY_H
