#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 uvs;

out vec3 WorldPos;
out vec2 uv;

uniform mat4 mvp;



void main()
{
    WorldPos = aPos;
    uv = uvs;

    gl_Position =  mvp * vec4(aPos, 1.0);
}