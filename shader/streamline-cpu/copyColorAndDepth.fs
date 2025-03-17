#version 330 core

in vec2 v_textureCoordinates;
out vec4 FragColor;

uniform sampler2D colorTexture;
uniform sampler2D depthTexture;

uniform bool first;

void main(){
    FragColor = texture(colorTexture, v_textureCoordinates);

    if(first){
        gl_FragDepth  = 1.0;
    }else{
        gl_FragDepth = texture(depthTexture, v_textureCoordinates).r;
    }

    // FragColor = vec4(segmentDepth, 0.0, 0.0, 1.0);
    // FragColor = screenColor;
}