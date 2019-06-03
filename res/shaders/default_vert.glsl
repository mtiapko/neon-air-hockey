#version 330 core

layout(location = 0) in vec2 pos;

uniform vec2 offset;

void main()
{
	vec2 p = pos;
	//if (p.x < 0.0) p.x += offset.x;
	//else p.x -= offset.x;

	//if (p.y < 0.0) p.y += offset.y;
	//else p.y -= offset.y;

	gl_Position = vec4(p, 0.0, 1.0);
}
