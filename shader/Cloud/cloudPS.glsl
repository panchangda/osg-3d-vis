#version 450

layout(location = 0) in vec2 fragTexCoord;
layout(location = 0) out vec4 outColor;

uniform float u_time;

// 用于模糊的偏移数组
const vec2 offsets[9] = vec2[](
    vec2(-1.0,  1.0), vec2(0.0,  1.0), vec2(1.0,  1.0),
    vec2(-1.0,  0.0), vec2(0.0,  0.0), vec2(1.0,  0.0),
    vec2(-1.0, -1.0), vec2(0.0, -1.0), vec2(1.0, -1.0)
);

// 权重数组，用于实现高斯模糊
const float weights[9] = float[](
    0.05, 0.1, 0.05,
    0.1,  0.4, 0.1,
    0.05, 0.1, 0.05
);

float hash(float n) {
    return fract(sin(n) * 43758.5453);
}

float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    float a = hash(i.x + i.y * 57.0);
    float b = hash(i.x + 1.0 + i.y * 57.0);
    float c = hash(i.x + (i.y + 1.0) * 57.0);
    float d = hash(i.x + 1.0 + (i.y + 1.0) * 57.0);

    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(a, b, u.x) +
           (c - a) * u.y * (1.0 - u.x) +
           (d - b) * u.x * u.y;
}

float fbm(vec2 p) {
    float value = 0.0;
    float amplitude = 0.5;
    for (int i = 0; i < 5; i++) {
        value += amplitude * noise(p);
        p = p * 2.0 + u_time * 0.1; // 使用时间来平移噪声
        amplitude *= 0.5;
    }
    return value;
}

void main() {
    vec2 uv = fragTexCoord * 5.0; // 调整缩放因子以控制云的密度
    float cloudValue = fbm(uv);
    float alpha = smoothstep(0.5, 1.0, cloudValue); // 控制透明度阈值

    // 计算模糊效果
    vec4 colorSum = vec4(0.0);
    for (int i = 0; i < 9; i++) {
        vec2 offsetUV = uv + offsets[i] * 0.002; // 控制模糊半径
        float sampleValue = fbm(offsetUV);
        float sampleAlpha = smoothstep(0.5, 1.0, sampleValue);
        colorSum += vec4(vec3(1.0), sampleAlpha) * weights[i];
    }

    outColor = colorSum; // 模糊后的云效果
}
