#version 330

layout (location = 0) in vec2 pos;

uniform float aspectRatio;
uniform float tanHalfFOV;

out vec2 PremultipliedNDC;
out vec2 UV;

void main()
{
    gl_Position = vec4(pos.xy, 0.0f, 1.0f);

    UV = 0.5f * pos + vec2(0.5f, 0.5f);
    PremultipliedNDC.x = pos.x * aspectRatio * tanHalfFOV;
    PremultipliedNDC.y = pos.y * tanHalfFOV;
}