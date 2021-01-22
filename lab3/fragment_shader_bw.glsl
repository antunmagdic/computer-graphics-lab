#version 410 core

in vec3 position;

uniform int max_iter;
uniform vec2 c;
uniform vec2 center;
uniform float scale;

float gamma = 0.05;

layout(location = 0) out vec4 color;

void main() {
    float zx = position.x / scale + center.x;
    float zy = position.y / scale + center.y;
    float cx = c.x;
    float cy = c.y;
    float x = 0;
    float r = 0.5 + 0.5 * sqrt(1 + 4*sqrt(cx*cx + cy*cy));
    float rsquared = r*r;
    int i = 0;
    while (zx*zx + zy*zy <= rsquared && i < max_iter) {
        float zzx = zx*zx - zy*zy + cx;
        float zzy = 2*zx*zy + cy;
        zx = zzx;
        zy = zzy;
        i++;
    }
    if (i == max_iter) {
        // convergence
        color = vec4(1.0, 1.0, 1.0, 1.0);
    } else {
        float ifloat = float(i);
        color = (1 - exp(-gamma * ifloat)) * vec4(0.97, 0.97, 0.97, 1.0);
    }
}
