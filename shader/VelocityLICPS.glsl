//#version 330

//in vec2 texCoord;

varying vec2 texCoord;

//out vec4 fragmentColor;
uniform sampler2D texLIC;

void main()
{
    gl_FragColor = texture(texLIC, texCoord);
}
