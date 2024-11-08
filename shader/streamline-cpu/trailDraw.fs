#version 330 core

#extension GL_EXT_frag_depth : enable

uniform sampler2D trailColorTexture;
uniform sampler2D trailDepthTexture;
uniform sampler2D segmentColorTexture;
uniform sampler2D segmentDepthTexture;

in vec2 v_textureCoordinates;
out vec4 FragColor;

uniform bool first;

uniform float fadeOpacity;

void main(){

	vec4 trailColor =  texture(trailColorTexture, v_textureCoordinates);
    float trailDepth = texture(trailDepthTexture, v_textureCoordinates).r;
    
    vec4 segmentColor =  texture(segmentColorTexture, v_textureCoordinates);
    float segmentDepth = texture(segmentDepthTexture, v_textureCoordinates).r;

    vec4 decayedTrail = trailColor;
    FragColor = vec4(segmentColor.rgb + decayedTrail.rgb, decayedTrail.a);

    FragColor = vec4(decayedTrail.rgb, 1.0);

    if(first){
        gl_FragDepthEXT = segmentDepth;
    }else
    {
        gl_FragDepthEXT = min(trailDepth, segmentDepth);
    }

//     if(gl_FragDepthEXT == trailDepth && FragColor.a <= 0.05){
//         gl_FragDepthEXT = 1.0;
//         FragColor.a = 0.0;
//     }



    // gl_FragDepthEXT = segmentDepth;

    // gl_FragDepthEXT = trailDepth;
    // gl_FragDepthEXT = segmentDepth;
    // gl_FragDepthEXT = trailDepth;
    // FragColor = vec4(segmentDepth, 0.0, 0.0, 1.0);
    // FragColor = screenColor;
}