# version 330
uniform vec4 mainColor;

out vec4 Color;

void main()
{
	Color = vec4(mainColor.rgb,1.0f);
}
