#version 330 core


layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

#define InstanceMAX 100
uniform mat4 offset[InstanceMAX];


// 风的相关参数
uniform float time;         // 时间变量，传入随时间变化的值
uniform vec3 windDirection;


out vec2 texCoord;
out vec3 Nor;
void main(){
    texCoord = TexCoord;
    Nor = Normal;
    // 为每个实例设置不同的位移
    vec3 instancePos = (offset[gl_InstanceID]*vec4( Position ,1.f)).rgb;
//    vec3 instancePos = Position;


    // 越往下越固定
    float heightInfluence = smoothstep(0.0, 1.0, Position.y); // y 值范围在 [0, 1] 之间
    // 模拟草的摆动：根据风方向和时间变化产生位移
    float swayFactor = sin( dot(instancePos.xz, windDirection.xz) + time) * 0.2; // 通过正弦函数产生摆动效果
    // swayFactor = sin(time) * 0.15;
    vec3 sway = vec3( swayFactor * heightInfluence,swayFactor * heightInfluence, 0);  // 摆动不影响z轴方向

    // 更新顶点位置
    vec3 finalPos = instancePos + sway;

    // MVP变换
    vec4 pos = proj * view * model * vec4(finalPos, 1.0);
    gl_Position = pos;
}