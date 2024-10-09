#version 330

layout(location = 0) out vec4 fragmentColor;

uniform vec4 mainColor;

void main()
{
	fragmentColor = mainColor;
}