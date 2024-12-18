#version 330

uniform sampler2D depthTexture;
uniform mat4 projection;

// SSAO params
#define MAX_KERNEL_SIZE (128)
uniform float sampleRadius;
uniform vec3 kernel[MAX_KERNEL_SIZE];
uniform int kernelSize;

in vec2 PremultipliedNDC;
in vec2 UV;

out vec4 FragColor;

float ReconstructViewZ(vec2 uv)
{
    float depth = texture(depthTexture, uv).x;

    float A = projection[2][2];
    float B = projection[3][2];
    
    float eye_z = B / (A + (2.0f * depth - 1.0f));

    return -eye_z;
}

vec3 ReconstructView(float eye_z)
{
    return vec3(-eye_z * PremultipliedNDC, eye_z);
}


void main()
{
    if (texture(depthTexture, UV).x == 1) {
        return;
    }
    
    vec3 viewPos = ReconstructView(ReconstructViewZ(UV.xy));

    float AO = 0.0;

    for (int i = 0 ; i < kernelSize ; i++) {
        vec3 samplePos = viewPos + kernel[i];

        vec4 offset = projection * vec4(samplePos, 1.0);
        offset.xy /= offset.w;
        offset.xy = offset.xy * 0.5 + vec2(0.5);

        float sampleDepth = ReconstructViewZ(offset.xy);

        if (abs(viewPos.z - sampleDepth) < sampleRadius) {
            AO += smoothstep(0.0, 1.0, sampleRadius / abs(viewPos.z - sampleDepth));
        }
    }

    AO = AO / kernelSize;

    FragColor = vec4(pow(AO, 2.0));
}
