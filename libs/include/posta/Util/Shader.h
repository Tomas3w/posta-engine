#ifndef POSTAENGINE_SHADER_H
#define POSTAENGINE_SHADER_H
#include <typeinfo>
#include <GL/glew.h>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <filesystem>
#include <unordered_map>
#include <utility>
#include <posta/Util/General.h>

namespace posta {
	using anim_mat4 = glm::mat4;
	
	// Defines for all uniform related things, if a new uniform type is needed, add it to this list and implement a new set_uniform with the new type
	#define DO_DEFINES_OVER_EACH_TYPE_USED_BY_UNIFORMS(FUNC) \
		FUNC(bool); \
		FUNC(int); \
		FUNC(float); \
		FUNC##_GLM(mat4); \
		FUNC##_GLM(mat3); \
		FUNC##_GLM(vec4); \
		FUNC##_GLM(vec3); \
		FUNC##_GLM(vec2); \
		FUNC##_MANY_VEC2(); \
		FUNC##_MANY_VEC3(); \
		FUNC##_MANY_VEC4(); \
		FUNC##_ANIM_MAT4();
	template<class T> inline void set_uniform(T value, GLint id);
	template<> inline void set_uniform<bool>(bool value, GLint id) { glUniform1i(id, static_cast<int>(value)); }
	template<> inline void set_uniform<int>(int value, GLint id) { glUniform1i(id, value); }
	template<> inline void set_uniform<float>(float value, GLint id) { glUniform1f(id, value); }
	template<> inline void set_uniform<glm::mat4>(glm::mat4 value, GLint id) { /* std::cout << "updating uniform...(" << id << ")\n"; */ glUniformMatrix4fv(id, 1, false, &value[0][0]); }
	template<> inline void set_uniform<glm::mat3>(glm::mat3 value, GLint id) { glUniformMatrix3fv(id, 1, false, &value[0][0]); }
	template<> inline void set_uniform<glm::vec4>(glm::vec4 value, GLint id) { glUniform4fv(id, 1, &value[0]); }
	template<> inline void set_uniform<glm::vec3>(glm::vec3 value, GLint id) { glUniform3fv(id, 1, &value[0]); }
	template<> inline void set_uniform<glm::vec2>(glm::vec2 value, GLint id) { glUniform2fv(id, 1, &value[0]); }
	template<> inline void set_uniform<posta::span<glm::vec2>>(posta::span<glm::vec2> value, GLint id) { glUniform2fv(id, value.size(), &value[0][0]); }
	template<> inline void set_uniform<posta::span<glm::vec3>>(posta::span<glm::vec3> value, GLint id) { glUniform3fv(id, value.size(), &value[0][0]); }
	template<> inline void set_uniform<posta::span<glm::vec4>>(posta::span<glm::vec4> value, GLint id) { glUniform4fv(id, value.size(), &value[0][0]); }
	template<> inline void set_uniform<posta::span<posta::anim_mat4>>(posta::span<posta::anim_mat4> value, GLint id) { glUniformMatrix4fv(id, value.size(), false, reinterpret_cast<float*>(value.data())); }

	template<class T>
	class Uniform;

	// Specific for declaring uniform maps
	#define DECLARE_UNIFORM_UNORDERED_MAP_WITH_TYPE(t) std::unordered_map<std::string, std::unique_ptr<Uniform<t>>> uniforms_##t
	#define DECLARE_UNIFORM_UNORDERED_MAP_WITH_TYPE_GLM(t) std::unordered_map<std::string, std::unique_ptr<Uniform<glm::t>>> uniforms_##t
	#define DECLARE_UNIFORM_UNORDERED_MAP_WITH_TYPE_MANY_VEC2() std::unordered_map<std::string, std::unique_ptr<Uniform<posta::span<glm::vec2>>>> uniforms_many_vec2
	#define DECLARE_UNIFORM_UNORDERED_MAP_WITH_TYPE_MANY_VEC3() std::unordered_map<std::string, std::unique_ptr<Uniform<posta::span<glm::vec3>>>> uniforms_many_vec3
	#define DECLARE_UNIFORM_UNORDERED_MAP_WITH_TYPE_MANY_VEC4() std::unordered_map<std::string, std::unique_ptr<Uniform<posta::span<glm::vec4>>>> uniforms_many_vec4
	#define DECLARE_UNIFORM_UNORDERED_MAP_WITH_TYPE_ANIM_MAT4() std::unordered_map<std::string, std::unique_ptr<Uniform<posta::span<posta::anim_mat4>>>> uniforms_anim_mat4

	/// Shader abstract class
	/** Derive from this class to make a shader */
	class Shader
	{
		public:
			/// Constructs a shader from a folder located in path
			/** The path must be of a folder containing a vertex.glsl and/or a fragment.glsl.
			 * If either the fragment or the vertex shader is not provided, the shader will take it from the default_path */
			Shader(std::filesystem::path path);
			Shader(const Shader&) = delete;
			Shader operator=(const Shader&) = delete;
			~Shader();

			/// Binds the shader
			/** Once binded, it is possible to set the shader uniforms.
			 * The uniforms must be provided again each time the shader
			 * is binded, except when it's been binded before */
			void bind();

			/// Returns the program id, needed to get uniforms from the shader
			GLuint get_program() const;

			// Uniforms
			DO_DEFINES_OVER_EACH_TYPE_USED_BY_UNIFORMS(DECLARE_UNIFORM_UNORDERED_MAP_WITH_TYPE)
		private:
			GLuint shader_program;
	};

