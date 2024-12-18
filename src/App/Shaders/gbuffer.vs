#version 330 core
layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 vertexUV;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float morphingMixValue;
uniform float morphCoef;
uniform float morphClampValue;

out vec3 Pos;
out vec3 Normal;
out vec2 UV;

void main()
{
	gl_Position = projection * view * model * vec4(vertexPos, 1.0f);
	Pos = vec3(view * model * vec4(vertexPos, 1.0f));
	Normal = normalize(mat3(model) * vertexNormal);
	UV = vertexUV;
}