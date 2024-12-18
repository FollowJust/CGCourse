#version 330 core

layout(location=0) in vec2 pos;

out vec2 UV;

void main() {
	gl_Position = vec4(pos.xy, 0.0, 1.0);
	UV = 0.5f * gl_Position.xy + vec2(0.5f, 0.5f);
}