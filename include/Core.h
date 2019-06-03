#ifndef __CORE_H__
#define __CORE_H__

#include <memory>
#include <SDL2/SDL.h>
#include "Shader.h"

namespace hockey
{

class Core
{
private:
	std::unique_ptr<SDL_Window, decltype(SDL_DestroyWindow)*> m_wnd { nullptr, SDL_DestroyWindow };
	int m_wnd_width  = 800;
	int m_wnd_height = 600;

	std::unique_ptr<
		std::remove_pointer_t<SDL_GLContext>, decltype(SDL_GL_DeleteContext)*
	> m_ctx { nullptr, SDL_GL_DeleteContext };

	Shader m_table_shader;
	GLuint m_table_vao = 0;
	GLuint m_table_vbo = 0;
	GLuint m_table_ibo = 0;
	GLint  m_res_location = -1;
	GLint  m_offset_location = -1;
	GLint  m_view_location = -1;

	float m_player_x = 0;
	float m_player_y = 0;
	GLint m_player_location = -1;

	float m_puck_x = 0.5;
	float m_puck_y = 0.5;
	float m_puck_unit_x = 0.0f;
	float m_puck_unit_y = 0.0f;
	float m_puck_force = 0.0f;
	GLint m_puck_location = -1;

	float m_bot_x = 0.8f;
	float m_bot_y = 0.5f;
	float m_bot_unit_x = 0.0f;
	float m_bot_unit_y = 0.0f;
	float m_bot_speed = 0.01f;
	GLint m_bot_location = -1;

	bool m_is_running = false;

	void handle_events();

public:
	~Core();

	Status run();
	Status create();
};

}

#endif  //  !__CORE_H__
