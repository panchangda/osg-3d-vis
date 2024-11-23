#version 330 core

smooth in vec3 interpColor;

void main()
{
    gl_FragColor = vec4(interpColor, 1.0);
}