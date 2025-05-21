uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

uniform float uTime;
uniform vec4 uEMIParameters;

varying vec4 Color;


// 伪随机函数
float rand(float x) {
    return fract(sin(x) * 43758.5453);
}

float rand(vec3 co) {
    return fract(sin(dot(co, vec3(12.9898, 78.233, 45.164))) * 43758.5453);
}

void main()
{
    vec3 emiPosition = uEMIParameters.xyz; // world space 
    float emiRadius = uEMIParameters.w;
    //float emiRadius = 727018;
    vec3 worldPosition = vec3(uModelMatrix*gl_Vertex);
    vec3 worldCenter = vec3(uModelMatrix*vec4(0,0,0,1));

    vec3 toCenterDir = (worldCenter-worldPosition);
    float toCenterLen = length(toCenterDir);
    toCenterDir = toCenterDir/max(toCenterLen, 1e-5);

    vec3 toEmiDir = emiPosition-worldPosition;
    float toEmiLen = length(toEmiDir);

    toEmiLen = max(toEmiLen, 1e-5);

    if(toEmiLen<emiRadius)
    {
        float intensity = 1.0/(toEmiLen*toEmiLen) * emiRadius/4.0;
        worldPosition += toCenterDir*(min(1.5*(emiRadius-toEmiLen), toCenterLen)); 
    }
    

    float t = uTime;

    Color = gl_Color;
    gl_Position = uProjectionMatrix*uViewMatrix* vec4(worldPosition, 1.0);
   // gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * vec4(gl_Vertex.xyz, 1.0);
}


