#version 330 core

layout(location = 0) in vec4 pos;

uniform mat4 mvp;

void main()                     
{
    gl_Position = mvp * vec4(pos);  // Êä³ö gl_Position
}