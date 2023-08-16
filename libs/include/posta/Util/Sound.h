#ifndef POSTAENGINE_SOUND_UTILITY_H
#define POSTAENGINE_SOUND_UTILITY_H
#include <SDL2/SDL_mixer.h>
#include <filesystem>
#include <algorithm>

namespace posta {
	/// Sound class for managing all kinds of sound, the sound is loaded in memory and read when needed
	class Sound
	{
		public:
			class SoundPlaying
			{
				friend class Sound;
				public:
					/// Sets the volume to volume (clamped value from 0 to 1)
					void set_volume(float volume);
					/// Gets the volume for the playing sound
					float get_volume();
					/// Stops the sound from playing
					void halt();
					/// Returns whether the playing sound was actually spawned
					operator bool();
				private:
					SoundPlaying(int _channel);
					int channel;
			};

			/// Cronstructs a sound object from a .ogg file
			Sound(std::filesystem::path path);
			~Sound();
			Sound(const Sound&) = delete;
			Sound& operator=(const Sound&) = delete;

			/// Start playing the sound (if possible) and returns a SoundPlaying object to interact with the sound playing
			/// \param volume initial volume of the sound(from 0 to 1, value will be clamped to this range)
			/// \param loops number of times that the sound will be played (one after another)
			/// \return returns SoundPlaying to interact with the newly spawned sound, if the sound was not spawn, then the soundplaying bool operator returns false
			SoundPlaying play(float volume, int loops = 0);
		private:
			Mix_Chunk* chunk;
	};
}

#endif // POSTAENGINE_SOUND_UTILITY_H
