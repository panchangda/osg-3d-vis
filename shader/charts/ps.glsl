#version 330

layout(location = 0) out vec4 fragmentColor;
in vec3 Nor;
flat in int index;


const vec3 light[] = vec3[6](

	vec3(1,0,0),
	vec3(0,1,0),
	vec3(0,0,1),
	vec3(-1,0,0),
	vec3(0,-1,0),
	vec3(0,0,-1)
	
);

const vec3 colors[8] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0),
    vec3(1.0, 1.0, 0.0),
    vec3(0.0, 1.0, 1.0),
    vec3(1.0, 0.0, 1.0),
    vec3(1.0, 1.0, 1.0),
    vec3(0.0, 0.0, 0.0)
);

void main()
{
	vec4 mainColor = vec4(colors[index],0.6);


	fragmentColor = mainColor ;
}