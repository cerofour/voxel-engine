#version 330 core
 
in vec3 ourPos;
in vec3 outColor;
out vec3 FragColor; // output a color to the fragment shader

void main()
{
    //FragColor = vec3(1.0f, 1.0f, 1.0f);
    FragColor = outColor + vec3(0.5f);
}