#version 330 core
#extension GL_ARB_separate_shader_objects : enable


layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;


out vec2 fragTexCoord;
void main(){
	gl_Position = vec4(Position,1);
	fragTexCoord = TexCoord;
}
