#version 330 core

layout(location = 0) in vec3 Vertex;
layout(location = 1) in vec3 VertColor;

out vec3 interpColor;

void main()
{
    interpColor = VertColor;
    gl_Position = vec4(Vertex.x, Vertex.y, 0.0, 1.0);
}