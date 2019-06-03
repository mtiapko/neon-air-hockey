#version 330 core

#define STEP 0.02
#define SIZE 0.001
#define CUR_SIZE 0.1
#define PUCK_SIZE 0.035

//const vec2 resolution = vec2(800, 600);
uniform vec2 resolution;
uniform vec2 view;
uniform vec2 offset;
uniform vec2 player_pos;
uniform vec2 bot_pos;
uniform vec2 puck_pos;

const vec2 mouse = vec2(0.0);

const float pi = 3.1415926;
const float inf = 1e+5;
const float eps = 1e-5;
const float bright = 0.01;

float arc_dist(vec2 uv, vec2 pos, float r, float angle, float start)
{
	uv -= pos;

	float s = sin(start - angle * 0.5);
	float c = cos(start - angle * 0.5);
	uv *= mat2(
		c, s,
		-s, c
	);

	float a = acos(dot(uv, vec2(r, 0)) / (length(uv) * r));
	return
		pow(smoothstep(0.001, 1.5, abs(length(uv) - r)), 0.45) /* circle */
		+ pow(smoothstep(0.01, angle, a), 10.0);  /* arc */
}

float circle_dist(vec2 uv, vec2 pos, float r)
{
	uv -= pos;
	return pow(smoothstep(0.001, 1.5, abs(length(uv) - r)), 0.45);
}

float line_dist(vec2 uv, vec2 p1, vec2 p2)
{
	vec2 line = p2 - p1;
	float frac = dot(uv - p1, line) / dot(line, line);
	return distance(p1 + line * clamp(frac, 0.0, 1.0), uv);
}

void main() {
	//float ratio = 1.0;//view.x / view.y;
	//vec2 pos = ((gl_FragCoord.xy - 0.5 * offset * view) / resolution.xy);
	//pos.x *= ratio;

	//vec2 cur = player_pos;
	//cur.x *= ratio;
	//cur.x = clamp(cur.x, CUR_SIZE, ratio - CUR_SIZE);
	//cur.y = clamp(cur.y, CUR_SIZE, 1.0 - CUR_SIZE);

	/*vec3 color = vec3(0.4, 0.4, 0.45) * vec3(smoothstep(SIZE, SIZE + 0.002,
		distance(vec2(mod(pos.x, STEP), mod(pos.y, STEP)), vec2(STEP / 2.0))));
	if (step(distance(cur, pos), CUR_SIZE) != 0.0)
		color = vec3(0.1, 0.25, 0.7);*/

	//  puck
	/*vec2 puck = puck_pos;
	puck.x *= ratio;
	float puck_k = smoothstep(PUCK_SIZE + 0.002, PUCK_SIZE, distance(puck, pos));
	color = puck_k * vec3(0.65, 0.125, 0.165) + (1.0 - puck_k) * color;*/

	vec2 uv;// = gl_FragCoord.xy / view;
	uv = (gl_FragCoord.xy - vec2(0.5) * view) / min(view.x, view.y) + vec2(0.5);
	//uv.x *= ratio;

	vec3 color;
	float dist;

	vec2 off = vec2(0.05, 0.15);//offset * 0.5;
	vec2 size = vec2(0.9, 0.7);//resolution / view;
	//off.x *= ratio;
	//size.x *= ratio;

	// table
	// player side
	vec2 table_size = vec2(size.x - 0.055 * 2.0, size.y - 0.055 * 2.0);
	vec2 table_top_left = vec2(off.x + 0.055, 1.0 - off.y - 0.055);
	dist = arc_dist(uv, table_top_left, 0.055, pi / 2.0, pi);
	color = vec3(0.75, 0.075, 0.05) * bright / (dist + eps);

	vec2 table_bot_left = table_top_left - vec2(0, table_size.y);
	dist = arc_dist(uv, table_bot_left, 0.055, pi / 2.0, pi * 1.5);
	color += vec3(0.75, 0.075, 0.05) * bright / (dist + eps);

	dist = line_dist(uv, table_top_left + vec2(0.025, 0.055), table_top_left + vec2(size.x * 0.5, 0) + vec2(-0.055 * 2.0, 0.055));
	color += vec3(0.75, 0.075, 0.05) * bright / (dist + eps);

	dist = line_dist(uv, table_bot_left + vec2(0.025, -0.055), table_bot_left + vec2(size.x * 0.5, 0) + vec2(-0.055 * 2.0, -0.055));
	color += vec3(0.75, 0.075, 0.05) * bright / (dist + eps);

	dist = line_dist(uv, table_top_left - vec2(0.055, 0.025), table_bot_left + vec2(-0.055, 0.025));
	color += vec3(0.8, 0.65, 0.65) * bright / (dist + eps);

	// bot side
	vec2 table_top_right = table_top_left + vec2(table_size.x, 0);
	dist = arc_dist(uv, table_top_right, 0.055, pi / 2.0, pi / 2.0);
	color += vec3(0.05, 0.1, 0.75) * bright / (dist + eps);

	vec2 table_bot_right = table_top_right - vec2(0, table_size.y);
	dist = arc_dist(uv, table_bot_right, 0.055, pi / 2.0, 0.0);
	color += vec3(0.05, 0.1, 0.75) * bright / (dist + eps);

	dist = line_dist(uv, table_top_right + vec2(-0.025, 0.055), table_top_right - vec2(size.x * 0.5, 0) + vec2(0.055 * 2.0, 0.055));
	color += vec3(0.05, 0.1, 0.75) * bright / (dist + eps);

	dist = line_dist(uv, table_bot_right - vec2(0.025, 0.055), table_bot_right - vec2(size.x * 0.5, 0) + vec2(0.055 * 2.0, -0.055));
	color += vec3(0.05, 0.1, 0.75) * bright / (dist + eps);

	dist = line_dist(uv, table_top_right + vec2(0.055, -0.025), table_bot_right + vec2(0.055, 0.025));
	color += vec3(0.65, 0.65, 0.85) * bright / (dist + eps);

	// player
	dist = circle_dist(uv, player_pos, 0.055);
	color += vec3(0.75, 0.15, 0.25) * bright / (dist + eps);

	// bot
	dist = circle_dist(uv, bot_pos, 0.055);
	color += vec3(0.05, 0.1, 0.75) * bright / (dist + eps);

	// puck
	dist = circle_dist(uv, puck_pos, 0.02);
	color += vec3(0.05, 0.55, 0.15) * bright / (dist + eps);

	gl_FragColor = vec4(color, 1.0);
}
