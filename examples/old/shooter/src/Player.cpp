#include "Player.h"

std::unique_ptr<Engine::Component::StaticMesh> Player::arms_mesh;
//std::unique_ptr<Engine::Component::Skeleton> Player::arms_skeleton;

std::unique_ptr<Engine::Component::StaticMesh> Player::thin_capsule;
std::unique_ptr<Engine::Component::StaticMesh> Player::arrow;
std::unique_ptr<Engine::Component::StaticMesh> Player::player_model;

std::vector<std::unique_ptr<Engine::Sound>> Player::weapon_0_sounds;
std::unique_ptr<Engine::Sound> Player::weapon_pick_up_sound;

Player::Player(glm::vec3 position, std::string name) :
	body_transform(position),
	body(position, 0.0f, 0.5f, 1.5f)
	//player_skeleton("assets/player/tpv/skeleton.ozz")
{
	velocity = {0, 0, 0};
	nickname = name;
	animation = "Idle";
	//player_skeleton.add_animation("assets/player/tpv/Idle.ozz");
	//player_skeleton.add_animation("assets/player/tpv/Walk.ozz");
	life = 100.0f;
	move_flags[0] = false;
	move_flags[1] = false;
	move_flags[2] = false;
	move_flags[3] = false;

	// Setting weapons to none
	memset(has_gun, 0, sizeof(has_gun));
	memset(reserved_ammunition, 0, sizeof(reserved_ammunition));
	memset(charged_ammunition, 0, sizeof(charged_ammunition));
	weapon_selected = -1;
	is_shooting = false;
	is_firing = false;
	gun_offset = glm::vec3(0, 0, 0);
	stunt_effect = -1;
	fire_delay = 0;

	recharge_time = -1;
	
	srand(std::hash<std::string>()(name));
	color = glm::vec3(0, 0, 0);
	while (color.r + color.g + color.b < 1.0f)
		color = glm::vec3(rand() % 101 / 100.0f, rand() % 101 / 100.0f, rand() % 101 / 100.0f);
	srand(std::hash<std::string>()(name) + time(NULL));

	// Setting capsule physics
	//body->get_body()->setAngularFactor(Engine::to_btVector3(glm::vec3(0, 1, 0)));
	transform.set_position(position + glm::vec3(0, 1.5f, 0));
	
	if (!thin_capsule)
		thin_capsule.reset(new Engine::Component::StaticMesh(Engine::Assets::load_obj("assets/thin_capsule.obj")));
	if (!arrow)
		arrow.reset(new Engine::Component::StaticMesh(Engine::Assets::load_obj("assets/arrow.obj")));
	if (!player_model)
		player_model.reset(new Engine::Component::StaticMesh(Engine::Assets::load_obj_with_bones("assets/player/tpv/body")));
	if (!arms_mesh)
		arms_mesh.reset(new Engine::Component::StaticMesh(Engine::Assets::load_obj_with_bones("assets/player/fpv/arms")));
	if (weapon_0_sounds.empty())
	{
		for (int i = 0; i < 10; i++)
			weapon_0_sounds.emplace_back(new Engine::Sound("assets/weapons/0_fire" + std::to_string(i) + ".ogg"));
		weapon_pick_up_sound.reset(new Engine::Sound("assets/weapons/pick_up.ogg"));
	}
	/*
	if (!arms_skeleton)
	{
		arms_skeleton.reset(new Engine::Component::Skeleton("assets/player/fpv/skeleton.ozz"));
		arms_skeleton->add_animation("assets/player/fpv/Idle1.ozz");
		arms_skeleton->add_animation("assets/player/fpv/Reload1.ozz");
		arms_skeleton->add_animation("assets/player/fpv/Test.ozz");
	}*/
}

static void spring_like_ray(Engine::Component::Transform& body_transform, glm::vec3& velocity, glm::vec3 direction)
{
	float distance_to_ground = 5.0f, center_of_spring = 0.5f;
	btVector3 origin = Engine::to_btVector3(body_transform.get_position());
	btVector3 destination = Engine::to_btVector3(body_transform.get_position() - distance_to_ground * direction);
	btCollisionWorld::ClosestRayResultCallback callback(origin, destination);
	Engine::App::app->physics->world->rayTest(origin, destination, callback);
	if (callback.hasHit())
	{
		float d = callback.m_closestHitFraction * distance_to_ground;
		float dif = center_of_spring - d;
		if (dif > 0)
		{
			velocity += direction * ((dif * dif) * (dif > 0 ? 1:-1) * 10.0f);
			body_transform.set_position(body_transform.get_position() + direction * (dif / 1.1f));
		}
	}
}

