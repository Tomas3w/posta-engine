#include <posta/Component/Rigidbody.h>
#include <posta/Component/HingeConstraint.h>

using HingeConstraint = posta::component::HingeConstraint;
using Rigidbody = posta::component::Rigidbody;

HingeConstraint::HingeConstraint(Rigidbody& a, Rigidbody& b, glm::vec3 pivot_a, glm::vec3 pivot_b, glm::vec3 axis_a, glm::vec3 axis_b) :
	Constraint(new btHingeConstraint(*a.get_body(), *b.get_body(), posta::to_btVector3(pivot_a), posta::to_btVector3(pivot_b), posta::to_btVector3(axis_a), posta::to_btVector3(axis_b)))
{}

