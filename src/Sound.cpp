#include "Sound.h"
#include "Log.h"

namespace hockey
{

void Sound::play(SDL_AudioDeviceID audio_dev) const
{
	SDL_QueueAudio(audio_dev, m_buf.get(), m_len);
}

Status Sound::load(const std::string& path)
{
	SDL_AudioSpec spec;
	Uint8* buf;

	if (SDL_LoadWAV(path.data(), &spec, &buf, &m_len) == nullptr) {
		LOG_ERROR("Failed to load WAV file '" << path << "': " << SDL_GetError());
		return Status::ERROR;
	}

	m_buf.reset(buf);
	return Status::OK;
}

}
