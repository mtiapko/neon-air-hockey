#include <algorithm>
#include <thread>
#include "Table.h"
#include "Core.h"
#include "Log.h"

namespace hockey
{
/* static */ void Table::delete_vertex_array(GLuint id)
{
	glDeleteVertexArrays(1, &id);
}

/* static */ void Table::delete_buffer(GLuint id)
{
	glDeleteBuffers(1, &id);
}

Status Table::init_graphic()
{
	if (auto ret = m_shader.create("res/shaders/default_vert.glsl", "res/shaders/default_frag.glsl"); !ret)
		return ret;

	m_shader.enable();
	m_time_location = glGetUniformLocation(m_shader.id(), "time");
	m_view_location = glGetUniformLocation(m_shader.id(), "view");
	m_player_location = glGetUniformLocation(m_shader.id(), "player_pos");
	m_player_bright_location = glGetUniformLocation(m_shader.id(), "player_bright");
	m_bot_location = glGetUniformLocation(m_shader.id(), "bot_pos");
	m_bot_bright_location = glGetUniformLocation(m_shader.id(), "bot_bright");
	m_puck_location = glGetUniformLocation(m_shader.id(), "puck_pos");

	static const uint32_t indices[] = { 0, 1, 2, 0, 1, 3 };
	static const float vertices[][2] = {
		{ -1.0f, -1.0f },
		{  1.0f,  1.0f },
		{ -1.0f,  1.0f },
		{  1.0f, -1.0f }
	};

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	m_vao.reset(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	m_vbo.reset(vbo);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(0);

	GLuint ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	m_ibo.reset(ibo);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	return Status::OK;
}

Status Table::init_audio()
{
	if (auto ret = m_bounce.load("res/sounds/bounce2.wav"); !ret)
		return ret;

	if (auto ret = m_table_bounce.load("res/sounds/table_bounce.wav"); !ret)
		return ret;

	static const std::string win_sound_paths[] = {
		"res/sounds/first_blood.wav",
		"res/sounds/win_1.wav",
		"res/sounds/win_2.wav",
		"res/sounds/win_3.wav"
	};

	for (const auto& path: win_sound_paths) {
		auto id = std::distance(win_sound_paths, &path);
		if (auto ret = m_win_sounds[id].load(path); !ret)
			return ret;
	}

	return Status::OK;
}

bool Table::check_gates(RigidBody& body)
{
	float gate_top = m_offset.y() + m_size.y() - body.size() * 2.0f;
	float gate_bot = m_offset.y() + body.size() * 2.0f;
	if (body.pos().y() < gate_bot || body.pos().y() > gate_top)
		return false;

	if (body.pos().x() <= m_offset.x() + body.size()) {
		m_game_winner = GameWinner::SkyNET;
		return true;
	}

	if (body.pos().x() >= m_offset.x() + m_size.x() - body.size()) {
		m_game_winner = GameWinner::HUMANITY;
		return true;
	}

	return false;
}

bool Table::collide_with_borders(RigidBody& body)
{
	float left_limit = m_offset.x() + body.size();
	float right_limit = m_size.x() + m_offset.x() - body.size();

	float top_limit = m_size.y() + m_offset.y() - body.size();
	float bot_limit = m_offset.y() + body.size();

	/* chto? real collision detection? pfff... 2k19 outside */
	bool is_collided = false;
	if (body.pos().x() >= right_limit) {
		body.pos().set_x(right_limit);
		body.dir().set_x(-body.dir().x());
		is_collided = true;
	} else if (body.pos().x() <= left_limit) {
		body.pos().set_x(left_limit);
		body.dir().set_x(-body.dir().x());
		is_collided = true;
	}

	if (body.pos().y() >= top_limit) {
		body.pos().set_y(top_limit);
		body.dir().set_y(-body.dir().y());
		is_collided = true;
	} else if (body.pos().y() <= bot_limit) {
		body.pos().set_y(bot_limit);
		body.dir().set_y(-body.dir().y());
		is_collided = true;
	}

	return is_collided;
}

void Table::clamp_pos_to_left_side(RigidBody& body)
{
	body.pos() = Vec2f {
		std::clamp(body.pos().x(), m_offset.x() + body.size(), m_offset.x() + m_size.x() * 0.5f - body.size()),
		std::clamp(body.pos().y(), m_offset.y() + body.size(), m_offset.y() + m_size.y() - body.size())
	};
}

void Table::clamp_pos_to_right_side(RigidBody& body)
{
	body.pos() = Vec2f {
		std::clamp(body.pos().x(), m_offset.x() + m_size.x() * 0.5f + body.size(), m_offset.x() + m_size.x() - body.size()),
		std::clamp(body.pos().y(), m_offset.y() + body.size(), m_offset.y() + m_size.y() - body.size())
	};
}

void Table::update_player()
{
	if (m_player.process_collision(m_puck)) {
		SDL_ClearQueuedAudio(m_core->audio_dev());
		m_bounce.play(m_core->audio_dev());
	}

	m_player.force() = 0.0f;
}

void Table::update_bot()
{
	float base_bot_force = m_bot.force();
	float dist = m_bot.pos().distance(m_puck.pos());
	if (m_bot.force() > -0.01f && dist < m_bot.size() + m_puck.size() + 0.05f)
		m_bot.force() = 0.0425f;

	if (m_bot.force() > 0.0f) {
		Vec2f target = m_puck.pos();
		if (m_bot.pos().x() < m_puck.pos().x()) {
			if (m_puck.pos().y() > 0.5f) target += Vec2f { 0.05f, -0.05f };
			else target += Vec2f { 0.05f, 0.05f };
		}

		m_bot.dir() = target - m_bot.pos();
		m_bot.dir().normalize();
	} else {
		base_bot_force += 0.001f;
		if (base_bot_force > 0.0f)
			base_bot_force = 0.02f;
	}

	m_bot.pos() += m_bot.dir() * m_bot.force();
	m_bot.force() *= ((std::rand() % 8) + 10) / 15.0f;
	if (m_bot.process_collision(m_puck)) {
		SDL_ClearQueuedAudio(m_core->audio_dev());
		m_bounce.play(m_core->audio_dev());
		base_bot_force = -0.015f;
	}

	m_bot.force() = base_bot_force;
	this->clamp_pos_to_right_side(m_bot);
}

void Table::update_puck()
{
	m_puck.apply_force();
	m_puck.force() *= 0.985f;

	if (this->collide_with_borders(m_puck)) {
		SDL_ClearQueuedAudio(m_core->audio_dev());
		m_table_bounce.play(m_core->audio_dev());
	}
}

void Table::update_shader()
{
	glUniform2f(m_player_location, m_player.pos().x(), m_player.pos().y());
	glUniform2f(m_bot_location, m_bot.pos().x(), m_bot.pos().y());
	glUniform2f(m_puck_location, m_puck.pos().x(), m_puck.pos().y());
	glUniform1f(m_time_location, std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::steady_clock::now() - m_start_time).count() / 1000.0f);
}

void Table::init_state()
{
	m_player.size() = 0.055f;
	m_bot.size()    = 0.055f;
	m_puck.size()   = 0.02f;

	m_player.pos() = Vec2f { m_offset.x() + 0.1f, 0.5f };
	m_bot.pos()    = Vec2f { m_offset.x() + m_size.x() - 0.1f, 0.5f };
	m_puck.pos()   = Vec2f { 0.5f, 0.5f };
	m_puck.dir()   = Vec2f { 0.0f, 1.0f };

	m_player.force() = 0.0f;
	m_bot.force() = 0.0f;
	m_puck.force() = 0.0f;

	glUniform1f(m_player_bright_location, 1.0f);
	glUniform1f(m_bot_bright_location, 1.0f);

	m_start_time = std::chrono::steady_clock::now();
	m_game_state = GameState::PREPARE_STATE;
}

void Table::prepare_state()
{
	if (std::chrono::steady_clock::now() > m_start_time + std::chrono::seconds(1)) {
		m_start_time = std::chrono::steady_clock::now();
		m_game_state = GameState::RUNNING_STATE;
		return;
	}

	this->update_shader();
}

void Table::running_state()
{
	this->update_player();
	this->update_bot();
	this->update_puck();
	this->update_shader();

	if (this->check_gates(m_puck)) {
		if (m_game_count++ == 0) m_win_sounds[0].play(m_core->audio_dev());
		else m_win_sounds[std::rand() % 3 + 1].play(m_core->audio_dev());

		m_start_time = std::chrono::steady_clock::now();
		m_game_state = GameState::WIN_STATE;
	}
}

void Table::win_state()
{
	if (std::chrono::steady_clock::now() > m_start_time + std::chrono::seconds(2)) {
		m_game_state = GameState::INIT_STATE;
		return;
	}

	float duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - m_start_time).count();
	float bright = 2000.0f / (duration * 12.0f + 1e-8) - 0.075f;

