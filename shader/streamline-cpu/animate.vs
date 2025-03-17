#version 330 core

layout (location = 0) in vec3 Vertex;
layout (location = 1) in vec4 VertexColor;
layout (location = 2) in float VertexIndex;
layout (location = 3) in float LineLength;
layout (location = 4) in float Dotted;

out vec4 v_color;

uniform mat4 ModelViewProjectionMatrix;
uniform float FrameCounter;

uniform float DecayRate;

void main(){
	v_color = VertexColor;
	float Progress = mod(FrameCounter, LineLength);
	// VertexIndex:[0,4] <=> LineLength = 5
    Progress = int(Progress);
    float DecayTimes = 0.0f;
	if(Progress >= VertexIndex){
	    DecayTimes = Progress - VertexIndex;
	}else{
        DecayTimes = LineLength - VertexIndex - Progress;
	}
	v_color.a = pow(DecayRate, DecayTimes);

    if(Dotted == 1.0) v_color.a = 0.0f;

	gl_Position =  ModelViewProjectionMatrix * vec4(Vertex, 1.0);
}