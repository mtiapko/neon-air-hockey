#ifndef __SOUND_H__
#define __SOUND_H__

#include <memory>
#include <SDL2/SDL.h>
#include "Status.h"

namespace hockey
{

class Sound
{
private:
	std::unique_ptr<Uint8, decltype(SDL_FreeWAV)*> m_buf { 0, SDL_FreeWAV };
	Uint32 m_len;

public:
	void play(SDL_AudioDeviceID audio_dev) const;

	Status load(const std::string& path);
};

}

#endif  //  !__SOUND_H__
