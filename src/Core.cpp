#include <algorithm>
#include "Core.h"
#include "Log.h"

namespace hockey
{

Core::~Core()
{
	SDL_Quit();
}

/* static */ Core& Core::get()
{
	static Core core;
	return core;
}

Status Core::init_SDL()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		LOG_ERROR("Failed to initialize SDL: " << SDL_GetError());
		return Status::ERROR;
	}

	return Status::OK;
}

Status Core::init_window()
{
	m_wnd.reset(SDL_CreateWindow("Air Hockey", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
	));

	if (!m_wnd) {
		LOG_ERROR("Failed to create window: " << SDL_GetError());
		return Status::ERROR;
	}

	SDL_ShowCursor(SDL_DISABLE);
	return Status::OK;
}

Status Core::init_OpenGL()
{
	SDL_GL_SetAttribute (SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute (SDL_GL_CONTEXT_MINOR_VERSION, 1);
	// SDL_GL_SetAttribute (SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	m_gl_ctx.reset(SDL_GL_CreateContext(m_wnd.get()));
	if (!m_gl_ctx) {
		LOG_ERROR("Failed to create OpenGL context: " << SDL_GetError());
		return Status::ERROR;
	}

	auto err = glewInit();
	if (err != GLEW_OK) {
		LOG_ERROR("Failed to initialize GLEW: " << glewGetErrorString(err));
		return Status::ERROR;
	}

	return Status::OK;
}

Status Core::init_audio()
{
	SDL_AudioSpec desired {};
	desired.channels = 2;
	desired.samples  = 256;
	desired.format   = AUDIO_S16;
	desired.freq     = 44100;

	SDL_AudioSpec obtained;
	m_audio_dev.reset(SDL_OpenAudioDevice(nullptr, false, &desired, &obtained, 0));
	if (!m_audio_dev) {
		LOG_ERROR("Failed to open audio device");
		return Status::ERROR;
	}

	SDL_PauseAudioDevice(m_audio_dev.get(), false);
	return Status::OK;
}

void Core::handle_events()
{
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		switch (e.type) {
			case SDL_QUIT: m_is_running = false; break;
			case SDL_WINDOWEVENT:
				if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
					m_table.process_resize(e.window.data1, e.window.data2);

				break;
			case SDL_MOUSEMOTION:
				m_table.process_mouse_input(e.motion.x, e.motion.y);
				break;
		}
	}
}

Status Core::run()
{
	auto curr_time = std::chrono::high_resolution_clock::now();
	auto update_time = std::chrono::high_resolution_clock::duration(std::chrono::seconds(1)) / 120;
	auto next_update = curr_time;
	auto next_print = curr_time + std::chrono::seconds(1);

	int fps = 0;
	int ups = 0;

	m_is_running = true;
	while (m_is_running) {
		while (std::chrono::high_resolution_clock::now() >= next_update) {
			this->handle_events();
			next_update = next_update + update_time;
			m_table.update();
			++ups;
		}

		m_table.render();
		++fps;

		if (std::chrono::high_resolution_clock::now() >= next_print) {
			LOG("FPS: " << fps << ", UPS: " << ups);
			next_print += std::chrono::seconds(1);
			fps = 0;
			ups = 0;
		}
	}

	return Status::OK;
}

Status Core::create()
{
	if (auto ret = this->init_SDL(); !ret)
		return ret;

	if (auto ret = this->init_window(); !ret)
		return ret;

	if (auto ret = this->init_OpenGL(); !ret)
		return ret;

	if (auto ret = this->init_audio(); !ret)
		return ret;

	if (auto ret = m_table.create(); !ret)
		return ret;

	m_table.process_resize(800, 600);
	return Status::OK;
}

}
