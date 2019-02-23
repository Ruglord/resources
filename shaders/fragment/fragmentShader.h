#version 330 core

out vec4 fragColor;
in vec2 texCoord;
in vec3 shade;
uniform sampler2D sprite;

void main()
{

    fragColor = texture(sprite,texCoord);
}
