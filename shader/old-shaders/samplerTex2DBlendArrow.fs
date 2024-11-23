#version 330 core

in vec2 v_textureCoordinates;
out vec4 FragColor;

uniform sampler2D blendedTex;
uniform sampler2D arrowsTex;

void main(){
	vec4 c1 =  texture(blendedTex, v_textureCoordinates);
	vec4 c2= texture(arrowsTex, v_textureCoordinates);
	FragColor = c1 + c2;
}