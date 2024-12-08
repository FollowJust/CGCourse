#version 330 core

out vec4 FragColor;

uniform sampler2D tex;

in vec3 pos;
in vec3 normal;
in vec2 uv;

void main()
{
	vec3 color = texture(tex, uv).rgb;
	color.b = 0.6f;
	FragColor = vec4(color, 1.0f);
}