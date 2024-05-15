#version 330 core
layout(location = 0) in vec2 texCoord;
layout(location = 1) in float sign;

void main() {
	if(sign == 0){
		gl_Position = vec4(-1.0, -1.0, 0.0, 1.0);
	}else{
		gl_Position = vec4(1.0, 1.0, 0.0, 1.0);
	}
	
	
}