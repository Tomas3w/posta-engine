#include "engine/include/Component/Skeleton.h"

using Engine::Component::Skeleton;

Engine::Component::Transform Skeleton::str_to_transform(std::string s)
{
	std::stringstream s_s(s);
	std::string number;
	std::vector<float> numbers;
	while (getline(s_s, number, ' '))
	{
		if (!number.empty())
			numbers.push_back(stof(number));
	}
	Transform transform(glm::vec3(numbers[0], numbers[1], numbers[2]));
	transform.set_rotation(glm::quat(numbers[3], numbers[4], numbers[5], numbers[6]));
	transform.set_scale({numbers[7], numbers[8], numbers[9]});
	return transform;
}

static void print_bones(std::vector<Skeleton::Bone>& bones, std::vector<Engine::Component::Transform> bone_transforms, int level = 0)
{
	for (auto& bone : bones)
	{
		for (int i = 0; i < level; i++)
			std::cout << "\t";
		std::cout << "|" << bone.index << "|" << bone.name << "|" << bone.parent_name << "|" << to_string(bone_transforms[bone.index].get_position()) << "|" <<
			to_string(bone_transforms[bone.index].get_rotation()) << "|" << std::endl;
		print_bones(bone.children, bone_transforms, level + 1);
	}
}

Skeleton::Bone* Skeleton::Bone::search_in_bones(const std::string bone_name, std::vector<Bone>& bones)
{
	Skeleton::Bone* rbone = nullptr;
	for (Bone& bone : bones)
	{
		if (bone.name == bone_name)
			return &bone;
		else if ((rbone = search_in_bones(bone_name, bone.children)))
			return rbone;
	}
	return nullptr;
}

size_t Skeleton::Bone::count_of_bones(std::vector<Bone>& bones)
{
	size_t count = bones.size();
	for (auto& bone : bones)
		count += count_of_bones(bone.children);
	return count;
}

void Skeleton::Bone::on_bones(std::vector<Bone>& bones, std::function<void(Bone*)> func)
{
	for (auto& bone : bones)
	{
		func(&bone);
		on_bones(bone.children, func);
	}
}

Skeleton::Skeleton(std::filesystem::path path)
{
	std::filesystem::path skl_filename = path / "skeleton.skl";
	std::filesystem::path rest_filename = path / "rest.anmr";
	// collecting rest transforms
	std::ifstream file(rest_filename);
	if (!file)
		throw std::logic_error(std::string("couldn't find .anmr file: ") + rest_filename.string());
	std::string line;
	while (getline(file, line))
		rest_transforms.push_back(str_to_transform(line));
	file.close();
	
	// loading bones
	std::vector<Skeleton::Bone> bones;
	file.open(skl_filename);
	if (!file)
		throw std::logic_error(std::string("couldn't find .skl file: ") + skl_filename.string());
	size_t i = 0;
	while (getline(file, line))
	{
		Bone bone;
		bone.index = i;
		std::vector<std::string> parts = Engine::split(line, ' ');
		bone.name = parts[0];
		bone.parent_name = parts[1];
		bone.length = stof(parts[2]);
		bone.parent = nullptr; // this should be null by default
		if (bone.parent_name.empty())
			root_bones.push_back(bone);
		else
			bones.push_back(bone);
		i++;
	}
	file.close();

	// creating the tree of bones
	while (!bones.empty())
	{
		for (auto it = bones.begin(); it != bones.end(); it++)
		{
			Bone* bone = Bone::search_in_bones(it->parent_name, root_bones);
			if (bone)
			{
				it->parent = bone;
				bone->children.push_back(*it);
				it = bones.erase(it) - 1;
			}
		}
	}
	
	// bones to rest_matrices and to bone_transforms
	size_t count = Bone::count_of_bones(root_bones);
	rest_matrices.resize(count);
	bone_transforms.resize(count);
	Bone::on_bones(root_bones, [&](Bone* bone){
		bone_transforms[bone->index] = rest_transforms[bone->index]; // local to object transform, not to parent bone
		rest_matrices[bone->index] = bone_transforms[bone->index].get_matrix();
	});
	models.resize(Bone::count_of_bones(root_bones), glm::mat4(1.0f));
}

