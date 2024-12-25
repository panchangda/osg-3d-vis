#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 uvs;
layout(location = 2) in vec3 nor;
out vec3 WorldPos;
out vec2 uv;
out vec3 Nor;
uniform mat4 mvp;



void main()
{
    WorldPos = aPos;
    uv = uvs;
    Nor = nor;
    gl_Position =  mvp * vec4(aPos, 1.0);
}