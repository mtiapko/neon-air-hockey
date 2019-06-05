#version 130

uniform float time;
uniform float player_bright;
uniform float bot_bright;

uniform vec2 view;
uniform vec2 player_pos;
uniform vec2 bot_pos;
uniform vec2 puck_pos;

const float pi = 3.1415926;
const float inf = 1e+5;
const float eps = 1e-5;
const float bright = 0.01;

float rand(float n)
{
	return fract(sin(n) * 43758.5453123);
}

float noise1(float p)
{
	float fl = floor(p);
	float fc = fract(p);
	return mix(rand(fl), rand(fl + 1.0), fc);
}

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
	return pow(smoothstep(0.0, 1.5, abs(length(uv) - r)), 0.45);
}

float line_dist(vec2 uv, vec2 p1, vec2 p2)
{
	vec2 line = p2 - p1;
	float frac = dot(uv - p1, line) / dot(line, line);
	return distance(p1 + line * clamp(frac, 0.0, 1.0), uv);
}

float lightning_dist(vec2 uv, vec2 p1, vec2 p2)
{
	vec2 line = p2 - p1;
	float frac = dot(uv - p1, line) / dot(line, line);
	return distance(p1 + line * clamp(frac, 0.0, 1.0), uv + vec2(
		noise1(uv.y * 2.0 * time * 5.0) * 0.1 -
		noise1(uv.y * 3.0 + time * 10.0) * 0.1 +
		noise1(sin(uv.y / time) * 8.0) * 0.1 +
		noise1(uv.x * 8.0) * 0.3, 0) * 0.05);
}

void main() {
	const vec2 off = vec2(0.05, 0.15);
	const vec2 size = vec2(0.9, 0.7);
	vec2 uv = (gl_FragCoord.xy - vec2(0.5) * view) / min(view.x, view.y) + vec2(0.5);

	vec3 color;
	float dist;

	float custom_left_bright = bright * (1.0 + 0.35 * noise1(time * 10.0));
	float custom_right_bright = bright * (1.0 - 0.55 * noise1(time * 22.0));
	float gate_bright = bright * (1.0 - 0.2 * noise1(time * 32.0));

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
	color += vec3(0.75, 0.075, 0.05) * custom_left_bright / (dist + eps);

	dist = lightning_dist(uv, table_top_left - vec2(0.055, 0.025), table_bot_left + vec2(-0.055, 0.025));
	color += vec3(0.8, 0.65, 0.65) * player_bright * gate_bright / (dist + eps);

	// bot side
	vec2 table_top_right = table_top_left + vec2(table_size.x, 0);
	dist = arc_dist(uv, table_top_right, 0.055, pi / 2.0, pi / 2.0);
	color += vec3(0.05, 0.1, 0.75) * bright / (dist + eps);

	vec2 table_bot_right = table_top_right - vec2(0, table_size.y);
	dist = arc_dist(uv, table_bot_right, 0.055, pi / 2.0, 0.0);
	color += vec3(0.05, 0.1, 0.75) * bright / (dist + eps);

	dist = line_dist(uv, table_top_right + vec2(-0.025, 0.055), table_top_right - vec2(size.x * 0.5, 0) + vec2(0.055 * 2.0, 0.055));
	color += vec3(0.05, 0.1, 0.75) * custom_right_bright / (dist + eps);

	dist = line_dist(uv, table_bot_right - vec2(0.025, 0.055), table_bot_right - vec2(size.x * 0.5, 0) + vec2(0.055 * 2.0, -0.055));
	color += vec3(0.05, 0.1, 0.75) * bright / (dist + eps);

	dist = lightning_dist(uv, table_top_right + vec2(0.055, -0.025), table_bot_right + vec2(0.055, 0.025));
	color += vec3(0.65, 0.65, 0.85) * bot_bright * gate_bright / (dist + eps);

	// player
	dist = circle_dist(uv, player_pos, 0.055 * player_bright);
	color += vec3(0.75, 0.15, 0.25) * player_bright * (1.0 / bot_bright) * bright / (dist + eps);

	// bot
	dist = circle_dist(uv, bot_pos, 0.055 * bot_bright);
	color += vec3(0.05, 0.1, 0.75) * bot_bright * (1.0 / player_bright) * bright / (dist + eps);

	// puck
	dist = circle_dist(uv, puck_pos, 0.02);
	color += vec3(0.05, 0.55, 0.15) * bright / (dist + eps);

	gl_FragColor = vec4(color, 1.0);
}
