#version 330 core

layout(location = 0) out vec4 fragment_color;

in vec3 position;
in vec3 normal;

uniform vec4 object_color;
uniform vec3 light_source;
uniform vec3 light_source_position;
uniform vec3 ambient_light;

void main() {
    vec3 light_dir = normalize(light_source_position - position);
    float diff_intensity = max(dot(normalize(normal), light_dir), 0.0);
    vec4 light_mul = vec4(ambient_light + diff_intensity * light_source, 1.0);
    fragment_color = light_mul * object_color;
}
