#ifndef POSTAENGINE_TEXTURE_COMPONENT_H
#define POSTAENGINE_TEXTURE_COMPONENT_H
#include <filesystem>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

namespace posta {
	class DepthFramebuffer;
	class ColorFramebuffer;
	class ColorDepthFramebuffer;
	class FloatColorFramebuffer;
	class Font;
}

namespace posta::component {
	class Image;

	/// Class that stores a texture, the texture cannot be modified after creation
	class Texture
	{
		public:
			/// Constructs a texture from a file
			Texture(std::filesystem::path path);
			Texture(const Texture&) = delete;
			Texture operator=(const Texture&) = delete;
			~Texture();

			void swap(Texture& o);

			/// Binds the texture
			void bind(GLuint texture_unit = 0) const;

			friend class posta::DepthFramebuffer;
			friend class posta::ColorFramebuffer;
			friend class posta::ColorDepthFramebuffer;
			friend class posta::FloatColorFramebuffer;
			friend class posta::Font;
			friend class Image;

			/// SDL-Specific: Constructs a texture from a SDL_Surface*, it doesn't free the surface
			Texture(SDL_Surface* image);
		private:
			/// OpenGL-specific: Constructs a texture from a texture-id
			Texture(GLuint _texture);

			void load(SDL_Surface* image);
			GLuint texture;
	};
}

#endif // POSTAENGINE_TEXTURE_COMPONENT_H
