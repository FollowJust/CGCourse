#version 330 core

out vec4 FragColor;

uniform sampler2D tex;

in vec3 pos;
in vec3 normal;
in vec2 uv;

void main()
{
	vec3 color = texture(tex, uv).rgb;
	FragColor = vec4(color, 1.0f);
}