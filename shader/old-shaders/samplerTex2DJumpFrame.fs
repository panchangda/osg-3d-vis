#version 330 core

in vec2 v_textureCoordinates;
out vec4 FragColor;

uniform sampler2D blendedTex;

uniform bool jumpFrame;

void main(){
if(jumpFrame)
 return;
	vec4 color =  texture(blendedTex, v_textureCoordinates);
	FragColor = color;

}