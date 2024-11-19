
#version 330 core

uniform mat4 orthoProjection;

uniform vec2 screenResolution;

uniform vec2 mandelbrotStart;
uniform vec2 mandelbrotSize;
uniform int mandelbrotIterations;


in vec2 uv;

out vec4 out_col;

// respectfully taken from GPGPU homeworks :)
vec3 getColor(in float value) {
    vec3 a = vec3(0.5,     0.5,   0.5);
    vec3 b = vec3(0.5,     0.5,   0.5);
    vec3 c = vec3(1.0,     0.7,   0.4);
    vec3 d = vec3(0.0,     0.15,  0.20);
    return a + b * cos(2.0 * 3.14 * (c * value + d));
}

#define THRESHOLD (256.0f)
#define THRESHOLD_SQUARED (THRESHOLD * THRESHOLD)

void main() {
    vec4 test = orthoProjection * gl_FragCoord;
    vec2 coords = test.xy * screenResolution;
    // if (coords.x < -1 || coords.y < -1 || coords.x > 1 || coords.y > 1) {
    //     out_col = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    //     return;
    // }
    vec2 p0 = mandelbrotStart + (coords + vec2(0.5f)) * mandelbrotSize / screenResolution;

    float x = p0.x;
    float y = p0.y;

    int iter = 0;
    for (; iter < mandelbrotIterations; ++iter) {
        float xPrev = x;
        x = x * x - y * y + p0.x;
        y = 2.0f * xPrev * y + p0.y;
        if ((x * x + y * y) > THRESHOLD_SQUARED) {
            break;
        }
    }

    float result = (float)iter;
    if (iter != mandelbrotIterations) {
        result = result - log2(log2(sqrt(x * x + y * y)) / log2(THRESHOLD)) / log2(2.0f);
    }

    result = (1.0f * result / (float)mandelbrotIterations);

    out_col = vec4(getColor(result), 1.0f);
}