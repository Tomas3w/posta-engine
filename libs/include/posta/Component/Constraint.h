#ifndef POSTAENGINE_CONSTRAINT_COMPONENT_H
#define POSTAENGINE_CONSTRAINT_COMPONENT_H
#include <posta/Component/Transform.h>
#include <posta/Util/General.h>
#include <iostream>

namespace posta::component {
	class Rigidbody;

	/// A physics constraint between two rigidbody, beware that constraints can be invalid once one of its rigidbody is deleted
	class Constraint
	{
		public:
			Constraint(btTypedConstraint* _constraint);
			Constraint(const Constraint&) = delete;
			Constraint& operator=(const Constraint&) = delete;
			virtual ~Constraint() = 0;

		protected:
			/// This function adds a rigidbody to the internal collection of rigidbody, so that it can be deleted safely down the line
			void add_rigidbody(Rigidbody* rigidbody);
			/// Makes this constraint invalid, its constraint pointer is deleted, removed from the world and set to nullptr
			/// All the rigidbodies associated with it are notified so that they don't have any this constraint associated with them anymore
			void self_destruct();

			btTypedConstraint* constraint = nullptr;
		private:
			/// This function removes all rigidbodies and invalidates the constraint
			void remove_rigidbodies();

			std::vector<Rigidbody*> rigidbodies; // all the rigidbodies associated with this constraint

			friend class Rigidbody;
	};
}

#endif // POSTAENGINE_CONSTRAINT_COMPONENT_H
