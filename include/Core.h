#ifndef __CORE_H__
#define __CORE_H__

#include <memory>
#include <SDL2/SDL.h>
#include "Table.h"

namespace hockey
{

class Core
{
private:
	static constexpr int WINDOW_WIDTH  = 800;
	static constexpr int WINDOW_HEIGHT = 600;

	std::unique_ptr<SDL_Window, decltype(SDL_DestroyWindow)*> m_wnd { nullptr, SDL_DestroyWindow };

	std::unique_ptr<
		std::remove_pointer_t<SDL_GLContext>, decltype(SDL_GL_DeleteContext)*
	> m_gl_ctx { nullptr, SDL_GL_DeleteContext };

	std::unique_ptr<
		SDL_AudioDeviceID, ResourceDeleter<decltype(SDL_CloseAudioDevice)*>
	> m_audio_dev { 0, SDL_CloseAudioDevice };

	Table m_table;
	bool  m_is_running = false;

	Status init_SDL();
	Status init_window();
	Status init_OpenGL();
	Status init_audio();
	void   handle_events();

public:
	~Core();

	static Core& get();

	SDL_Window*       wnd()       const { return m_wnd.get();       }
	SDL_GLContext     gl_ctx()    const { return m_gl_ctx.get();    }
	SDL_AudioDeviceID audio_dev() const { return m_audio_dev.get(); }

	Status run();
	Status create();
};

}

#endif  //  !__CORE_H__
