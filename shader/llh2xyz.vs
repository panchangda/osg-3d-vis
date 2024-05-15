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
    outTexCoord = TexCoord;
	
    float hDlt = maxHeight - minHeight;
    float latDlt = maxLatitute - minLatitute;
    float lonDlt = maxLongtitute - minLongtitute;

    float latitude = minLatitute + latDlt * Vertex.y;
    float longitude = minLongtitute + lonDlt * Vertex.x;
    float height = minHeight + hDlt * Vertex.z;
		
    vec4 pos;

    float _radiusEquator = 6378137.0;
    float _radiusPolar = 6356752.3142;
    float _flattening    = (_radiusEquator - _radiusPolar)/_radiusEquator;
    float _eccentricitySquared = 2*_flattening - _flattening*_flattening;

    float sin_latitude = sin(latitude);
    float cos_latitude = cos(latitude);
    float N = _radiusEquator / sqrt( 1.0 - _eccentricitySquared*sin_latitude*sin_latitude);

    pos.x = (N + height)*cos_latitude*cos(longitude);
    pos.y = (N + height)*cos_latitude*sin(longitude);
    pos.z = (N *(1-_eccentricitySquared)+height)* sin_latitude;
    pos.w = 1.0;

    gl_Position =  ModelViewProjectionMatrix * pos;
}
