#version 330 core

layout(location=0) in vec2 pos;
layout(location=1) in vec3 col;
layout(location=2) in vec2 tex;

uniform mat4 mvp;

out vec3 vert_col;
out vec2 vert_tex;

void main() {
	vert_col = col;
	vert_tex = tex;
	gl_Position = mvp * vec4(pos.xy, 0.0, 1.0);
}
