#version 330 core

in vec2 v_textureCoordinates;
out vec4 FragColor;

uniform sampler2D blendedTex;

void main(){
	vec4 color =  texture(blendedTex, v_textureCoordinates);
	FragColor = color;
}