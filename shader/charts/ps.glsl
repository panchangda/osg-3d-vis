#version 330

layout(location = 0) out vec4 fragmentColor;
in vec3 Nor;
in float index;


const vec3 light[] = vec3[6](

	vec3(1,0,0),
	vec3(0,1,0),
	vec3(0,0,1),
	vec3(-1,0,0),
	vec3(0,-1,0),
	vec3(0,0,-1)
	
);

const vec3 colors[8] = vec3[](
    vec3(1.0, 0.0, 0.0),  // ��ɫ
    vec3(0.0, 1.0, 0.0),  // ��ɫ
    vec3(0.0, 0.0, 1.0),  // ��ɫ
    vec3(1.0, 1.0, 0.0),  // ��ɫ
    vec3(0.0, 1.0, 1.0),  // ��ɫ
    vec3(1.0, 0.0, 1.0),  // Ʒ��
    vec3(1.0, 1.0, 1.0),  // ��ɫ
    vec3(0.0, 0.0, 0.0)   // ��ɫ
);

void main()
{
	vec4 mainColor = vec4(colors[int(index)],0.4);
	vec4 outColor = vec4(0.f);
	for(int i=0; i<6; ++i){
		vec3 l = normalize(light[i]);
		if( dot(Nor,l) > 0){
			outColor += mainColor *dot(Nor,l);
		}

	}

	fragmentColor = outColor ;
}