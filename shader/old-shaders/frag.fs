#version 330 core
out vec4 FragColor;

in vec3 outTexCoord;

uniform sampler3D VolumeTexture;

void main()
{
    vec4 color = texture(VolumeTexture, outTexCoord);
    FragColor = color;
}