void Player::loop()
{
	// Updates position and orientation
	// DEBUG
	/*{
		std::ofstream file("log.log", std::ios::app);
		glm::vec3 vel = Engine::from_btVector3(body->get_body()->getLinearVelocity());
		file << nickname << " " << to_string(body->get_transform().get_position()) << " " << to_string(vel) << std::endl;
		file.close();
	}*//*
	if (std::isnan(body->get_transform().get_position().x))
	{
		PRINT("well, this may be wrong...");
		body.reset(new Engine::Component::CapsuleRigidbody({0, 3, 0}, 1.0f, 0.5f, 2.0f));

		//btTransform prev_tr = Engine::Component::Transform::tobtTransform(prev_transform);
		//body.get_body()->getMotionState()->setWorldTransform(prev_tr);
		
		body->set_transform(prev_transform);
		body->get_body()->setLinearVelocity(Engine::to_btVector3(prev_linear_velocity));
		return;
	}
	else
	{
		prev_transform = body->get_transform();
		prev_linear_velocity = Engine::from_btVector3(body->get_body()->getLinearVelocity());
	}
	// ENDDEBUG*/
	transform.set_position(body_transform.get_position() + glm::vec3(0, 1.5f, 0));
	
	// Changing body transform rotation to match the view transform (this->transform)
	glm::vec3 front = transform.front();
	front = glm::normalize(glm::vec3(front.x, 0, front.z));
	//body->get_body()->setActivationState(1);
	//btTransform nt = body->get_body()->getCenterOfMassTransform();

	float angle = asin(front.x);
	if (front.z < 0)
		angle = -angle + M_PI;
	//nt.setRotation(btQuaternion(btVector3(0, 1, 0), angle + M_PI));
	//body->get_body()->setCenterOfMassTransform(nt);
	body_transform.set_rotation(glm::rotate(glm::quat(1, 0, 0, 0), angle, glm::vec3(0, 1, 0)));

	// Applies movement based on move_flags
	//  forward         backward        right           left
	if (move_flags[0] | move_flags[1] | move_flags[2] | move_flags[3]) // checks if any movement is applied
	{
		glm::vec3 direction = static_cast<float>(move_flags[0]) * -transform.front() + static_cast<float>(move_flags[1]) * transform.front() + static_cast<float>(move_flags[2]) * transform.right() + static_cast<float>(move_flags[3]) * -transform.right();
		direction = glm::normalize(glm::vec3(direction.x, 0, direction.z)) * 2.0f;

		if (stunt_effect > 0)
			direction *= 0.5f;

		//body->get_body()->applyCentralForce(Engine::to_btVector3(direction));
		velocity += direction;
		animation = "Walk";
	}
	else
		animation = "Idle";
	//glm::vec3 vel = Engine::from_btVector3(body->get_body()->getLinearVelocity());
	//body->get_body()->setLinearVelocity(Engine::to_btVector3(glm::vec3(vel.x / 1.1f, vel.y, vel.z / 1.1f)));
	// Updates position
	body_transform.set_position(body_transform.get_position() + velocity * Engine::App::app->delta_time);
	velocity = {velocity.x / 1.1f, velocity.y, velocity.z / 1.1f};

	// Make character collide
	float distance_to_ground = 2, center_of_spring = 1.5f;
	btVector3 origin = Engine::to_btVector3(body_transform.get_position());
	btVector3 destination = Engine::to_btVector3(body_transform.get_position() + glm::vec3(0, -distance_to_ground, 0));
	btCollisionWorld::ClosestRayResultCallback callback(origin, destination);
	Engine::App::app->physics->world->rayTest(origin, destination, callback);
	if (callback.hasHit())
	{
		float d = callback.m_closestHitFraction * distance_to_ground;
		float dif = center_of_spring - d;
		if (dif != 0)
		{
			velocity += glm::vec3(0, (dif * dif) * (dif > 0 ? 1:-1) / 1.0f, 0);
			body_transform.set_position(body_transform.get_position() + glm::vec3(0, dif / 10.0f, 0));
		}
	}
	else
		velocity += glm::vec3(0, -0.1f, 0);
	spring_like_ray(body_transform, velocity, glm::vec3(0, 0, 1));
	spring_like_ray(body_transform, velocity, glm::vec3(0, 0, -1));
	spring_like_ray(body_transform, velocity, glm::vec3(1, 0, 0));
	spring_like_ray(body_transform, velocity, glm::vec3(-1, 0, 0));
	glm::quat rot = glm::rotate(glm::quat(1, 0, 0, 0), static_cast<float>(M_PI / 4.0f), glm::vec3(0, 1, 0));
	spring_like_ray(body_transform, velocity, glm::vec3(0, 0, 1) * rot);
	spring_like_ray(body_transform, velocity, glm::vec3(0, 0, -1) * rot);
	spring_like_ray(body_transform, velocity, glm::vec3(1, 0, 0) * rot);
	spring_like_ray(body_transform, velocity, glm::vec3(-1, 0, 0) * rot);
	body.set_transform(body_transform);

	// Kills player if it's under the floor
	if (transform.get_position().y < -30)
		life = 0;
	
	// Reduces stunt effect time
	if (stunt_effect > 0)
		stunt_effect -= Engine::App::app->delta_time;
	
	// Autofire
	if (weapon_selected >= 0 && has_gun[weapon_selected])
	{
		if (is_firing)
		{
			if (charged_ammunition[weapon_selected] > 0)
				charged_ammunition[weapon_selected]--;
			is_firing = false;
		}
		if (fire_delay < 0)
		{
			if (is_shooting && recharge_time < 0)
			{
				is_firing = true;
				fire_delay = 0.1f;

				if (charged_ammunition[weapon_selected] > 0)
				{
					float rx = (rand() % 101 - 50) / 3000.0f;
					float ry = (rand() % 101 - 50) / 3000.0f;
					rotate_view(rx, ry);
				}
			}
			else
			{
				fire_delay = -1;
				gun_offset = glm::vec3(0, 0, 0);
			}
		}
		else
		{
			if (charged_ammunition[weapon_selected] > 0)
				gun_offset = glm::vec3(0, 0, fire_delay);
			else
				gun_offset = glm::vec3(0, 0, 0);
			fire_delay -= Engine::App::app->delta_time;
		}

		if (recharge_time > 0)
		{
			recharge_time -= Engine::App::app->delta_time;
			if (recharge_time < 0)
			{
				recharge_time = -1;
				short interchange = std::min(reserved_ammunition[weapon_selected], static_cast<short>(get_weapon_selected()->max_charged_ammunition - charged_ammunition[weapon_selected]));
				charged_ammunition[weapon_selected] += interchange;
				reserved_ammunition[weapon_selected] -= interchange;
			}
		}
	}
}

