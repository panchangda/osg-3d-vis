varying vec3 Nor;
uniform vec4 mainColor;

const vec3 light[] = vec3[4](
	vec3(1,1,1),
	vec3(-1,-1,-1),
	vec3(1,-1,1),
	vec3(-1,1,-1)
);
void main()
{
	vec4 outColor = vec4(0.f);
	for(int i=0; i<4; ++i){
		vec3 l = normalize(light[i]);
		if( dot(Nor,l) > 0.f){
			outColor += mainColor *dot(Nor,l);
		}

	}
	gl_FragColor = outColor;
}
