#version 330 core

layout (location = 0) in vec4 values;
layout (location = 2) in vec3 color;
layout (location = 3) in mat4 transform;
layout (location = 7) in vec4 portion;
uniform mat4 projection;

out vec2 texCoord;
out vec3 shade;


void main()
{

    gl_Position = projection*transform*vec4(values.xy,0,1);
    texCoord = vec2(portion.x + values.b*portion.z,portion.y + values.a*portion.a);
    shade = color;

}
