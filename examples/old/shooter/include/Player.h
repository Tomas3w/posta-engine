#ifndef PLAYER_H
#define PLAYER_H
#include "PackagesTypes.h"
#include <engine/include/Util/General.h>
#include <engine/include/Component/Transform.h>
#include <engine/include/Component/CapsuleRigidbody.h>
#include <engine/include/Component/StaticMesh.h>
#include <engine/include/Component/Skeleton.h>
#include <engine/include/Util/Sound.h>
#include <memory>
#include "Weapon.h"
#include "WeaponItem.h"
#include <engine/include/Util/NetworkPackage.h>

class Player
{
public:
	static std::unique_ptr<Engine::Component::StaticMesh> arms_mesh;
	//static std::unique_ptr<Engine::Component::Skeleton> arms_skeleton;
	static std::unique_ptr<Engine::Component::StaticMesh> thin_capsule;
	static std::unique_ptr<Engine::Component::StaticMesh> arrow;
	static std::unique_ptr<Engine::Component::StaticMesh> player_model;
	static std::vector<std::unique_ptr<Engine::Sound>> weapon_0_sounds;
	static std::unique_ptr<Engine::Sound> weapon_pick_up_sound;
	Player(glm::vec3 position, std::string name);

	void loop();
	void draw(Engine::ThreeDShader& shader);

	/// Actions that the player can do each frame
	void rotate_view(float x, float y);
	/// Set the movement flags: forward, backward, right, left
	void set_movement(bool* move_flags);

	/// Check if life > 0
	bool is_alive() const;

	/// Reduces life by the damage inflicted
	void reduce_life(float damage);

	/// Replenish weapons ammunitions and gives the player the weapon needed
	void replenish(WeaponItem& item);

	/// Returns the selected weapon, if none available, returns nullptr
	Weapon* get_weapon_selected();

	/// Slows the player down and rotates the view
	void stunt();

	void shoot();
	void stop_shooting();
	bool should_fire(); /// checks if the player just fired the 0 weapon

	glm::vec3 get_gun_offset(); /// used for effects on the weapons

	void recharge(); /// initiates the recharge sequence
	float get_recharge_time(); /// time before finish recharging

	void get_ammunition(short& r, short& c); /// Returns a pair of reserved ammunition and current ammunition
	int get_weapon_selected_number();
	void set_weapon_selected(int _weapon_selected);
	void make_all_guns_available();

	void set_ammunition(short r, short c);

	void set_life(float _life);
	void set_recharge_time(float _recharge_time);
	void set_gun_offset(glm::vec3 _gun_offset);

	void play_weapon_firing_sound();
	
	glm::vec3 color;

	Engine::Component::Transform transform; // camera view and body position
	Engine::Component::Transform body_transform;
	glm::vec3 velocity;
	Engine::Component::CapsuleRigidbody body;
	//std::unique_ptr<Engine::Component::CapsuleRigidbody> body; // DEBUG
	//Engine::Component::CapsuleRigidbody body;
	//Engine::Component::Skeleton player_skeleton;

	std::string nickname;
	float life;
	glm::vec3 gun_offset;
	float recharge_time;

	Engine::Component::Transform prev_transform; // DEBUG
	glm::vec3 prev_linear_velocity; // DEBUG
private:
	std::string animation;
	bool move_flags[4]; // forward, backward, right, left
	
	bool has_gun[WEAPONS_AVAILABLE]; // Weapons availability
	short reserved_ammunition[WEAPONS_AVAILABLE]; // Ammunition available
	short charged_ammunition[WEAPONS_AVAILABLE]; // Ammunition ready to be fired
	int weapon_selected;

	bool is_recharging;
	float stunt_effect; // current duration of the stunt effect in seconds

	bool is_shooting, is_firing;
	float fire_delay;
};

struct PlayerConnectionData
{
	PlayerConnectionData() : respawn_time(0), kill_count(0), net(respawn_time, kill_count) {}
	float respawn_time;
	uint16_t kill_count;
	Engine::NetworkPackageTemplate<static_cast<uint32_t>(NetworkPackageType::PLAYER_CONNECTION_DATA), float, uint16_t> net;
};

struct PlayersVisualData : public Engine::NetworkPackage
{
	PlayersVisualData() : data_cache(false) {}
	PlayersVisualData(std::unordered_map<std::string, std::unique_ptr<Player>>& players);
	uint32_t get_type() override { return static_cast<uint32_t>(NetworkPackageType::PLAYERS_VISUAL_DATA); }
	Engine::span<uint8_t> serialize() override;
	void deserialize(Engine::span<uint8_t>) override;

	bool data_cache;
	
	struct VisualData
	{
		VisualData() : weapon_selected(-1) {}
		VisualData(std::string _nickname, int _weapon_selected, Engine::Component::Transform _transform, float _life, glm::vec3 _gun_offset, float _recharge_time, int16_t _charged_ammunition, int16_t _reserved_ammunition) : nickname(_nickname), weapon_selected(_weapon_selected), transform(_transform), life(_life), gun_offset(_gun_offset), recharge_time(_recharge_time), charged_ammunition(_charged_ammunition), reserved_ammunition(_reserved_ammunition) {}
		
		void write_to(uint8_t* data); /// Writes size() number of bytes of data to data (scale of transform is not included)
		uint32_t size(); /// Returns the number of bytes needed to write the visual data
		uint32_t read_from(uint8_t* data); /// reads from data and stores the info into its attributes, returning the number of bytes read

		std::string nickname;
		int16_t weapon_selected;
		Engine::Component::Transform transform;
		float life;
		glm::vec3 gun_offset;
		float recharge_time;
		int16_t charged_ammunition;
		int16_t reserved_ammunition;
	};
	std::vector<VisualData> visual_data;
};

struct PlayerWeapon0Sound : public StringPackage
{
	PlayerWeapon0Sound() = default;
	PlayerWeapon0Sound(std::string str) : StringPackage(str) {}
	uint32_t get_type() override { return static_cast<uint32_t>(NetworkPackageType::PLAYER_WEAPON_0_SOUND); }
};

struct PlayerDiedData : public StringPackage
{
	PlayerDiedData() = default;
	PlayerDiedData(std::string str) : StringPackage(str) {}
	uint32_t get_type() override { return static_cast<uint32_t>(NetworkPackageType::PLAYER_DIED_DATA); }
};

struct PlayerMovData : public Engine::NetworkPackage
{
	PlayerMovData() { clear(); }
	uint32_t get_type() override { return static_cast<uint32_t>(NetworkPackageType::PLAYER_MOV_DATA); }
	Engine::span<uint8_t> serialize() override;
	void deserialize(Engine::span<uint8_t>) override;
	
	void apply(Player& player);

	void clear() {action_flags = 0; mov_x = mov_y = 0; }
	void set_movement(bool* move_flags);
	void recharge();
	void stop_recharging();
	void shoot();
	void stop_shooting();

	uint8_t action_flags; // bits correspond to actions
	float mov_x, mov_y; // x and y camera movement
private:
	uint8_t previous_action_flags;
};

#endif // PLAYER_H
