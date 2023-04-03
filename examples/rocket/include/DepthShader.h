#ifndef DEPTHSHADER_H
#define DEPTHSHADER_H
#include "engine/include/Util/Shader.h"

class DepthShader : public Engine::Shader
{
public:
	DepthShader() :
		Engine::Shader("assets/shaders/2DDepth"),
		model(this, "model"),
		global_color(this, "global_color"),
		far_plane(this, "far_plane"),
		near_plane(this, "near_plane")
	{}

	Engine::Uniform<glm::mat4> model;
	Engine::Uniform<glm::vec4> global_color;
	Engine::Uniform<float> far_plane;
	Engine::Uniform<float> near_plane;
};

#endif // DEPTHSHADER_H
