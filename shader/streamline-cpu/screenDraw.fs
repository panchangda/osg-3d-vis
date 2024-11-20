#version 330 core

in vec2 v_textureCoordinates;
out vec4 FragColor;

// uniform sampler2D screenColorTexture;
uniform sampler2D screenDepthTexture;
uniform sampler2D streamlineColorTexture;
uniform sampler2D streamlineDepthTexture;

void main(){
	// vec4 screenColor =  texture(screenColorTexture, v_textureCoordinates);
    vec4 streamlineColor =  texture(streamlineColorTexture, v_textureCoordinates);
    float screenDepth = texture(screenDepthTexture, v_textureCoordinates).r;
    float streamlineDepth = texture(streamlineDepthTexture, v_textureCoordinates).r;

	// FragColor = vec4(1.0, 0.0, 0.0, 1.0); return ;

    if(streamlineDepth < screenDepth){
        FragColor =  streamlineColor;
		// FragColor = vec4(1.0, 0.0, 0.0, 0.9);
    }
    else{
        discard;
    }
   

  
	// FragColor = screenColor;

	// FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    // FragColor = streamlineColor;
    // FragColor = vec4(streamlineColor.a, 0.0, 0.0, 1.0);
    // FragColor = screenColor;
    // FragColor = vec4(screenDepth, screenDepth, screenDepth, 1.0);
	// FragColor = vec4(streamlineDepth, streamlineDepth, streamlineDepth, 1.0);

    // FragColor = streamlineColor;
    // FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    // FragColor = segmentColor;
    // FragColor = vec4(segmentDepth);
    // FragColor = segmentColor;
    // FragColor = vec4(segmentDepth, 0.0, 0.0, 1.0);
    // FragColor = screenColor;
}
