#ifndef SHADOWSHADER_H
#define SHADOWSHADER_H

#include "engine/include/Util/Shader.h"
class ShadowShader : public Engine::Shader
{
public:
	ShadowShader() :
		Engine::Shader("assets/shaders/Shadow"),
		model(this, "model"),
		normal_model(this, "normal_model"),
		projection_view(this, "projection_view"),
		use_bones(this, "use_bones"),
		bones_matrices(this, "bones_matrices"),
		global_color(this, "global_color"),
		intexture(this, "intexture"),
		depthtexture(this, "depthtexture"),
		sun_light_projection_view(this, "sun_light_projection_view"),
		sun_light_position(this, "sun_light_position"),
		sun_light_direction(this, "sun_light_direction"),
		sun_depth_distance_ratio(this, "sun_depth_distance_ratio")
	{}

	Engine::Uniform<glm::mat4> model;
	Engine::Uniform<glm::mat3> normal_model;
	Engine::Uniform<glm::mat4> projection_view;
	Engine::Uniform<bool> use_bones;
	Engine::Uniform<Engine::span<Engine::anim_mat4>> bones_matrices;
	Engine::Uniform<glm::vec4> global_color;
	Engine::Uniform<int> intexture;
	Engine::Uniform<int> depthtexture;
	Engine::Uniform<glm::mat4> sun_light_projection_view;
	Engine::Uniform<glm::vec3> sun_light_position;
	Engine::Uniform<glm::vec3> sun_light_direction;
	Engine::Uniform<float> sun_depth_distance_ratio;
};

#endif // SHADOWSHADER_H