void Player::draw(Engine::ThreeDShader& shader)
{
	// Sets player mesh position
	//PRINT(nickname << " " << to_string(body.get_transform().get_position()));
	transform.set_position(body_transform.get_position() + glm::vec3(0, 1.5f, 0));
	// Debug meshes
	glm::mat4 matrix = transform.get_matrix();
	shader.model = matrix;
	shader.normal_model = glm::mat3(glm::transpose(glm::inverse(matrix)));
	//arrow->draw();

	shader.global_color = glm::vec4(glm::mix(color, glm::vec3(0.3f, 0, 0), 1 - life / 100.0f), 1.0f);
	matrix = body_transform.get_matrix();
	shader.model = matrix;
	shader.normal_model = glm::mat3(glm::transpose(glm::inverse(matrix)));
	//thin_capsule->draw();

	// Player mesh
	Engine::Component::Transform tr = transform;
	glm::vec3 front = transform.front();
	front = glm::normalize(glm::vec3(front.x, 0, front.z));
	tr.set_rotation(glm::quatLookAt(front, glm::vec3(0, 1, 0)));
	//tr.set_rotation(glm::rotate(tr.get_rotation(), static_cast<float>(M_PI), glm::vec3(0, 1, 0)));
	tr.set_position(body_transform.get_position() + glm::vec3(0, -1.5f, 0));
	tr.set_scale(glm::vec3(1.470588235f));

	matrix = tr.get_matrix();
	//shader.bones_matrices = player_skeleton.get_animation_matrices(animation);
	shader.model = matrix;
	shader.normal_model = glm::mat3(glm::transpose(glm::inverse(matrix)));
	player_model->draw();
	shader.global_color = glm::vec4(1, 1, 1, 1);
	
	// Gun
	while (weapon_selected > 0 && !has_gun[weapon_selected])
		weapon_selected--;
	if (weapon_selected >= 0)
	{
		Weapon* weapon = Weapon::get_weapon(weapon_selected);
		Engine::Component::Transform weapon_transform;
		weapon_transform.set_position(transform.get_position() + transform.to_local(glm::vec3(0.3f, -0.3f, -0.2f)));
		weapon_transform.set_rotation(transform.get_rotation());
		shader.model = weapon_transform.get_matrix();
		shader.normal_model = weapon_transform.get_normal_matrix();
		weapon->draw();
	}
}

