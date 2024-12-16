#version 330

layout (location = 0) in vec2 pos;

uniform float aspectRatio;
uniform float tanHalfFOV;

out vec2 UV;
out vec2 ViewRay;

void main()
{
    gl_Position = vec4(pos.xy, 0.0f, 1.0f);
    UV = 0.5 * pos + vec2(0.5f, 0.5f);
    ViewRay.x = pos.x * aspectRatio * tanHalfFOV;
    ViewRay.y = pos.y * tanHalfFOV;
}