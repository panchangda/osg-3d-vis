#version 330 core

in vec2 v_textureCoordinates;
out vec4 FragColor;

uniform sampler2D screenColorTexture;
uniform sampler2D screenDepthTexture;

void main(){
	vec4 screenColor =  texture(screenColorTexture, v_textureCoordinates);
    float screenDepth = texture(screenDepthTexture, v_textureCoordinates).r;

//     FragColor = screenColor;
    FragColor = vec4(screenDepth, 0.0, 0.0, 1.0);
//     FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    // FragColor = streamlineColor;
    // FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    // FragColor = segmentColor;
    // FragColor = vec4(segmentDepth);
    // FragColor = segmentColor;
    // FragColor = vec4(segmentDepth, 0.0, 0.0, 1.0);
    // FragColor = screenColor;


}