void Skeleton::add_animation(std::filesystem::path path)
{
	std::ifstream file(path);
	if (!file)
		throw std::logic_error(std::string("couldn't find .anm file: ") + path.string());
	std::string line;
	// getting data from file
	// fps
	getline(file, line);
	auto vec = Engine::split(line, '#');
	float fps = stof(vec[0]);
	// frames
	getline(file, line);
	vec = Engine::split(line, '#');
	int frames = stoi(vec[0]);
	// sample rate
	getline(file, line);
	vec = Engine::split(line, '#');
	int sample_rate = stoi(vec[0]);
	// bones count
	getline(file, line);
	vec = Engine::split(line, '#');
	int bone_count = stoi(vec[0]);
	if (static_cast<size_t>(bone_count) != Bone::count_of_bones(root_bones))
		throw std::logic_error(std::string("number of bones of animation is not equal to bones of skeleton, in file: ") + path.string());

	Animation* animation = new Animation;
	animation->duration = frames * sample_rate / fps;
	animation->frames.resize(frames);
	size_t i = 0;
	while (getline(file, line))
	{
		animation->frames[i / bone_count].push_back(Skeleton::str_to_transform(line));
		i++;
	}
	file.close();
	//std::cout << "frames of animation: " << animation->frames.size() << std::endl;
	//if (!animation->frames.empty())
	//	std::cout << "bones per frame: " << animation->frames[0].size() << std::endl;
	animations[path.stem().string()].reset(animation);
}

float Skeleton::get_animation_duration(std::string name)
{
	if (animations.count(name))
		return animations[name]->duration;
	return -1;
}

Engine::span<Engine::anim_mat4> Skeleton::get_animation_matrices(AnimationPlayer& player, bool advance_time)
{
	// Getting and updating bone_transforms
	auto& animation = player.animations[0];
	if (!animations.count(animation.name))
		throw std::logic_error(std::string("couldn't find the animation named: ") + animation.name);
	auto& anim = animations[animation.name];
	auto& frame = anim->frames[static_cast<int>(fmod(player.time, anim->duration) * anim->frames.size() / anim->duration)];
	bone_transforms = frame; // copying frame to bones

	// Getting matrices from bone_transforms
	auto it = bone_transforms.begin();
	auto jt = rest_matrices.begin();
	auto kt = models.begin();
	for (auto ft = bone_transforms.end(); it != ft; it++, jt++, kt++)
		(*kt) = it->get_matrix() * glm::inverse(*jt);
	if (advance_time)
		player.time += App::app->delta_time;
	return Engine::span<Engine::anim_mat4>(models.data(), models.size());
}

std::vector<std::vector<Engine::Component::Transform>>& Skeleton::Animation::get_local_frames(std::vector<Bone>& root_bones)
{
	// TOTEST
	if (local_frames.empty())
	{
		local_frames.resize(frames.size());
		auto it = local_frames.begin();
		for (auto& frame : frames)
		{
			(*it).resize(frame.size());
			auto& local_frame = (*it);
			Bone::on_bones(root_bones, [&](Bone* bone){
				if (bone->parent)
				{
					Engine::Component::Transform& parent_transform = frame[bone->parent->index];
					Engine::Component::Transform& child_transform = frame[bone->index];
					local_frame[bone->index] = parent_transform.as_origin_to(child_transform);
				}
				else
					local_frame[bone->index] = frame[bone->index];
			});
			it++;
		}
	}
	return local_frames;
}

Engine::span<Engine::anim_mat4> Skeleton::get_blend_animation_matrices(AnimationPlayer& player, bool advance_time)
{
	// TOIMPROVE
	bone_transforms = rest_transforms;
	std::vector<Engine::Component::Transform> local_transforms(rest_transforms.size());
	Bone::on_bones(root_bones, [&](Bone* bone){
		if (bone->parent)
		{
			Engine::Component::Transform& parent_transform = bone_transforms[bone->parent->index];
			Engine::Component::Transform& child_transform = bone_transforms[bone->index];
			local_transforms[bone->index] = parent_transform.as_origin_to(child_transform);
		}
		else
			local_transforms[bone->index] = rest_transforms[bone->index];
	});
	for (auto& animation : player.animations)
	{
		if (!animations.count(animation.name))
			throw std::logic_error(std::string("couldn't find the animation named: ") + animation.name);
		auto& anim = animations[animation.name];
		auto& frame = anim->get_local_frames(root_bones)[static_cast<int>(fmod(player.time, anim->duration) * anim->frames.size() / anim->duration)];

		auto it = local_transforms.begin();
		auto jt = frame.begin();
		for (auto ft = local_transforms.end(); it != ft; it++, jt++)
			(*it) = it->interpolate(*jt, animation.weight);
	}
	//Bone::adjust_bone_positions(bone_transforms, rest_transforms, root_bones);
	Bone::on_bones(root_bones, [&](Bone* bone){
		if (bone->parent)
			bone_transforms[bone->index] = local_transforms[bone->index].as_local_to(bone_transforms[bone->parent->index]);
		else
			bone_transforms[bone->index] = local_transforms[bone->index];
	});

	// converting bone_transforms to mat4
	auto it = bone_transforms.begin();
	auto jt = rest_matrices.begin();
	auto kt = models.begin();
	for (auto ft = bone_transforms.end(); it != ft; it++, jt++, kt++)
		(*kt) = it->get_matrix() * glm::inverse(*jt);
	if (advance_time)
		player.time += App::app->delta_time;
	return Engine::span<Engine::anim_mat4>(models.data(), models.size());
}

