#version 330 core

uniform sampler2D samplerTexture;

in vec2 texCoord;
in vec3 Nor;

out vec4 fragColor;

const vec3 light[] = vec3[4](
	vec3(1,1,1),
	vec3(-1,-1,-1),
	vec3(1,-1,1),
	vec3(-1,1,-1)
);

void main(){
    vec4 color = texture(samplerTexture, vec2(texCoord.x,texCoord.y)).rgba;
    color.a-=0.1;

	vec4 outColor = vec4(0.f);
	for(int i=0; i<4; ++i){
		vec3 l = normalize(light[i]);
		if( dot(Nor,l) > 0){
			outColor += color *dot(Nor,l);
		}

	}


    fragColor = outColor;

}