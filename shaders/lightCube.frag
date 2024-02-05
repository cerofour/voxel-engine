#version 330 core
out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D texture0;

void main()
{
    FragColor = texture(texture0, texCoord); // set all 4 vector values to 1.0
}