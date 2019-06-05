#ifndef __TABLE_H__
#define __TABLE_H__

#include <chrono>
#include "RigidBody.h"
#include "Status.h"
#include "Shader.h"
#include "Sound.h"

namespace hockey
{

enum class GameState
{
	INIT_STATE = 0,
	PREPARE_STATE,
	RUNNING_STATE,
	WIN_STATE
};

enum class GameWinner
{
	HUMANITY = 0,
	SkyNET
};

class Table
{
private:
	static inline Vec2f m_size   = { 0.9f,  0.7f  };
	static inline Vec2f m_offset = { 0.05f, 0.15f };

	class Core* m_core = nullptr;

	size_t                                m_game_count = 0;
	GameState                             m_game_state;
	GameWinner                            m_game_winner;
	std::chrono::steady_clock::time_point m_start_time;

	float m_wnd_width;
	float m_wnd_height;

	Sound m_bounce;
	Sound m_table_bounce;
	Sound m_win_sounds[4];

	static void delete_vertex_array(GLuint id);
	static void delete_buffer(GLuint id);

	std::unique_ptr<
		GLuint, ResourceDeleter<decltype(Table::delete_vertex_array)*>
	>  m_vao { 0, Table::delete_vertex_array };

	std::unique_ptr<
		GLuint, ResourceDeleter<decltype(Table::delete_buffer)*>
	>  m_vbo { 0, Table::delete_buffer };

	std::unique_ptr<
		GLuint, ResourceDeleter<decltype(Table::delete_buffer)*>
	> m_ibo { 0, Table::delete_buffer };

	Shader m_shader;
	GLint  m_view_location = -1;
	GLint  m_time_location = -1;

	RigidBody m_player;
	GLint m_player_location = -1;
	GLint m_player_bright_location = -1;

	RigidBody m_bot;
	GLint m_bot_location = -1;
	GLint m_bot_bright_location = -1;

	RigidBody m_puck;
	GLint m_puck_location = -1;

	Status init_graphic();
	Status init_audio();

	bool check_gates(RigidBody& body);
	bool collide_with_borders(RigidBody& body);
	void clamp_pos_to_left_side(RigidBody& body);
	void clamp_pos_to_right_side(RigidBody& body);

	void update_player();
	void update_bot();
	void update_puck();
	void update_shader();

	/* game states */
	void init_state();
	void prepare_state();
	void running_state();
	void win_state();

public:
	void update();
	void render();

	void process_resize(int width, int height);
	void process_mouse_input(int x, int y);

	Status create();
};

}

#endif  //  !__TABLE_H__
