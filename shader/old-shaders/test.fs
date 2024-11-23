#version 330 core

in vec2 v_textureCoordinates;
out vec4 FragColor;

uniform sampler2D texture0;
uniform sampler2D texture1;

void main(){
	vec4 c0 =  texture(texture0, v_textureCoordinates);
    vec4 c1 =  texture(texture1, v_textureCoordinates);
	FragColor = c0 == c1 ? vec4(1.0) : vec4(0.0, 0.0, 0.0, 1.0);
}