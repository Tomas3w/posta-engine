#ifndef POSTAENGINE_HINGECONSTRAINT_COMPONENT_H
#define POSTAENGINE_HINGECONSTRAINT_COMPONENT_H
#include <posta/Component/Constraint.h>

namespace posta::component {
	class HingeConstraint : public Constraint
	{
		public:
			HingeConstraint(Rigidbody& a, Rigidbody& b, glm::vec3 pivot_a, glm::vec3 pivot_b, glm::vec3 axis_a, glm::vec3 axis_b);
	};
}

#endif // POSTAENGINE_HINGECONSTRAINT_COMPONENT_H
