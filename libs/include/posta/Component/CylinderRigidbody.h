#ifndef POSTAENGINE_CYLINDERBODY_H
#define POSTAENGINE_CYLINDERBODY_H
#include <posta/Component/Rigidbody.h>

namespace posta::component {
	class CylinderRigidbody : public Rigidbody
	{
		public:
			enum class AxisAligned
			{
				X,
				Y,
				Z,
			};
			CylinderRigidbody(glm::vec3 position, float mass, glm::vec3 ext, AxisAligned axis, glm::vec3 velocity = glm::vec3(0, 0, 0));
	};
}

#endif // POSTAENGINE_CYLINDERBODY_H
