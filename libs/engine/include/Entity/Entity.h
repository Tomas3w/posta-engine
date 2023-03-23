#ifndef POSTAENGINE_ENTITY_H
#define POSTAENGINE_ENTITY_H

namespace Engine::Entity {
	/** Base class for every entity */
	class Entity
	{
		public:
			Entity() = default;
			virtual ~Entity() = default;
			/** Loop function, runs every frame */
			virtual void loop() = 0;
			/** Runs every frame and renders the entity */
			virtual void draw() = 0;
	};
}

#endif // POSTAENGINE_ENTITY_H
