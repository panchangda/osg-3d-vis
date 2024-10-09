#version 330

//layout(location = 0) in vec2 pos;
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 nor;

uniform mat4 mvp;

out vec3 nnn;

float M_PI = 3.141592654;
float _radiusEquator  = 6378137.0;
float _radiusPolar = 6356752.3142;
//float _radiusEquator  = 255;
//float _radiusPolar = 255;
float flattening = (_radiusEquator-_radiusPolar)/_radiusEquator;
float _eccentricitySquared = 2*flattening - flattening*flattening;

vec4 llh2xyz_Ellipsoid(vec3 llh_lower, vec3 llh_upper, float _lat, float _lon, float _h) 
{
	float latDlt = llh_upper.x - llh_lower.x;
	float lonDlt = llh_upper.y - llh_lower.y;
	float hDlt = llh_upper.z - llh_lower.z;

	float lat = llh_lower.x + latDlt * _lat;
	float lon = llh_lower.y + lonDlt * _lon;
	float h = int(llh_lower.z + hDlt * _h);
	
	float sin_latitude = sin(lat / 180.0 * M_PI);
    	float cos_latitude = cos(lat / 180.0 * M_PI);
    	float N = _radiusEquator / sqrt( 1.0 - _eccentricitySquared*sin_latitude*sin_latitude);
    	float X = (N+h)*cos_latitude*cos(lon / 180.0 * M_PI);
    	float Y = (N+h)*cos_latitude*sin(lon / 180.0 * M_PI);
    	float Z = (N*(1-_eccentricitySquared)+h)*sin_latitude;
	
	return vec4(X,Y,Z,1.0);
}

void main()                     
{
	nnn = vec3(nor.x, 0, nor.y);
	//vec3 llh_lower = vec3(0.0, 0.0, 10.0);
	//vec3 llh_upper = vec3(31.0, 121.0, 20.0);
	//gl_Position = llh2xyz_Ellipsoid(llh_lower, llh_upper, pos.x, 0, pos.y);
	//gl_Position = mvp *gl_Position;
	gl_Position = mvp * vec4(pos, 1.0);
    //gl_Position = mvp * vec4(pos.x, 0, pos.y, 1.0);
}