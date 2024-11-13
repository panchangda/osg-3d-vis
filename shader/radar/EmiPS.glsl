# version 330

layout(location = 0) out vec4 fragmentColor;

uniform vec4 mainColor;

out vec4 Color;

void main()
{
	Color = mainColor;
}