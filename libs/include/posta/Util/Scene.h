#ifndef POSTAENGINE_SCENE_H
#define POSTAENGINE_SCENE_H
#include <SDL2/SDL.h>

namespace Engine {
	class Scene
	{
		public:
			virtual ~Scene() = default;

			virtual void start() {}
			virtual void draw2d() {}
			virtual void update() {}
			virtual void event(SDL_Event& event) {}
	};
}

#endif // POSTAENGINE_SCENE_H
