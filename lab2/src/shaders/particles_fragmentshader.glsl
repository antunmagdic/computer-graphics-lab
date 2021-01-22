#version 330 core
out vec4 FragColor;

in vec2 texture_coordinate;

uniform float life;
uniform sampler2D texture0;

void main()
{
    FragColor = sqrt(sqrt(life)) * texture(texture0, texture_coordinate);
}