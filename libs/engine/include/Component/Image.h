#ifndef POSTAENGINE_IMAGE_COMPONENT_H
#define POSTAENGINE_IMAGE_COMPONENT_H
#include <engine/include/Util/General.h>
#include <engine/include/Component/Texture.h>
#include <engine/include/Component/StaticMesh.h>
#include <engine/include/Util/Assets.h>
#include <iostream>
#include <memory>
//#include <engine/include/App.h>

namespace Engine::Component {
	/// Image class, used to draw a 2D image in a given position, it stores a texture and its dimensions
	class Image
	{
	public:
		/// Creates an image from a file
		Image(std::filesystem::path path);
		/// Creates an image from a texture and its dimensions, takes ownership of the texture pointer
		Image(Engine::Component::Texture* texture, int w, int h);

		glm::mat4 get_matrix(int x, int y);
		glm::mat4 get_matrix_with_size(int x, int y, int _w, int _h);
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

		std::unique_ptr<Engine::Component::Texture> texture;
		int w, h;
	};
}

#endif // POSTAENGINE_IMAGE_COMPONENT_H
