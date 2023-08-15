#ifndef POSTAENGINE_FRAMEBUFFER_H
#define POSTAENGINE_FRAMEBUFFER_H

#include <posta/Util/General.h>
#include <posta/Component/Texture.h>

namespace Engine {
	class Framebuffer
	{
		public:
			/// Unbinds any framebuffer previously binded, the following draw calls will be done on the screen
			static void unbind_framebuffer();
			/// Binds the framebuffer, once binded the following draw calls will be done on this framebuffer and not on the the screen
			/// This only binds the framebuffer, make sure to also call clear() on this framebuffer after binding so that something
			/// is actually being rendered, keep in mind that the resulting texture will be vertically flipped
			virtual void bind();
			/// Clears the framebuffer
			virtual void clear() = 0;

			~Framebuffer();

			int w, h;
			
			/// Texture of the framebuffer, it is guarantee to not be null
			std::unique_ptr<Engine::Component::Texture> texture;

			friend class DepthFramebuffer;
			friend class ColorFramebuffer;
			friend class FloatColorFramebuffer;
		private:
			GLuint fbo;
	};
	
	class DepthFramebuffer : public Framebuffer
	{
		public:
			DepthFramebuffer(int _w, int _h);
			void clear() override;
	};

	class ColorFramebuffer : public Framebuffer
	{
		public:
			ColorFramebuffer(int _w, int _h);
			void clear() override;
	};

	class FloatColorFramebuffer : public Framebuffer
	{
		public:
			FloatColorFramebuffer(int _w, int _h);
			void clear() override;
	};
}

#endif // POSTAENGINE_FRAMEBUFFER_H
