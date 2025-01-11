#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in float ind;
out vec3 Nor;
flat out int index;
uniform mat4 mvp;

void main()                     
{
    Nor = normalize(pos.rgb);
    index = int(ind);
    gl_Position = mvp * vec4(pos.rgb,1);  
}