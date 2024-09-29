#version 330

layout(location = 0) in vec4 pos;
out vec3 nnn;

uniform mat4 mvp;
void main()                     
{
    gl_Position = mvp * pos;
    nnn = pos.xyz;
}