	if (m_game_winner == GameWinner::HUMANITY) glUniform1f(m_bot_bright_location, bright);
	else glUniform1f(m_player_bright_location, bright);

}

void Table::update()
{
	switch (m_game_state) {
		case GameState::INIT_STATE:    return this->init_state();
		case GameState::PREPARE_STATE: return this->prepare_state();
		case GameState::RUNNING_STATE: return this->running_state();
		case GameState::WIN_STATE:     return this->win_state();
	}
}

void Table::render()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	SDL_GL_SwapWindow(m_core->wnd());
}

Status Table::create()
{
	m_core = &Core::get();

	if (auto ret = this->init_graphic(); !ret)
		return ret;

	if (auto ret = this->init_audio(); !ret)
		return ret;

	m_game_state = GameState::INIT_STATE;
	m_start_time = std::chrono::steady_clock::now();
	return Status::OK;
}

void Table::process_resize(int width, int height)
{
	m_wnd_width = width;
	m_wnd_height = height;

	glUniform2f(m_view_location, m_wnd_width, m_wnd_height);
	glViewport(0, 0, m_wnd_width, m_wnd_height);
}

void Table::process_mouse_input(int x, int y)
{
	if (m_game_state != GameState::RUNNING_STATE)
		return;

	m_player.pos() = Vec2f { x / m_wnd_width, 1.0f - y / m_wnd_height };
	this->clamp_pos_to_left_side(m_player);

	m_player.dir() = m_player.pos();
	m_player.force() = m_player.dir().normalize();
}

}
