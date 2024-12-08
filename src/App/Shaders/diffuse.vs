#version 330 core
layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 vertexUV;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 pos;
out vec3 normal;
out vec2 uv;

void main()
{
	mat4 MVP = projection * view * model;
	gl_Position = MVP * vec4(vertexPos, 1.0f);
	pos = vertexPos;
	normal = normalize(vertexNormal);
	uv = vec2(vertexUV.x, vertexUV.y);
}