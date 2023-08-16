#ifndef POSTAENGINE_ENTITY_H
#define POSTAENGINE_ENTITY_H
#include <posta/Component/DrawableMesh.h>
#include <corecrt.h>
#include <posta/Component/Transform.h>
#include <posta/Component/Rigidbody.h>
#define POSTA_REGISTER_ENTITY_A(transform)
#define POSTA_REGISTER_ENTITY_B(transform, drawable_mesh)
#define POSTA_REGISTER_ENTITY_C(rigidbody)
#define POSTA_REGISTER_ENTITY_D(rigidbody, drawable_mesh)

namespace posta::Entity {
	/** Base class for every entity in the editor
	 * Ensure to enclose all code relevant to this parent class with an #ifdef EDITOR_ENABLED preprocessor directive. This can be achieved using the macro function POSTA_REGISTER_ENTITY() */
	class Entity
	{
		private:
			posta::component::Transform* __entity_transform = nullptr;
			posta::component::Rigidbody* __entity_rigidbody = nullptr;
			posta::component::DrawableMesh* __entity_drawable_mesh = nullptr;
			std::string __entity_name;

			// In case the entity doesn't have a rigidbody, then it is created from a mesh object
			std::unique_ptr<posta::component::Rigidbody> __entity_rigidbody_artificial;
		public:
			Entity()
			{
				__entity_name = std::to_string(reinterpret_cast<uintptr_t>(this));
			}
			virtual ~Entity() = default;

	};
}

#endif // POSTAENGINE_ENTITY_H
