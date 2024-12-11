
#version 330 core
#extension GL_ARB_separate_shader_objects : enable

//in vec2 TexCoords;
in vec3 WorldPos;
in vec2 uv;

uniform sampler2D alb;
uniform sampler2D Nor;
uniform sampler2D Rou;

// material parameters

uniform vec3 F0;
uniform vec3 kd;
uniform vec3 ks;
uniform vec3 ao;


uniform vec3 camPos;

out vec4 FragColor;

const float PI = 3.14159265359;

const vec4 lights[6] = vec4[6](
	vec4(-1,-1,-1,0),
	vec4(1,1,1,0),
	vec4(-1,0,-1,0),
	vec4(1,0,1,0),
	vec4(0,1,1,0),
	vec4(0,-1,-1,0)
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

float Fd_Lambert() {
    return 1.0 / PI;
}


// ----------------------------------------------------------------------------
void main()
{		  
	vec3 albedo = texture(alb,uv).rgb;
	vec3 inNormal = texture(Nor,uv).rgb;
	vec3 n = normalize(inNormal);
	vec3 v = normalize(camPos - WorldPos);
	vec3 h = normalize(n+v);
	float roughness = texture(Rou,uv).r;
	float NoV = abs(dot(n, v)) + 1e-5;
    float NoH = clamp(dot(n, h), 0.0, 1.0);


#ifdef ROUGHNESS_PATTERN
	roughness = max(roughness, step(fract(inWorldPos.y * 2.02), 0.5));
#endif

	// Specular contribution
	vec3 Lo = vec3(0.0);
	for (int i = 0; i < 6; i++) {
		vec3 l = lights[i].xyz;
		float NoL = clamp(dot(n, l), 0.0, 1.0);
		if (NoL > 0.0){
			float LoH = clamp(dot(l, h), 0.0, 1.0);
			float D = D_GGX(NoH, roughness);
			vec3  F = F_Schlick(LoH, F0);
			float V = V_SmithGGXCorrelated(NoV, NoL, roughness);
			    // specular BRDF
			vec3 Fr = (D * V) * F;

			// diffuse BRDF
			vec3 Fd = albedo * Fd_Lambert();
			Lo += Fr+Fd;
		}
	};

	// Combine with ambient
	vec3 color = Lo;

	FragColor = vec4(color,1);
}