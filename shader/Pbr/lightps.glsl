
#version 330 core

//in vec2 TexCoords;
in vec3 WorldPos;
in vec2 uv;
in vec3 Nor;

uniform sampler2D alb1;
uniform sampler2D Nor1;
uniform sampler2D Rou1;

uniform sampler2D alb2;
uniform sampler2D Nor2;
uniform sampler2D Rou2;


uniform int index;

uniform vec3 camPos;

out vec4 FragColor;

const float PI = 3.14159265359;

uniform float theta;

const vec4 lights[12] = vec4[12](
	vec4(-1,0,0,0),
	vec4(1,0,0,0),
	vec4(0,0,-1,0),
	vec4(0,0,1,0),
	vec4(0,1,0,0),
	vec4(0,-1,0,0),
	vec4(-1,0,1,0),
	vec4(1,0,-1,0),
	vec4(0,1,-1,0),
	vec4(0,-1,1,0),
	vec4(0.433737 ,-0.751254, 0.497484,0),
	vec4(-0.433737 ,0.751254, -0.497484,0)
);

float D_GGX(float NoH, float a) {
    float a2 = a * a;
    float f = (NoH * a2 - NoH) * NoH + 1.0;
    return a2 / (PI * f * f);
}

vec3 F_Schlick(float u, vec3 f0) {
    return f0 + (vec3(1.0) - f0) * pow(1.0 - u, 5.0);
}

float V_SmithGGXCorrelated(float NoV, float NoL, float a) {
    float a2 = a * a;
    float GGXL = NoV * sqrt((-NoL * a2 + NoL) * NoL + a2);
    float GGXV = NoL * sqrt((-NoV * a2 + NoV) * NoV + a2);
    return 0.5 / (GGXV + GGXL);
}



mat3 rotationZ(float theta) {
    float c = cos(theta);
    float s = sin(theta);
    return mat3(
        c, -s, 0,
        s,  c, 0,
        0,  0, 1
    );
}
vec3 HDRtoLDR(vec3 color) {
    // 1. Tone mapping (Reinhard tone mapping as an example)
    color = color / (color + vec3(1.0));

    // 2. Gamma correction
    color = pow(color, vec3(1.0 / 2.2));

    return color;
}
// ----------------------------------------------------------------------------
void main()
{		  
	if( index == 1){
		vec3 albedo = texture(alb1,uv).rgb;
		vec3 inNormal = Nor + texture(Nor1,uv).rgb;
		vec3 n = normalize(inNormal);
		vec3 v = normalize(camPos - WorldPos);
		vec3 h = normalize(n+v);
		float ao = texture(Rou1,uv).r;
		float roughness = texture(Rou1,uv).g;
		float metallic = texture(Rou1,uv).b*3;
		float NoV = abs(dot(n, v)) + 1e-5;
		float NoH = clamp(dot(n, h), 0.0, 1.0);

		vec3 F0 = vec3(mix(vec3(0.04), albedo , metallic));

		// Specular contribution
		vec3 Lo = vec3(0);
		for (int i = 0; i < 12; i++) {
			vec3 l = lights[i].xyz * rotationZ(theta);
			float NoL = clamp(dot(n, l), 0.0, 1.0);
			if (NoL > 0.0){
				float LoH = clamp(dot(l, h), 0.0, 1.0);
				float D = D_GGX(NoH, roughness);
				vec3  F = F_Schlick(LoH, F0);
				float V = V_SmithGGXCorrelated(NoV, NoL, roughness);
					// specular BRDF
				vec3 Fr = (D * V) * F *2;

				// diffuse BRDF
				vec3 Fd = albedo * (1-F) /PI * ao;
				Lo += (Fr+Fd) * NoL;
			}
		};

		FragColor = vec4(Lo,1);
	}else{
		vec3 albedo = texture(alb2,uv).rgb;
		vec3 inNormal =  Nor +  texture(Nor2,uv).rgb;
		vec3 n = normalize(inNormal);
		vec3 v = normalize(camPos - WorldPos);
		vec3 h = normalize(n+v);
		float ao = texture(Rou2,uv).r;
		float roughness = texture(Rou2,uv).g;
		float metallic = texture(Rou2,uv).b;
		float NoV = abs(dot(n, v)) + 1e-5;
		float NoH = clamp(dot(n, h), 0.0, 1.0);
		vec3 F0 = vec3(mix(vec3(0.04), albedo , metallic));

		// Specular contribution
		vec3 Lo = vec3(0);
		for (int i = 0; i < 6; i++) {
			vec3 l =  rotationZ(theta) * -lights[i].xyz ;
			float NoL = clamp(dot(n, l), 0.0, 1.0);
			if (NoL > 0.0){
				float LoH = clamp(dot(l, h), 0.0, 1.0);
				float D = D_GGX(NoH, roughness);
				vec3  F = F_Schlick(LoH, F0);
				float V = V_SmithGGXCorrelated(NoV, NoL, roughness);
					// specular BRDF
				vec3 Fr = (D * V) * F *2;

				// diffuse BRDF
				vec3 Fd = albedo * (1-F) /PI * ao;
				Lo += (Fr+Fd) * NoL;
			}
		};

		FragColor = vec4(HDRtoLDR(Lo),1);
	}
}