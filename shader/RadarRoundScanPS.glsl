#version 330

layout(location = 0) out vec4 fragmentColor;

uniform vec4 mainColor;

in vec3 nnn;

void main()
{
	fragmentColor = mainColor;
}