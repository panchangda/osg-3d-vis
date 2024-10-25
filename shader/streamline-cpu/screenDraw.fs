#version 330 core

in vec2 v_textureCoordinates;
out vec4 FragColor;

uniform sampler2D screenColorTexture;
uniform sampler2D screenDepthTexture;
uniform sampler2D segmentColorTexture;
uniform sampler2D segmentDepthTexture;

void main(){
	vec4 screenColor =  texture(screenColorTexture, v_textureCoordinates);
    vec4 segmentColor =  texture(segmentColorTexture, v_textureCoordinates);
    float screenDepth = texture(screenDepthTexture, v_textureCoordinates).r;
    float segmentDepth = texture(segmentDepthTexture, v_textureCoordinates).r;


    if(segmentDepth < 1.0)
	    FragColor = segmentColor;
    else
        FragColor = screenColor;


    FragColor = segmentColor;
    // FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    // FragColor = segmentColor;
    // FragColor = vec4(segmentDepth);
    // FragColor = segmentColor;
    // FragColor = vec4(segmentDepth, 0.0, 0.0, 1.0);
    // FragColor = screenColor;


}