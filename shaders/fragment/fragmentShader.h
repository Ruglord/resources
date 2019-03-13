#version 330 core

out vec4 fragColor;
in vec2 texCoord;
in vec3 shade;
uniform sampler2D sprite;

void main()
{
    vec4 text = texture(sprite,texCoord);
    fragColor = vec4(text.xyz*shade,text.w);

}
