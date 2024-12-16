#version 330 core

uniform sampler2D aoTexture;

uniform int kernelSize;

in vec2 uv;
out float out_col;

void main() 
{
    vec2 texelSize = 1.0 / vec2(textureSize(aoTexture, 0));

    float res = 0.0f;

    int halfKernelSize = (int)ceil(((float)kernelSize - 1.0f) / 2.0f);

    for (int x = -halfKernelSize; x < halfKernelSize; ++x) 
    {
        for (int y = -halfKernelSize; y < halfKernelSize; ++y) 
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            res += texture(aoTexture, uv + offset).r;
        }
    }

    out_col = res;
}