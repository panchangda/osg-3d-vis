#version 330 core

layout (location = 0) in vec3 Vertex;
layout (location = 1) in vec3 TexCoord;

out vec3 outTexCoord;

uniform mat4 ModelViewProjectionMatrix;

uniform float minLatitute;
uniform float maxLatitute;
uniform float minLongtitute;
uniform float maxLongtitute;
uniform float minHeight;
uniform float maxHeight;

void main()
{
    outTexCoord = Vertex;
	
    float hDlt = maxHeight - minHeight;
    float latDlt = maxLatitute - minLatitute;
    float lonDlt = maxLongtitute - minLongtitute;

    float lat = minLatitute + latDlt * Vertex.y;
    float lon = minLongtitute + lonDlt * Vertex.x;
    float h = minHeight + hDlt * Vertex.z;

    vec4 pos;
    pos.z = h * sin(lat);
    float r = h * cos(lat);
    pos.x = r * cos(lon);
    pos.y = r * sin(lon);
    pos.w = 1.f;

    gl_Position =  ModelViewProjectionMatrix * pos;
}
