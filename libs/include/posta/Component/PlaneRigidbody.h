#ifndef POSTAENGINE_PLANERIGIDBODY_H
#define POSTAENGINE_PLANERIGIDBODY_H
#include <posta/Component/Rigidbody.h>

namespace posta::component {
	class PlaneRigidbody : public Rigidbody
	{
		public:
			PlaneRigidbody(glm::vec3 position, glm::vec3 normal);
	};
}

#endif // POSTAENGINE_PLANERIGIDBODY_H
