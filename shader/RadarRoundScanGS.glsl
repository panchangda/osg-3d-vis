#version 330

layout(lines) in;
layout(triangle_strip, max_vertices = 42) out;

uniform mat4 mvp;
uniform float stepAngle;
uniform float startAngle;
uniform vec3 llh_lower;
uniform vec3 llh_upper;

out vec3 nnn;


float M_PI = 3.141592654;
//float _radiusEquator  = 6378137.0;
//float _radiusPolar = 6356752.3142;
float _radiusEquator  = 100;
float _radiusPolar = 100;
float flattening = (_radiusEquator-_radiusPolar)/_radiusEquator;
float _eccentricitySquared = 2*flattening - flattening*flattening;

vec4 llh2xyz_Ellipsoid(vec3 llh_lower, vec3 llh_upper, float _lat, float _lon, float _h) 
{
	float latDlt = llh_upper.x - llh_lower.x;
	float lonDlt = llh_upper.y - llh_lower.y;
	float hDlt = llh_upper.z - llh_lower.z;

	float lat = llh_lower.x + latDlt * _lat;
	float lon = llh_lower.y + lonDlt * _lon;
	float h = llh_lower.z + hDlt * _h;
	
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
	vec4 v0 = gl_in[0].gl_Position;
	vec4 v1 = gl_in[1].gl_Position;
	//vec3 llh_lower = vec3(29.0, 299.0, 0.0);
	//vec3 llh_upper = vec3(31.0, 301.0, 10.0);//maxX
	for(int i = 0; i < 21; i++)
	{
		float curangle = startAngle + stepAngle * i;
		float sinthita = sin(curangle);
		float costhita = cos(curangle);
		vec4 v00 = vec4(v0.x * costhita, v0.x * sinthita, v0.y, 1.0);
		v00 = llh2xyz_Ellipsoid(llh_lower, llh_upper, v00.x, v00.y, v00.z);
		gl_Position = mvp * v00;
		//gl_Position = llh2xyz_Ellipsoid(llh_lower, llh_upper, gl_Position.x, gl_Position.y, gl_Position.z);
		nnn = vec3(v0.z* costhita, v0.z * sinthita, v0.w);
		EmitVertex();
	
		vec4 v11 = vec4(v1.x * costhita, v1.x * sinthita, v1.y, 1.0);
		v11= llh2xyz_Ellipsoid(llh_lower, llh_upper, v11.x, v11.y, v11.z);
		gl_Position = mvp * v11;
		//gl_Position = llh2xyz_Ellipsoid(llh_lower, llh_upper, gl_Position.x, gl_Position.y, gl_Position.z);
		nnn = vec3(v1.z * costhita, v1.z * sinthita, v1.w);
		EmitVertex();
	}
}