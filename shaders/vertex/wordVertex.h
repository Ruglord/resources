#version 330 core

layout (location = 0) in vec4 values;

uniform mat4 projection;

out vec2 texCoord;
out vec3 shade;

void main()
{
    gl_Position =projection*vec4(values.xy,0,1);
    texCoord = values.ba;

}
