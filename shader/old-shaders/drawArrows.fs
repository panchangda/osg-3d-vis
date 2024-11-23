#version 330 core

in vec4 interpColor;

out vec4 FragColor;

void main() {
    vec4 white = vec4(1.0);
    FragColor = interpColor;
}