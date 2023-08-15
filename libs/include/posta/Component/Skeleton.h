#ifndef POSTAENGINE_SKELETON_H
#define POSTAENGINE_SKELETON_H
#include <iostream>
#include <vector>
#include <memory>
#include <unordered_map>
#include <fstream>
#include <functional>
#include "engine/include/Util/General.h"
#include "engine/include/Util/Assets.h"
#include "engine/include/Util/Shader.h"
#include "engine/include/App.h"
#include <glm/gtx/string_cast.hpp>

#include <filesystem>

namespace Engine::Component {

	/// Skeleton class, responsible for playing animations and storing the skeleton data
	/** To use, use addon to export the animation files: skeleton.skl, animation.anm, and rest.anmr.
	 * Remember to always set the transformation of the skeleton and the mesh to the identity */
	class Skeleton
	{
		public:
			/// Allows to play animations
			struct AnimationPlayer
			{
				float time;
				struct AnimationWeightsAndName
				{
					std::string name;
					float weight;
				};
				std::vector<AnimationWeightsAndName> animations; // animations that will be played together along with their respective weights (which sums up to 1)
				//std::vector<Engine::anim_mat4> cache;
			};
			/// Creates an skeleton from a folder that contains a skeleton.skl file, it also searches for a rest.anmr file
			Skeleton(std::filesystem::path path);

			/// Adds an animation
			void add_animation(std::filesystem::path path);

			/// Returns the duration of the animation named name
			float get_animation_duration(std::string name);

			/// Get animation matrices for an individual animation (the first one from the AnimationPlayer)
			/** Returns an span to the matrices to pass to the shader, it moves the animation time
			 * by the delta_time of the application */
			Engine::span<Engine::anim_mat4> get_animation_matrices(AnimationPlayer& player, bool advance_time = true);
			/// Get animation matrices for multiple animations
			/** It does not work with bone weights individually */
			Engine::span<Engine::anim_mat4> get_blend_animation_matrices(AnimationPlayer& player, bool advance_time = true);

			struct Bone
			{
				static Bone* search_in_bones(const std::string bone_name, std::vector<Bone>& bones);
				static size_t count_of_bones(std::vector<Bone>& bones);
				static void on_bones(std::vector<Bone>& bones, std::function<void(Bone*)> func);

				int index; // used in shaders and for indexing the bone_transforms
				std::string name;
				std::string parent_name;
				float length;
				Bone* parent; // = nullptr if there is no parent to this bone
				std::vector<Bone> children;
			};
			std::vector<Bone> root_bones;
			std::vector<Engine::Component::Transform> bone_transforms; // local to object, not to parent bone
		private:
			/// Splits str into numbers (by using space as a separator), and then returns a transform using those numbers
			/// The format used is: 3 numbers corresponding to the position: x y z, four numbers corresponding with the rotation (a quaternion) w x y z, and finally three numbers corresponding with the scale
			static Transform str_to_transform(std::string s);
			struct Animation
			{
				float duration; // in seconds
				std::vector<std::vector<Engine::Component::Transform>> frames; // each frame contains all the bone transforms
			
				/// Returns frames of the animation but with the transforms being local to the parent bone
				std::vector<std::vector<Engine::Component::Transform>>& get_local_frames(std::vector<Bone>& root_bones);
			private:
				std::vector<std::vector<Engine::Component::Transform>> local_frames; // each frame contains all the local to parent bone transforms

			};
			std::unordered_map<std::string, std::unique_ptr<Animation>> animations;
			std::vector<Engine::anim_mat4> models;
			std::vector<Engine::anim_mat4> rest_matrices;
			std::vector<Engine::Component::Transform> rest_transforms;
	};
}

#endif // POSTAENGINE_SKELETON_H
