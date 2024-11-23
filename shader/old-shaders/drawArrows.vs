#version 330 core
layout(location = 0) in vec2 texCoord;
layout(location = 1) in float sign;

uniform sampler2D prevParticlesPosition;
uniform sampler2D currentParticlesPosition;
uniform sampler2D nextParticlesPosition;
uniform sampler2D particleSpeed;

uniform float arrowLength;

uniform float S;
uniform float V;
uniform float Opacity;


const float M_PI = 3.1415926;

out vec4 interpColor;

vec2 rotate(vec2 v, float theta){
	return vec2( v.x * cos(theta) - v.y * cos(theta), v.x * sin(theta) + v.y * cos(theta));
}

void main(){
    vec4 prevPosition = texture(prevParticlesPosition, texCoord);
    vec4 currentPosition = texture(currentParticlesPosition, texCoord);
    vec4 nextPosition = texture(nextParticlesPosition, texCoord);
    vec4 speed = texture(particleSpeed, texCoord);
	
	if(currentPosition.a == 0.0 || nextPosition.a == 0.0){
        gl_Position = vec4(-2.0, -2.0, 0.0, 1.0);
        interpColor = vec4(0.0, 0.0, 0.0, 0.0);
    }{
		if(sign == 0.0 || sign == 2){
			gl_Position = vec4(nextPosition.rg * 2.0 - 1.0, 0.0, 1.0);
		}
		else if(sign == 1.0){
			vec2 dir = rotate( - normalize(speed.xy), M_PI / 6.0);
			gl_Position = vec4( (nextPosition.rg + dir*arrowLength*0.002 ) * 2.0 - 1.0, 0.0, 1.0);
		}
		else if(sign == 3.0){
			vec2 dir = rotate( - normalize(speed.xy), -M_PI / 6.0);
			gl_Position = vec4( (nextPosition.rg + dir*arrowLength*0.002 ) * 2.0 - 1.0, 0.0, 1.0);
		}
		interpColor = vec4(1.0, 1.0, 1.0, 1.0);
	}
}