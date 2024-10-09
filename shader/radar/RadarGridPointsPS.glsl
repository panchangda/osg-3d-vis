# version 330

layout(location = 0) out vec4 fragmentColor;

uniform float curd;

in float dvalue;

void main()
{
	if(dvalue > curd)
		fragmentColor = vec4(78.0/255, 201.0/255, 176.0/255, 1.0);
	else
		discard;//fragmentColor = vec4(0, 0, 1.0, 1.0);
}