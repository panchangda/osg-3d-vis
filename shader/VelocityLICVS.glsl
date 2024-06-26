#version 330

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 _texCoord;  

uniform mat4 mvp;
out vec2 texCoord;

void main()
{
    texCoord = _texCoord;
    gl_Position = mvp* vec4(pos, 1.0);
}