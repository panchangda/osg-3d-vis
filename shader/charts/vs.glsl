#version 330 core

layout(location = 0) in vec4 pos;
out vec3 Nor;
out float index;
uniform mat4 mvp;

void main()                     
{
    Nor = normalize(pos.rgb);
    index = pos.a;
    gl_Position = mvp * vec4(pos.rgb,1);  
}