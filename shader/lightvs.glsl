#version 330 core
#extension GL_ARB_separate_shader_objects : enable
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 WorldPos;
out vec3 Normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 normalMatrix;

void main()
{
    WorldPos = vec3(view * model * vec4(aPos, 1.0));
    Normal = (normalMatrix * vec4(aNormal,1.0)).xyz;  

    gl_Position =  projection * view * vec4(WorldPos, 1.0);
}