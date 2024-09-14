#version 330

in vec2 texCoord;
out vec4 fragmentColor;
uniform sampler2D texLIC;

void main()
{
    fragmentColor = texture(texLIC, texCoord);
	if(fragmentColor.r>0.2&&fragmentColor.b>0.2)
		fragmentColor.a=0;
	else
		fragmentColor.a=0.8;
	if(texCoord.x<0.5||texCoord.y<0.5)
		fragmentColor.a=0;
    //fragmentColor.a = fragmentColor.b-fragmentColor.r;
    //fragmentColor.b = fragmentColor.b-fragmentColor.r;
    //fragmentColor.r = 0;

}