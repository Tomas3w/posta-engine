#include <engine/include/Util/Framebuffer.h>
#include <engine/include/App.h>

using Engine::Framebuffer;
using Engine::DepthFramebuffer;

void Framebuffer::unbind_framebuffer()
{
	glViewport(0, 0, Engine::App::app->get_width(), Engine::App::app->get_height());
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
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

void DepthFramebuffer::bind()
{
	glViewport(0, 0, w, h);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

void DepthFramebuffer::clear()
{
	glClear(GL_DEPTH_BUFFER_BIT);
}

