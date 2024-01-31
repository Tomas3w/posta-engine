#include <posta/Component/Texture.h>
#include <iostream>

using posta::component::Texture;

Texture::Texture(std::filesystem::path path)
{
	SDL_Surface* image = IMG_Load(path.string().c_str());
	if (image == nullptr)
		throw std::logic_error(std::string("couldn't find texture file: '") + path.string().c_str() + "'");
	load(image);
	SDL_FreeSurface(image);
}

Texture::Texture(SDL_Surface* image)
{
	load(image);
}

Texture::Texture(GLuint _texture)
{
	texture = _texture;
}

void Texture::load(SDL_Surface* image)
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// loading the image to a SDL_Surface with the correct format
	SDL_Surface* surface = SDL_ConvertSurfaceFormat(image, SDL_PIXELFORMAT_RGBA32, 0);

	// generate the texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, reinterpret_cast<Uint32*>(surface->pixels));
	glGenerateMipmap(GL_TEXTURE_2D);

	// deleting pixels
	SDL_FreeSurface(surface);
}

Texture::~Texture()
{
	glDeleteTextures(1, &texture);
}

void Texture::swap(Texture& o)
{
	auto prev_texture = o.texture;
	o.texture = texture;
	texture = prev_texture;
}

void Texture::bind(GLuint texture_unit) const
{
	glActiveTexture(GL_TEXTURE0 + texture_unit);
	glBindTexture(GL_TEXTURE_2D, texture);
}

