#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in float ind;
out vec3 Nor;
flat out float index;
uniform mat4 mvp;

void main()                     
{
    Nor = normalize(pos.rgb);
    index = ind;
    gl_Position = mvp * vec4(pos.rgb,1);  
}