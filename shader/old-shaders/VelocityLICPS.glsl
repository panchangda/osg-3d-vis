#version 330

in vec2 texCoord;
out vec4 fragmentColor;
uniform sampler2D texLIC;

void main()
{
    fragmentColor = texture(texLIC, texCoord);
}