#version 330

layout(location = 0) out vec4 fragmentColor;

uniform vec4 mainColor;

in vec3 nnn;

void main()
{
	fragmentColor = mainColor;
	//fragmentColor = mainColor * (0.3 + max(0, dot(nnn, vec3(0.57, 0.57, 0.57))) * 0.7);
}