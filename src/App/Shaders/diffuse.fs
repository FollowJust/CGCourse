#version 330 core

uniform sampler2D tex_2d;

in vec3 vert_col;
in vec2 vert_tex;

out vec4 out_col;

void main() {
	vec4 texel = texture(tex_2d, vert_tex);
	float greyscale_factor = dot(texel.rgb, vec3(0.21, 0.71, 0.07));
	out_col = vec4(mix(vec3(greyscale_factor), vert_col.rgb, 0.7), 1.0f);
}