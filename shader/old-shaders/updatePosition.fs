#version 330 core
uniform sampler2D currentParticlesPosition; // (lon, lat, lev)
uniform sampler2D particlesSpeed; // (u, v, w, norm) Unit converted to degrees of longitude and latitude 
uniform sampler2D densityWeightedTileTex1D;
uniform sampler2D informEntropyWeightedTileTex1D;

in vec2 v_textureCoordinates;

uniform float tileXInterval;
uniform float tileYInterval;
uniform float randomCoefficient;
uniform float dropRate;
uniform float dropRateBump;

uniform int randomSeedSelection;
#define GLSL_PSEUDO_RANDOM 0
#define DENSITY_BASED 1
#define INFORMATION_ENTROPY_BASED 2

// pseudo-random generator
const vec3 randomConstants = vec3(12.9898, 78.233, 4375.85453);
const vec2 normalRange = vec2(0.0, 1.0);
float rand(vec2 seed, vec2 range) {
    vec2 randomSeed = randomCoefficient * seed;
    float temp = dot(randomConstants.xy, randomSeed);
    temp = fract(sin(temp) * (randomConstants.z + temp));
    return temp * (range.y - range.x) + range.x;
}
vec2 generateRandomParticle(vec2 seed1, vec2 seed2){
    float randX = rand(seed1, normalRange);
    float randY = rand(-seed2, normalRange);
    return vec2(randX, randY);
}
vec2 generateRandomParticleBasedOnAreaWeight(vec2 seed, vec2 XYOriginal){
    float randX = rand(seed, normalRange);
    float randY = rand(-seed, normalRange);
    return vec2(XYOriginal.x + tileXInterval*randX*10, XYOriginal.y + tileYInterval*randY*10);
}
vec2 getAreaXYOriginal(sampler2D textureToSampleFrom, float randomArea){
    return texture(textureToSampleFrom, vec2(randomArea, 1.0)).xy;
}
void main() {
    // texture coordinate must be normalized
    vec3 currParticle = texture2D(currentParticlesPosition, v_textureCoordinates).rgb;
    vec4 speed = texture2D(particlesSpeed, v_textureCoordinates);

    vec2 seed1 = currParticle.xy + v_textureCoordinates;
    vec2 seed2 = speed.xy + v_textureCoordinates;
    vec2 seed3 = vec2(speed.x, currParticle.x) + v_textureCoordinates;
    vec2 seed4 = vec2(v_textureCoordinates.x + speed.y, v_textureCoordinates.y + currParticle.x);
    float randomArea = rand(seed3, normalRange);
    float randomNumber = rand(seed2, normalRange);
    float particleDropRate = dropRate + dropRateBump*speed.r;

    if(randomNumber < particleDropRate){
		vec2 randomXY;
		if(randomSeedSelection == GLSL_PSEUDO_RANDOM){
			randomXY = generateRandomParticle(seed1, seed4);
		}else if(randomSeedSelection == DENSITY_BASED){
		    vec2 XYOriginal = getAreaXYOriginal(densityWeightedTileTex1D, randomArea);
			randomXY = generateRandomParticleBasedOnAreaWeight(seed1, XYOriginal);
		}else if(randomSeedSelection == INFORMATION_ENTROPY_BASED){
		    vec2 XYOriginal = getAreaXYOriginal(informEntropyWeightedTileTex1D ,randomArea);
			randomXY = generateRandomParticleBasedOnAreaWeight(seed1, XYOriginal);
		}
        gl_FragColor = vec4(randomXY, 0.0, 0.0);
    }
    else if(speed.a == 0.0){
        gl_FragColor = vec4(currParticle.rg, 0.0, 0.0);
    }
    else
    {
        vec3 nextParticle = currParticle + speed.rgb;
        gl_FragColor = vec4(nextParticle.rg, 0.0, 1.0);
    }
}