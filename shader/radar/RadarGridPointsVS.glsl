# version 330

layout(location = 0) in vec3 pos;

uniform mat4 mvp;

out float dvalue;

void main()                     
{
	dvalue = pos.z;
    gl_Position = mvp * vec4(pos.x, 0, pos.y, 1.0);
}