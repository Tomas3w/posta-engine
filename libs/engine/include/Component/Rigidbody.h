#ifndef POSTAENGINE_RIGIDBODY_COMPONENT_H
#define POSTAENGINE_RIGIDBODY_COMPONENT_H
#include "engine/include/App.h"
#include "engine/include/Component/Transform.h"
#include "engine/include/Util/General.h"
#include <iostream>

namespace Engine::Component {
	class Rigidbody
	{
		public:
			Rigidbody() = default;
			Rigidbody(const Rigidbody&) = delete;
			Rigidbody& operator=(const Rigidbody&) = delete;
			virtual ~Rigidbody() = 0;

			/// Returns the transforms matrix
			virtual glm::mat4 get_matrix() final;
			/// Returns a new transform object of the internal rigidbody
			virtual Engine::Component::Transform get_transform() final;
			/// Set transform, ignores scale of transform
			virtual void set_transform(Engine::Component::Transform transform);
			
			/// Returns the internal rigidbody pointer
			virtual btRigidBody* get_body() final;
			/// Sets the internal rigidbody pointer
			virtual void set_body(btRigidBody* body) final;
		private:
			btRigidBody* body;
	};
}

#endif // POSTAENGINE_RIGIDBODY_COMPONENT_H
