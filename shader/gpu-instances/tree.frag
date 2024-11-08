#version 330 core

uniform sampler2D samplerTexture;

in vec2 texCoord;

out vec4 fragColor;

void main(){
    vec4 color = texture(samplerTexture, vec2(texCoord.x, 1.0-texCoord.y)).rgba;
    color.a-=0.1;
    fragColor = vec4(color.rgba);
    //    fragColor = vec4(texCoord, 0.0, 1.0);
}