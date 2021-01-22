#version 330 core

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;

out vec3 position;
out vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = model * vec4(in_position, 1.0);
    position = gl_Position.xyz / gl_Position.w;
    gl_Position = projection * view * gl_Position;
    normal = mat3(transpose(inverse(model))) * in_normal;
}
