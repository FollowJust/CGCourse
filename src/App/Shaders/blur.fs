#version 330 core

uniform sampler2D aoTexture;

uniform int kernelHalfSize;

in vec2 uv;
out vec4 out_col;

void main() 
{
    vec2 texelSize = 1.0 / vec2(textureSize(aoTexture, 0));

    vec3 res = vec3(0.0f);
    int totalSamples = 0;
    for (int x = -kernelHalfSize; x < kernelHalfSize; ++x) 
    {
        for (int y = -kernelHalfSize; y < kernelHalfSize; ++y) 
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;

            vec2 sampleUV = clamp(uv + offset, vec2(0.0f), vec2(1.0f));
            res += texture(aoTexture, sampleUV).rgb;
            totalSamples++;
        }
    }

    out_col = vec4(res / totalSamples, 1.0f);
}