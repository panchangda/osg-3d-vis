# version 330

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 uvs;

out vec2 fragTexCoord;

void main()                     
{
    fragTexCoord = uvs;
    gl_Position = vec4(pos, 1.0);
}