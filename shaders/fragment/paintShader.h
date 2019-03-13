#version 330 core

out vec4 fragColor;
in vec2 texCoord;

uniform vec3 shade;
uniform sampler2D sprite;

void main()
{

    fragColor = texture(sprite,texCoord);
    if (fragColor.a > 0)
    {
        fragColor = vec4(shade,1);
    }
}
