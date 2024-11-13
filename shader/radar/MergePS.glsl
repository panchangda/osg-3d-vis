#version 330 core


uniform sampler2D colorTex;
uniform sampler2D depthTex;

in vec2 fragTexCoord;
out vec4 fragColor;

void main() {
    float depthValue = texture(depthTex, fragTexCoord).r;  // 采样深度纹理
    float currentDepth = gl_FragCoord.z;  // 当前片段的深度

    // 深度比较
    if (currentDepth < depthValue) {
        fragColor = texture(colorTex, fragTexCoord);  // 使用颜色纹理的颜色
    } else {
        discard;  // 丢弃片段，不更新
    }
}
