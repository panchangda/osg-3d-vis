#version 330 core


uniform sampler2D colorTex;
uniform sampler2D depthTex;

in vec2 fragTexCoord;
out vec4 fragColor;

void main() {
    float depthValue = texture(depthTex, fragTexCoord).r;  // �����������
    float currentDepth = gl_FragCoord.z;  // ��ǰƬ�ε����

    // ��ȱȽ�
    if (currentDepth < depthValue) {
        fragColor = texture(colorTex, fragTexCoord);  // ʹ����ɫ�������ɫ
    } else {
        discard;  // ����Ƭ�Σ�������
    }
}
