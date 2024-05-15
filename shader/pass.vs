#version 330 core

layout (location = 0) in vec3 Vertex;
layout (location = 1) in vec3 TexCoord;

out vec3 outTexCoord;

uniform mat4 ModelViewProjectionMatrix;

void main(){
	outTexCoord = TexCoord;
	gl_Position =  ModelViewProjectionMatrix * vec4(Vertex, 1.0);
}