#version 330 core


layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

#define InstanceMAX 100
uniform vec3 instancePos[InstanceMAX];

// 风的相关参数
uniform float time;         // 时间变量，传入随时间变化的值
uniform vec3 windDirection;

uniform int instanceX;
uniform int instanceY;

out vec2 texCoord;

void main(){
    texCoord = TexCoord;


    int x = gl_InstanceID / instanceX;
    int y = gl_InstanceID % instanceX;
    vec3 instanceOffset = vec3(x, -1.0, y);
    vec3 instancePos =  Position + instanceOffset;

    // 越往下越固定
    float heightInfluence = smoothstep(0.0, 1.0, Position.y); // y 值范围在 [0, 1] 之间
    // 模拟草的摆动：根据风方向和时间变化产生位移
    float swayFactor = sin( dot(instancePos.xz, windDirection.xz) + time) * 0.2; // 通过正弦函数产生摆动效果
    // swayFactor = sin(time) * 0.15;
    vec3 sway = vec3(swayFactor * heightInfluence, 0.0, swayFactor * heightInfluence);  // 摆动不影响y轴方向

    // 更新顶点位置
    vec3 finalPos = instancePos + sway;

    // MVP变换
    vec4 pos = proj * view * model * vec4(finalPos, 1.0);
    gl_Position = pos;
}
