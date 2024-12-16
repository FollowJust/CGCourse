#version 330 core

uniform sampler2D tex;

in vec2 uv;
out vec4 out_col;

void main() 
{
    out_col = vec4(texture(tex, uv).rgb, 1.0f);
}