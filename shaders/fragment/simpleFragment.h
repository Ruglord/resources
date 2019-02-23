#version 330 core

out vec4 FragColor;

in vec3 shade;

void main()
{

    FragColor = vec4(shade,1);
}
