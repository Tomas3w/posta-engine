#include <engine/include/Util/Sound.h>

using Engine::Sound;

void Sound::SoundPlaying::set_volume(float volume)
{
	volume = std::clamp(volume, 0.0f, 1.0f);
	if (channel != -1)
		Mix_Volume(channel, volume * MIX_MAX_VOLUME);
}

float Sound::SoundPlaying::get_volume()
{
	if (channel != -1)
		return Mix_Volume(channel, -1) / static_cast<float>(MIX_MAX_VOLUME);
	return 0;
}

void Sound::SoundPlaying::halt()
{
	if (channel != -1)
		Mix_HaltChannel(channel);
}

Sound::SoundPlaying::SoundPlaying::operator bool()
{
	return channel != -1;
}

Sound::SoundPlaying::SoundPlaying(int _channel)
{
	channel = _channel;
}

Sound::Sound(std::filesystem::path path)
{
	chunk = Mix_LoadWAV(path.string().c_str());
	if (chunk == nullptr)
		throw std::logic_error(std::string("couldn't load sound file: ") + path.string());
}

Sound::~Sound()
{
	if (chunk)
		Mix_FreeChunk(chunk);
}

Sound::SoundPlaying Sound::play(float volume, int loops)
{
	if (chunk)
	{
		SoundPlaying sound_playing(Mix_PlayChannel(-1, chunk, loops));
		if (sound_playing)
			sound_playing.set_volume(volume);
		return sound_playing;
	}
	return SoundPlaying(-1);
}