void Player::rotate_view(float x, float y)
{
	glm::quat rot = transform.get_rotation();
	float m = (1 - (glm::dot(glm::vec3(0, -1, 0), rot * glm::vec3(0, 0, -1)) + 1) / 2.0f) * M_PI;

	glm::vec3 up = {0, 1, 0};
	glm::quat first_rotation = glm::rotate(rot, std::min(std::max(x, -m), static_cast<float>(M_PI - m)), glm::vec3(1, 0, 0));
	transform.set_rotation(glm::rotate(first_rotation, y, glm::toMat3(glm::inverse(first_rotation)) * up));
}

void Player::set_movement(bool* move_flags)
{
	memcpy(this->move_flags, move_flags, sizeof(bool) * 4);
}

bool Player::is_alive()	const
{
	return life > 0;
}

void Player::reduce_life(float damage)
{
	life -= damage;
}

void Player::replenish(WeaponItem& item)
{
	Weapon* weapon = item.get_weapon();
	int number = weapon->number;
	reserved_ammunition[number] += item.get_ammunition();
	if (!has_gun[number])
	{
		weapon_selected = number;
		has_gun[number] = true;

		int interchange = std::min(item.get_ammunition(), weapon->max_charged_ammunition);
		charged_ammunition[number] = interchange;
		reserved_ammunition[number] -= interchange;
	}
}

Weapon* Player::get_weapon_selected()
{
	return Weapon::get_weapon(weapon_selected);
}

void Player::stunt()
{
	stunt_effect = 1;
	float rx = (rand() % 101 - 50) / 300.0f;
	float ry = (rand() % 101 - 50) / 300.0f;
	rotate_view(rx, ry);
}

void Player::shoot()
{
	if (has_gun[weapon_selected] && charged_ammunition[weapon_selected] > 0)
	{
		is_shooting = true;
		if (weapon_selected == 0 && recharge_time < 0)
			is_firing = true;
	}
}

void Player::stop_shooting()
{
	is_shooting = false;
}

bool Player::should_fire()
{
	return is_firing && charged_ammunition[weapon_selected] && recharge_time < 0;
}

glm::vec3 Player::get_gun_offset()
{
	return gun_offset;
}

float Player::get_recharge_time()
{
	return recharge_time;
}

void Player::recharge()
{
	if (has_gun[weapon_selected])
	{
		stop_shooting();
		if (recharge_time < 0 && reserved_ammunition[weapon_selected] > 0 && charged_ammunition[weapon_selected] < get_weapon_selected()->max_charged_ammunition)
			recharge_time = get_weapon_selected()->charge_time;
	}
}

void Player::get_ammunition(short& r, short& c) 
{
	if (weapon_selected >= 0)
	{
		r = reserved_ammunition[weapon_selected];
		c = charged_ammunition[weapon_selected];
	}
}

int Player::get_weapon_selected_number()
{
	return weapon_selected;
}

void Player::set_weapon_selected(int _weapon_selected)
{
	weapon_selected = _weapon_selected;
}

void Player::make_all_guns_available()
{
	for (size_t i = 0; i < WEAPONS_AVAILABLE; i++)
		has_gun[i] = true;
}

void Player::set_ammunition(short r, short c)
{
	if (weapon_selected != -1)
	{
		reserved_ammunition[weapon_selected] = r;
		charged_ammunition[weapon_selected] = c;
	}
}

void Player::set_life(float _life)
{
	life = _life;
}

void Player::set_recharge_time(float _recharge_time)
{
	recharge_time = _recharge_time;
}

void Player::set_gun_offset(glm::vec3 _gun_offset)
{
	gun_offset = _gun_offset;
}

void Player::play_weapon_firing_sound()
{
	if (weapon_selected == 0)
	{
		float d = glm::length(transform.get_position() - Engine::App::app->camera->transform.get_position());
		if (d <= 1)
			weapon_0_sounds[rand() % weapon_0_sounds.size()]->play(1.0f);
		else
			weapon_0_sounds[rand() % weapon_0_sounds.size()]->play(1.0f / d);
	}
}

void PlayersVisualData::VisualData::write_to(uint8_t* data)
{
	Writer writer(data);
	writer.write_many(
		nickname,
		weapon_selected,
		transform.get_position(),
		transform.get_rotation(),
		life,
		gun_offset,
		recharge_time,
		charged_ammunition,
		reserved_ammunition
	);
}

