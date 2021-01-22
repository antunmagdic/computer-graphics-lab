#version 410 core

in vec3 position;

uniform int max_iter;
uniform vec2 c;
uniform vec2 center;
uniform float scale;

vec4 p1 = vec4(0.24, 0.12, 0.55, 1.0);
vec4 p2 = vec4(0.03, 0.07, 0.7, 1.0);
vec4 p3 = vec4(0.9, 0.35, 0.19, 1.0);
vec4 p4 = vec4(0.0, 1.0, 1.0, 1.0);
float w = 0.5;
float gamma = 0.01;

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
        color = 0.25 * (p1 + p2 + p3 + p4);
    } else {
        if (i == 0) {
            color = vec4(0.0, 0.0, 0.2, 1.0);
        } else {
            float t = float(i);
            float a = exp(-gamma * t);
            float x = (a * cos(w * t) + 1) / 2;
            float y = (a * sin(w * t) + 1) / 2;
            color = (1-x)*(1-y)*p4 + x*(1-y)*p1 + (1-x)*y*p3 + x*y*p2;
        }
    }
}
