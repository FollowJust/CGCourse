#version 330

uniform sampler2D depthTexture;
uniform float sampleRadius;
uniform mat4 projection;

const int MAX_KERNEL_SIZE = 64;
uniform vec3 aoKernel[MAX_KERNEL_SIZE];

in vec2 UV;
in vec2 ViewRay;

out vec4 FragColor;

float CalcViewZ(vec2 Coords)
{
    float Depth = texture(depthTexture, Coords).x;
    float ViewZ = projection[3][2] / (2 * Depth -1 - projection[2][2]);
    return ViewZ;
}


void main()
{
    float ViewZ = CalcViewZ(UV);

    float ViewX = ViewRay.x * ViewZ;
    float ViewY = ViewRay.y * ViewZ;

    vec3 Pos = vec3(ViewX, ViewY, ViewZ);

    float AO = 0.0;

    for (int i = 0 ; i < MAX_KERNEL_SIZE ; i++) {
        vec3 samplePos = Pos + aoKernel[i];
        vec4 offset = vec4(samplePos, 1.0);
        offset = projection * offset;
        offset.xy /= offset.w;
        offset.xy = offset.xy * 0.5 + vec2(0.5);

        float sampleDepth = CalcViewZ(offset.xy);

        if (abs(Pos.z - sampleDepth) < sampleRadius) {
            AO += step(sampleDepth,samplePos.z);
        }
    }

    AO = 1.0 - AO/64.0;

    FragColor = vec4(pow(AO, 2.0));
}
