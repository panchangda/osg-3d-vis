#version 330 core
#extension GL_EXT_frag_depth : enable

in vec2 v_textureCoordinates;
out vec4 FragColor;

uniform sampler2D colorTexture;
uniform sampler2D depthTexture;


void main(){
    FragColor = texture(colorTexture, v_textureCoordinates);
    gl_FragDepthEXT = texture(depthTexture, v_textureCoordinates).r;
    // FragColor = vec4(segmentDepth, 0.0, 0.0, 1.0);
    // FragColor = screenColor;
}