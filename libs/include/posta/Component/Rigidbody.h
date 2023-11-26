#ifndef POSTAENGINE_RIGIDBODY_COMPONENT_H
#define POSTAENGINE_RIGIDBODY_COMPONENT_H
#include "posta/Component/Constraint.h"
#include <posta/Component/Transform.h>
#include <posta/Util/General.h>
#include <iostream>

namespace posta::component {
	class Rigidbody
	{
		protected:
			Rigidbody() = default;
		public:
			Rigidbody(const Rigidbody&) = delete;
			virtual ~Rigidbody();
			Rigidbody& operator=(const Rigidbody&) = delete;

			/// Returns the transforms matrix
			virtual glm::mat4 get_matrix() final;
			/// Returns a new transform object of the internal rigidbody
			virtual posta::component::Transform get_transform() final;
			/// Set transform, ignores scale of transform
			virtual void set_transform(posta::component::Transform transform);
			
			/// Returns the internal rigidbody pointer
			virtual btRigidBody* get_body() final;
			/// Sets the internal rigidbody pointer
			virtual void set_body(btRigidBody* body) final;

		private:
			std::vector<Constraint*> constraints; // all the constraints associated with this rigidbody
			btRigidBody* body = nullptr;

			void add_constraint(Constraint* constraint);
			void remove_constraint(Constraint* constraint);

			friend class Constraint;
	};
}

#endif // POSTAENGINE_RIGIDBODY_COMPONENT_H
