#ifndef PACKAGESTYPES_H
#define PACKAGESTYPES_H
#include <engine/include/Util/NetworkPackage.h>

enum class NetworkPackageType
{
	SIMPLE_STRING,
	PLAYERS_VISUAL_DATA,
	PLAYER_DIED_DATA,
	PLAYER_WEAPON_0_SOUND,
	PLAYER_MOV_DATA,
	PLAYER_CONNECTION_DATA,
	WEAPON_ITEM_CHANGE,
};

class StringPackage : public Engine::NetworkPackage
{
public:
	StringPackage() = default;
	StringPackage(std::string string)
	{
		str = string;
	}

	uint32_t get_type()
	{
		return static_cast<uint32_t>(NetworkPackageType::SIMPLE_STRING);
	}

	Engine::span<uint8_t> serialize()
	{
		data.resize(str.size() + sizeof(size_t));
		size_t size = str.size();
		memcpy(data.data(), &size, sizeof(size_t));
		memcpy(data.data() + sizeof(size_t), str.data(), str.size());
		return Engine::make_span(data);
	}

	void deserialize(Engine::span<uint8_t> data)
	{
		size_t size = 0;
		memcpy(&size, data.data(), sizeof(size_t));
		str.resize(size);
		memcpy(str.data(), data.data() + sizeof(size_t), size);
	}

	std::string str;
};


#endif // PACKAGESTYPES_H
