#version 330 core

layout(location = 0) in vec4 pos;

uniform mat4 mvp;

uniform vec4 factor4[3];  

const float globalRadius = 6356752.3142;
uniform bool isDisruption;

void main()                     
{

    vec3 bias = vec3(0.0);  // 用 0.0 初始化
    for (int i = 0; i < 3; ++i) {
        vec3 diff = pos.xyz - factor4[i].xyz;  // 计算差值向量
        float len = length(diff);  // 这里使用 length 计算向量的长度
        


        // 避免除零的情况，同时也优化了除法顺序  
        if( len < factor4[i].w ) bias += 1500000000000.0 * normalize(diff) / (len + 1.0);
    }
    vec3 newPos = pos.xyz + bias;

    if (length(newPos) < globalRadius) {
        newPos = normalize(newPos) * globalRadius;  // 将 newPos 归一化并乘以 globalRadius
    }

    if( isDisruption == false) newPos = pos.xyz;
    gl_Position = mvp * vec4(newPos, 1.0);  // 输出 gl_Position


}