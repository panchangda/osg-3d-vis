#version 330 core

layout(location = 0) in vec4 pos;

uniform mat4 mvp;

uniform vec4 factor4[3];  

const float globalRadius = 6356752.3142;
uniform bool isDisruption;

void main()                     
{

    vec3 bias = vec3(0.0);  // �� 0.0 ��ʼ��
    for (int i = 0; i < 3; ++i) {
        vec3 diff = pos.xyz - factor4[i].xyz;  // �����ֵ����
        float len = length(diff);  // ����ʹ�� length ���������ĳ���
        


        // �������������ͬʱҲ�Ż��˳���˳��  
        if( len < factor4[i].w ) bias += 1500000000000.0 * normalize(diff) / (len + 1.0);
    }
    vec3 newPos = pos.xyz + bias;

    if (length(newPos) < globalRadius) {
        newPos = normalize(newPos) * globalRadius;  // �� newPos ��һ�������� globalRadius
    }

    if( isDisruption == false) newPos = pos.xyz;
    gl_Position = mvp * vec4(newPos, 1.0);  // ��� gl_Position


}