	#define IF_OF_TYPE_ADD_TO_UNORDERED_MAP_OF_SHADER(t) if constexpr(std::is_same_v<T, t>) { shader->uniforms_##t[location].reset(new Uniform<t>(*this)); }
	#define IF_OF_TYPE_ADD_TO_UNORDERED_MAP_OF_SHADER_GLM(t) if constexpr(std::is_same_v<T, glm::t>) { shader->uniforms_##t[location].reset(new Uniform<glm::t>(*this)); }
	#define IF_OF_TYPE_ADD_TO_UNORDERED_MAP_OF_SHADER_ANIM_MAT4() if constexpr(std::is_same_v<T, posta::span<posta::anim_mat4>>) { shader->uniforms_anim_mat4[location].reset(new Uniform<posta::span<posta::anim_mat4>>(*this)); }
	#define IF_OF_TYPE_ADD_TO_UNORDERED_MAP_OF_SHADER_MANY_VEC2() if constexpr(std::is_same_v<T, posta::span<glm::vec2>>) { shader->uniforms_many_vec2[location].reset(new Uniform<posta::span<glm::vec2>>(*this)); }
	#define IF_OF_TYPE_ADD_TO_UNORDERED_MAP_OF_SHADER_MANY_VEC3() if constexpr(std::is_same_v<T, posta::span<glm::vec3>>) { shader->uniforms_many_vec3[location].reset(new Uniform<posta::span<glm::vec3>>(*this)); }
	#define IF_OF_TYPE_ADD_TO_UNORDERED_MAP_OF_SHADER_MANY_VEC4() if constexpr(std::is_same_v<T, posta::span<glm::vec4>>) { shader->uniforms_many_vec4[location].reset(new Uniform<posta::span<glm::vec4>>(*this)); }

	template<class T>
	class Uniform
	{
		public:
			Uniform() : id(-1) {};
			Uniform(Shader* shader, std::string location)
			{
				//glUseProgram(shader_program);
				id = glGetUniformLocation(shader->get_program(), location.c_str());
				if (id == -1)
					std::cout << "[WARNING] Uniform location does not exists, or isn't being used, location = '" + location + "'" << std::endl;
				//std::cout << "after obtaining from " << shader_program << " the location " << location << ", we got " << id << "\n";
				DO_DEFINES_OVER_EACH_TYPE_USED_BY_UNIFORMS(IF_OF_TYPE_ADD_TO_UNORDERED_MAP_OF_SHADER)
			}

			void operator=(T value)
			{
				set_value(value);
			}

			void set_value(T& value)
			{
				set_uniform<T>(value, id);
			}
		private:
			GLint id;
	};
	#undef DECLARE_UNIFORM_UNORDERED_MAP_WITH_TYPE
	#undef DECLARE_UNIFORM_UNORDERED_MAP_WITH_TYPE_GLM
	#undef DECLARE_UNIFORM_UNORDERED_MAP_WITH_TYPE_ANIM_MAT4
	#undef DECLARE_UNIFORM_UNORDERED_MAP_WITH_TYPE_MANY_VEC2
	#undef DECLARE_UNIFORM_UNORDERED_MAP_WITH_TYPE_MANY_VEC3
	#undef DECLARE_UNIFORM_UNORDERED_MAP_WITH_TYPE_MANY_VEC4
	#undef IF_OF_TYPE_ADD_TO_UNORDERED_MAP_OF_SHADER
	#undef IF_OF_TYPE_ADD_TO_UNORDERED_MAP_OF_SHADER_GLM
	#undef IF_OF_TYPE_ADD_TO_UNORDERED_MAP_OF_SHADER_ANIM_MAT4
	#undef IF_OF_TYPE_ADD_TO_UNORDERED_MAP_OF_SHADER_MANY_VEC2
	#undef IF_OF_TYPE_ADD_TO_UNORDERED_MAP_OF_SHADER_MANY_VEC3
	#undef IF_OF_TYPE_ADD_TO_UNORDERED_MAP_OF_SHADER_MANY_VEC4
	#undef DO_DEFINES_OVER_EACH_TYPE_USED_BY_UNIFORMS

	/// TwoDShader, automatically set before calling on_draw_2d
	class TwoDShader : public Shader
	{
	public:
		TwoDShader() :
			posta::Shader("common/shaders/2D"),
			model(posta::Uniform<glm::mat4>(this, "model")),
			global_color(posta::Uniform<glm::vec4>(this, "global_color"))
		{}

		posta::Uniform<glm::mat4> model;
		posta::Uniform<glm::vec4> global_color;
	};

	/// ThreeDShader, default shader binded in a new project
	class ThreeDShader : public Shader
	{
		public:
			ThreeDShader() :
				posta::Shader("common/shaders/3D"),
				model(posta::Uniform<glm::mat4>(this, "model")),
				projection_view(posta::Uniform<glm::mat4>(this, "projection_view")),
				normal_model(posta::Uniform<glm::mat3>(this, "normal_model")),
				global_color(posta::Uniform<glm::vec4>(this, "global_color")),
				bones_matrices(posta::Uniform<posta::span<posta::anim_mat4>>(this, "bones_matrices")),
				use_bones(posta::Uniform<bool>(this, "use_bones"))
			{}

			posta::Uniform<glm::mat4> model;
			posta::Uniform<glm::mat4> projection_view;
			posta::Uniform<glm::mat3> normal_model;
			posta::Uniform<glm::vec4> global_color;
			posta::Uniform<posta::span<posta::anim_mat4>> bones_matrices;
			posta::Uniform<bool> use_bones;
	};
}

#endif // POSTAENGINE_SHADER_H
