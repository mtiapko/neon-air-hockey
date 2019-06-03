#include <algorithm>
#include <GL/glew.h>
#include "Log.h"
#include "Core.h"

namespace hockey
{

Core::~Core()
{
	SDL_Quit();
}

void Core::handle_events()
{
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		switch (e.type) {
			case SDL_QUIT: m_is_running = false; break;
			case SDL_WINDOWEVENT:
				if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
					m_wnd_width = e.window.data1;
					m_wnd_height = e.window.data2;

					glViewport(0, 0, m_wnd_width, m_wnd_height);
					int width = m_wnd_width * 0.9;
					int height = m_wnd_height * 0.75;
					if (width < height * 2.0) {
						//width = m_wnd_width * 0.8;
						height = width / 2;
					} else {
						//height = m_wnd_height * 0.7;
						width = height * 2;
					}

					float x_offset = (m_wnd_width - width) / ((float)m_wnd_width);
					float y_offset = (m_wnd_height - height) / ((float)m_wnd_height);
					glUniform2f(m_res_location, width, height);
					glUniform2f(m_offset_location, x_offset, y_offset);
					glUniform2f(m_view_location, m_wnd_width, m_wnd_height);
				}
				break;
			case SDL_MOUSEMOTION:
				m_player_x = (float)e.motion.x / (m_wnd_width);
				m_player_x = std::clamp(m_player_x, 0.05f + 0.055f, 0.05f + 0.9f * 0.5f - 0.055f);

				m_player_y = 1.0f - (float)e.motion.y / (m_wnd_height);
				m_player_y = std::clamp(m_player_y, 0.15f + 0.055f, 0.15f + 0.7f - 0.055f);

				glUniform2f(m_player_location, m_player_x, m_player_y);
		}
	}
}

Status Core::run()
{
	m_is_running = true;
	SDL_ShowCursor(SDL_DISABLE);
	while (m_is_running) {
		glClear(GL_COLOR_BUFFER_BIT);
		this->handle_events();

		// bot
		float dist = std::sqrt(std::pow(m_bot_x - m_puck_x, 2.0f) + std::pow(m_bot_y - m_puck_y, 2.0f));
		float bot_speed;
		if (m_bot_speed > -0.01f && dist < 0.055f + 0.02f + 0.05f)
			bot_speed = 0.025f;
		else
			bot_speed = m_bot_speed;

		if (bot_speed > 0.0f) {
			m_bot_unit_x = (m_puck_x - m_bot_x) / dist;
			m_bot_unit_y = (m_puck_y - m_bot_y) / dist;
		} else {
			m_bot_speed += 0.001f;
			if (m_bot_speed > 0.0f)
				m_bot_speed = 0.01f;
		}

		m_bot_x += m_bot_unit_x * bot_speed;
		m_bot_y += m_bot_unit_y * bot_speed;

		dist = std::sqrt(std::pow(m_bot_x - m_puck_x, 2.0f) + std::pow(m_bot_y - m_puck_y, 2.0f));
		float d = 0.055f + 0.02f;
		if (dist < d) {
			m_puck_unit_x = (m_puck_x - m_bot_x) / dist;
			m_puck_unit_y = (m_puck_y - m_bot_y) / dist;
			m_puck_force = (d - dist) * 1.5f;
			m_bot_speed = -0.015f;
		}

		m_bot_x = std::clamp(m_bot_x, 0.05f + 0.9f * 0.5f + 0.055f, 0.95f - 0.055f);
		m_bot_y = std::clamp(m_bot_y, 0.15f + 0.055f, 0.7f + 0.055f);
		glUniform2f(m_bot_location, m_bot_x, m_bot_y);

		// player
		dist = std::sqrt(std::pow(m_player_x - m_puck_x, 2.0f) + std::pow(m_player_y - m_puck_y, 2.0f));
		d = 0.055f + 0.02f;
		if (dist < d) {
			m_puck_unit_x = (m_puck_x - m_player_x) / dist;
			m_puck_unit_y = (m_puck_y - m_player_y) / dist;
			m_puck_force = (d - dist) * 1.5f;
		}

		// puck
		m_puck_force = std::min(m_puck_force, 0.075f);
		m_puck_x += m_puck_unit_x * m_puck_force;
		m_puck_y += m_puck_unit_y * m_puck_force;
		m_puck_force = std::max(m_puck_force * 0.95f, 0.008f);
		if (m_puck_x >= 0.95f - 0.02f) {
			m_puck_x = 0.95f - 0.02f;
			m_puck_unit_x = -m_puck_unit_x;
		} else if (m_puck_x <= 0.05f + 0.02f) {
			m_puck_x = 0.05f + 0.02f;
			m_puck_unit_x = -m_puck_unit_x;
		}

		if (m_puck_y >= 0.85f - 0.02f) {
			m_puck_y = 0.85f - 0.02f;
			m_puck_unit_y = -m_puck_unit_y;
		} else if (m_puck_y <= 0.15f + 0.02f) {
			m_puck_y = 0.15f + 0.02f;
			m_puck_unit_y = -m_puck_unit_y;
		}

		glUniform2f(m_puck_location, m_puck_x, m_puck_y);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		SDL_GL_SwapWindow(m_wnd.get());

		/*auto err = glGetError();
		if (err != GL_NO_ERROR)
			LOG_ERROR("OpenGL error #" << err);*/
	}

	return Status::OK;
}

Status Core::create()
{
	SDL_Init(SDL_INIT_EVERYTHING);

	/* set OpenGL version */
	SDL_GL_SetAttribute (SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute (SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute (SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	/* window */
	m_wnd.reset(SDL_CreateWindow("Air Hockey", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		m_wnd_width, m_wnd_height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
	));

	if (!m_wnd)
		return Status::ERROR;

	/* GL context */
	m_ctx.reset(SDL_GL_CreateContext(m_wnd.get()));
	if (!m_ctx)
		return Status::ERROR;

	/* GLEW */
	glewInit();

	//  TODO: remove
	LOG("Version: " << glGetString(GL_VERSION));
	LOG("Vendor:  " << glGetString(GL_VENDOR));

	/* GL setup */
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	if (auto ret = m_table_shader.create("res/shaders/default_vert.glsl", "res/shaders/default_frag.glsl"); !ret)
		return ret;

	m_table_shader.enable();
	m_res_location = glGetUniformLocation(m_table_shader.id(), "resolution");
	m_offset_location = glGetUniformLocation(m_table_shader.id(), "offset");
	m_view_location = glGetUniformLocation(m_table_shader.id(), "view");
	m_player_location = glGetUniformLocation(m_table_shader.id(), "player_pos");
	m_puck_location = glGetUniformLocation(m_table_shader.id(), "puck_pos");
	m_bot_location = glGetUniformLocation(m_table_shader.id(), "bot_pos");
	glUniform2f(m_res_location, m_wnd_width * 0.9, m_wnd_height * 0.75);
	glUniform2f(m_view_location, m_wnd_width, m_wnd_height);

	static const uint32_t indices[] = { 0, 1, 2, 0, 1, 3 };
	static const float vertices[][2] = {
		{ -1.0f, -1.0f },
		{  1.0f,  1.0f },
		{ -1.0f,  1.0f },
		{  1.0f, -1.0f }
	};

	glGenVertexArrays(1, &m_table_vao);
	glBindVertexArray(m_table_vao);

	glGenBuffers(1, &m_table_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_table_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &m_table_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_table_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	return Status::OK;
}

}
