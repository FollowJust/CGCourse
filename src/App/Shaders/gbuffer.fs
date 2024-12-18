#version 330 core

uniform sampler2D albedoTexture;

in vec3 Pos;
in vec3 Normal;
in vec2 UV;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec3 FragNormal;
layout(location = 2) out vec3 FragPos;

void main()
{
    FragColor = vec4(texture(albedoTexture, UV).rgb, 1.0f);
    FragNormal = vec3(Normal);
    FragPos = Pos;
}