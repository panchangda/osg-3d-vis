#version 330 core

in vec2 v_textureCoordinates;

out vec4 FragColor;

uniform sampler2D lastFrameTex;
uniform sampler2D lineTex;
uniform float decayRate;

void main(){
	vec4 c1 = texture(lastFrameTex, v_textureCoordinates);
	vec4 c2 = texture(lineTex, v_textureCoordinates);
	FragColor = c1*decayRate + c2;
}