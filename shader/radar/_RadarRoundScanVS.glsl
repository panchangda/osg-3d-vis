#version 330 core

layout(location = 0) in vec3 pos;
out vec3 Nor;
uniform mat4 mvp;

void main()                     
{
    Nor = normalize(pos);
    gl_Position = mvp * vec4(pos,1);  // Êä³ö gl_Position
}