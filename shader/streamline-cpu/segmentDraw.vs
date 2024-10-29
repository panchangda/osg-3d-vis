#version 330 core

layout (location = 0) in vec3 Vertex;
layout (location = 1) in vec4 VertexColor;

out vec4 v_color;

uniform mat4 ModelViewProjectionMatrix;

void main(){
	v_color = VertexColor;
	gl_Position =  ModelViewProjectionMatrix * vec4(Vertex, 1.0);
}