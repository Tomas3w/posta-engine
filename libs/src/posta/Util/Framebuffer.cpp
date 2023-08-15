#include <engine/include/Util/Framebuffer.h>
#include <engine/include/App.h>

using Engine::Framebuffer;
using Engine::DepthFramebuffer;
using Engine::ColorFramebuffer;
using Engine::FloatColorFramebuffer;

void Framebuffer::unbind_framebuffer()
{
	glViewport(0, 0, Engine::App::app->get_width(), Engine::App::app->get_height());
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::bind()
{
	glViewport(0, 0, w, h);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

Framebuffer::~Framebuffer()
{
	glDeleteFramebuffers(1, &fbo);
}

//std::unique_ptr<Engine::Component::Texture> texture;

DepthFramebuffer::DepthFramebuffer(int _w, int _h)
{
	w = _w;
	h = _h;
	// generating fbo
	glGenFramebuffers(1, &fbo);
	// generating texture
	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	texture.reset(new Engine::Component::Texture(depthMap));

	// assigning the texture to the framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DepthFramebuffer::clear()
{
	glClear(GL_DEPTH_BUFFER_BIT);
}

ColorFramebuffer::ColorFramebuffer(int _w, int _h)
{
	w = _w;
	h = _h;
	// generating fbo
	glGenFramebuffers(1, &fbo);
	// generating textureMap
	unsigned int textureMap;
	glGenTextures(1, &textureMap);
	glBindTexture(GL_TEXTURE_2D, textureMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	texture.reset(new Engine::Component::Texture(textureMap));

	// assigning the texture to the framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureMap, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ColorFramebuffer::clear()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

FloatColorFramebuffer::FloatColorFramebuffer(int _w, int _h)
{
	w = _w;
	h = _h;
	// generating fbo
	glGenFramebuffers(1, &fbo);
	// generating textureMap
	unsigned int textureMap;
	glGenTextures(1, &textureMap);
	glBindTexture(GL_TEXTURE_2D, textureMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, w, h, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	texture.reset(new Engine::Component::Texture(textureMap));

	// assigning the texture to the framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureMap, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FloatColorFramebuffer::clear()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

