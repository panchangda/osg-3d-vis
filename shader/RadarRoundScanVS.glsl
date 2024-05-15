#version 330

layout(location = 0) in vec2 pos;
//layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 nor;

//out vec3 pos3;

void main()                     
{
    gl_Position = vec4(pos.xy, nor);
//	pos3 = pos;
}