uint32_t PlayersVisualData::VisualData::size()
{
	return Engine::NetworkPackageTypeSize::size_many(nickname, weapon_selected, transform.get_position(), transform.get_rotation(), life, gun_offset, recharge_time, charged_ammunition, reserved_ammunition);
}

uint32_t PlayersVisualData::VisualData::read_from(uint8_t* data)
{
	glm::vec3 pos;
	glm::quat rot;
	Writer writer(data);
	writer.read_many(
		nickname,
		weapon_selected,
		pos,
		rot,
		life,
		gun_offset,
		recharge_time,
		charged_ammunition,
		reserved_ammunition
	);
	transform.set_position(pos);
	transform.set_rotation(rot);
	return size();
}

PlayersVisualData::PlayersVisualData(std::unordered_map<std::string, std::unique_ptr<Player>>& players)
{
	data_cache = false;
	visual_data.reserve(players.size());
	for (auto& player : players)
	{
		Engine::Component::Transform tr = player.second->body_transform;
		tr.set_rotation(player.second->transform.get_rotation());
		short _charged, _reserved;
		player.second->get_ammunition(_reserved, _charged);
		int16_t charged = _charged, reserved = _reserved;
		visual_data.push_back(VisualData(player.first, player.second->get_weapon_selected_number(), tr, player.second->life, player.second->gun_offset, player.second->recharge_time, charged, reserved));
	}
}

Engine::span<uint8_t> PlayersVisualData::serialize()
{
	if (!data_cache)
	{
		// calculate size
		size_t size = 0;
		for (auto& visual : visual_data)
			size += visual.size();
		data.resize(sizeof(uint32_t) + size);
		
		// write count of VisualData in visual_data
		uint32_t visual_data_count = visual_data.size();
		memcpy(data.data(), &visual_data_count, sizeof(uint32_t));
		// write visual_data to data
		uint8_t* data_ptr = data.data() + sizeof(uint32_t);
		for (auto& visual : visual_data)
		{
			visual.write_to(data_ptr);
			data_ptr += visual.size();
		}
		data_cache = true;
		//PRINT(data.size());
	}
	return Engine::make_span(data);
}

void PlayersVisualData::deserialize(Engine::span<uint8_t> to_data)
{
	visual_data.clear();
	uint32_t count = reinterpret_cast<uint32_t&>(*to_data.data());
	
	uint8_t* ptr = to_data.data() + sizeof(uint32_t);
	for (uint32_t i = 0; i < count; i++)
	{
		visual_data.push_back(VisualData());
		ptr += visual_data.back().read_from(ptr);
	}
}

Engine::span<uint8_t> PlayerMovData::serialize()
{
	if (data.empty())
		data.resize(Engine::NetworkPackageTypeSize::size_many(action_flags, mov_x, mov_y));
	Writer writer(data.data());
	writer.write_many(action_flags, mov_x, mov_y);
	return Engine::make_span(data);
}

void PlayerMovData::deserialize(Engine::span<uint8_t> data)
{
	Writer writer(data.data());
	writer.read_many(action_flags, mov_x, mov_y);
}

void PlayerMovData::apply(Player& player)
{
	bool move_flags[4] = {
		static_cast<bool>((action_flags >> 0) & 1U),
		static_cast<bool>((action_flags >> 1) & 1U),
		static_cast<bool>((action_flags >> 2) & 1U),
		static_cast<bool>((action_flags >> 3) & 1U),
	};
	player.set_movement(move_flags);
	if ((action_flags >> 4) & 1U)
		player.recharge();
	if (((action_flags >> 5) & 1U) != ((previous_action_flags >> 5) & 1U))
	{
		previous_action_flags = action_flags;
		if ((action_flags >> 5) & 1U)
			player.shoot();
		else
			player.stop_shooting();
	}
	player.rotate_view(mov_y, mov_x);
}

void PlayerMovData::set_movement(bool* move_flags)
{
	for (size_t i = 0; i < 4; i++)
	{
		if (move_flags[i])
			action_flags |= 1UL << i;
		else
			action_flags &= ~(1UL << i);
	}
}

void PlayerMovData::recharge()
{
	action_flags |= 1UL << 4;
}

void PlayerMovData::stop_recharging()
{
	action_flags &= ~(1UL << 4);
}

void PlayerMovData::shoot()
{
	action_flags |= 1UL << 5;
}

void PlayerMovData::stop_shooting()
{
	action_flags &= ~(1UL << 5);
}

