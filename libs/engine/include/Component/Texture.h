#ifndef POSTAENGINE_TEXTURE_COMPONENT_H
#define POSTAENGINE_TEXTURE_COMPONENT_H
#include <filesystem>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

namespace Engine::Component {
	/// Class that stores a texture, the texture cannot be modified after creation
	class Texture
	{
		public:
			/// Constructs a texture from a file
			Texture(std::filesystem::path path);
			/// Constructs a texture from a SDL_Surface*, it doesn't free the surface
			Texture(SDL_Surface* image);
			Texture(const Texture&) = delete;
			Texture operator=(const Texture&) = delete;
			~Texture();

			/// Binds the texture
			void bind(GLuint texture_unit = 0);

		private:
			void load(SDL_Surface* image);
			GLuint texture;
	};
}

#endif // POSTAENGINE_TEXTURE_COMPONENT_H
