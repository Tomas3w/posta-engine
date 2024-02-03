#ifndef BASICSHADER_H
#define BASICSHADER_H
#include <engine/include/Util/Shader.h>

class BasicShader : public Engine::Shader
{
	public:
		BasicShader() :
			Engine::Shader("common/shaders/basic"),
			model(Engine::Uniform<glm::mat4>(this, "model")),
			projection_view(Engine::Uniform<glm::mat4>(this, "projection_view")),
			normal_model(Engine::Uniform<glm::mat3>(this, "normal_model")),
			global_color(Engine::Uniform<glm::vec4>(this, "global_color")),
			bones_matrices(Engine::Uniform<Engine::span<Engine::anim_mat4>>(this, "bones_matrices")),
			use_bones(Engine::Uniform<bool>(this, "use_bones"))
		{}

		Engine::Uniform<glm::mat4> model;
		Engine::Uniform<glm::mat4> projection_view;
		Engine::Uniform<glm::mat3> normal_model;
		Engine::Uniform<glm::vec4> global_color;
		Engine::Uniform<Engine::span<Engine::anim_mat4>> bones_matrices;
		Engine::Uniform<bool> use_bones;
};

#endif // BASICSHADER_H
