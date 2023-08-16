#ifndef POSTAENGINE_IMAGE_COMPONENT_H
#define POSTAENGINE_IMAGE_COMPONENT_H
#include <posta/Util/General.h>
#include <posta/Component/Texture.h>
#include <posta/Component/StaticMesh.h>
#include <posta/Util/Assets.h>
#include <iostream>
#include <memory>
#include <posta/UI/Rect.h>

namespace posta::component {
	/// Image class, used to draw a 2D image in a given position, it stores a texture and its dimensions
	class Image
	{
	public:
		/// Returns the matrix for a rectangle
		static glm::mat4 matrix_for_rect(posta::UI::Rect rect, int screen_w, int screen_h);
		/// Draws a rectangle with texture given by texture
		static void draw_rect(posta::component::Texture* texture);
		/// Creates an image from a file
		Image(std::filesystem::path path);
		/// Creates an image from a texture and its dimensions, takes ownership of the texture pointer
		Image(posta::component::Texture* texture, int w, int h);

		glm::mat4 get_matrix(int x, int y);
		glm::mat4 get_matrix_with_size(int x, int y, int _w, int _h);
		glm::mat4 get_matrix_with_size(posta::UI::Rect rect);
		int get_w();
		int get_h();
		void set_w(int _w);
		void set_h(int _h);
		/// Binds the texture and then draws the image (calling Image::mesh2d.draw())
		void draw();
		/// Only binds the texture, and the 2d mesh, faster if one wishes to draw a lot of images
		void only_bind();
		/// Only draws, the texture and the mesh used are the previously binded
		void only_draw();

		std::unique_ptr<posta::component::Texture> texture;
		int w, h;
	};
}

#endif // POSTAENGINE_IMAGE_COMPONENT_H