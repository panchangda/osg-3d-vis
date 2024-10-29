#version 330 core
#extension GL_EXT_frag_depth : enable

in vec2 v_textureCoordinates;
out vec4 FragColor;

uniform sampler2D trailColorTexture;
uniform sampler2D trailDepthTexture;
uniform sampler2D segmentColorTexture;
uniform sampler2D segmentDepthTexture;


uniform bool first;

uniform float fadeOpacity;

void main(){

	vec4 trailColor =  texture(trailColorTexture, v_textureCoordinates);
    float trailDepth = texture(trailDepthTexture, v_textureCoordinates).r;
    
    vec4 segmentColor =  texture(segmentColorTexture, v_textureCoordinates);
    float segmentDepth = texture(segmentDepthTexture, v_textureCoordinates).r;

    FragColor = vec4(0.0);
    trailColor = floor(fadeOpacity * 255.0 * trailColor) / 255.0;
    FragColor = FragColor + trailColor + segmentColor;
    
    if(first)
        gl_FragDepthEXT = segmentDepth;
    else
        gl_FragDepthEXT = min(segmentDepth, trailDepth);
    // gl_FragDepthEXT = segmentDepth;
    // gl_FragDepthEXT = trailDepth;
    // FragColor = vec4(segmentDepth, 0.0, 0.0, 1.0);
    // FragColor = screenColor;
}