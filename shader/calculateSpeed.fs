#version 330 core
// the size of UV textures: width = lon, height = lat*lev
uniform sampler2D U; // eastward wind 
uniform sampler2D V; // northward wind
uniform sampler2D currentParticlesPosition; // (lon, lat, lev)

uniform vec3 dimension; // (lon, lat, lev)

// used to calculate the wind norm
uniform float pixelSize;
uniform float speedFactor;

uniform float latStart;
uniform float lonStart;
uniform float latInterval;
uniform float lonInterval;

out vec4 FragColor;

in vec2 v_textureCoordinates;


float getWindComponent(sampler2D componentTexture, vec3 lonLatLev) {
    vec2 wind = texture2D(componentTexture, lonLatLev.rg).rg;
    // check if is land
    if(wind.g == 1.0){
        return 9999.0;
    }
    // scale to original
    return (wind.r - 0.5) * 10.0;
}

float interpolateTexture(sampler2D componentTexture, vec3 lonLatLev) {

    float lon = lonLatLev.x;
    float lat = lonLatLev.y;

    float lon0 = lonStart + floor(lon * dimension.x * lonInterval) / dimension.x;
    float lon1 = lon0 + 1.0 / dimension.x;
    float lat0 = latStart + floor(lat * dimension.y * latInterval) / dimension.y;
    float lat1 = lat0 + 1.0 / dimension.y;
	
	return getWindComponent(componentTexture, vec3(lon1, lat0, 0.0));

    float lon0_lat0 = getWindComponent(componentTexture, vec3(lon0, lat0, 0));
    float lon1_lat0 = getWindComponent(componentTexture, vec3(lon1, lat0, 0));
    float lon0_lat1 = getWindComponent(componentTexture, vec3(lon0, lat1, 0));
    float lon1_lat1 = getWindComponent(componentTexture, vec3(lon1, lat1, 0));
	
    

    if(lon0_lat0==9999.0||lon1_lat0==9999.0||lon0_lat1==9999.0||lon1_lat1==9999.0)
        return 9999.0;

    float lon_lat0 = mix(lon0_lat0, lon1_lat0, lon - lon0);
    float lon_lat1 = mix(lon0_lat1, lon1_lat1, lon - lon0);
    float lon_lat = mix(lon_lat0, lon_lat1, lat - lat0);
    return lon_lat;
}

vec3 linearInterpolation(vec3 lonLatLev) {
    // https://en.wikipedia.org/wiki/Bilinear_interpolation
    float u = interpolateTexture(U, lonLatLev);
    float v = interpolateTexture(V, lonLatLev);

    return vec3(u, v, 0);
}

vec3 calculateSpeedByRungeKutta2(vec3 lonLatLev) {
    // see https://en.wikipedia.org/wiki/Runge%E2%80%93Kutta_methods#Second-order_methods_with_two_stages for detail
    float speedScaleFactor = speedFactor * pixelSize;
    const float h = 0.5;

    vec3 y_n = lonLatLev;
    vec3 f_n = linearInterpolation(y_n);
    if(f_n.x==9999.0 || f_n.y==9999.0)
        return vec3(9999.0);

    vec3 midpoint = y_n + 0.5 * h * f_n * speedScaleFactor;
    vec3 tmp = linearInterpolation(midpoint);

    if(tmp.x==9999.0 || tmp.y==9999.0)
        return vec3(9999.0);

    vec3 speed = h * tmp * speedScaleFactor;

    return speed;
}
vec3 calculateSpeedByForwardEuler(vec3 lonLatLev){
    float speedScaleFactor = speedFactor * pixelSize;
    const float h = 0.4;
    vec3 f_n = linearInterpolation(lonLatLev);
    if(f_n.x==9999.0 || f_n.y==9999.0)
        return vec3(9999.0);
    vec3 speed = h * f_n * speedScaleFactor;
    return speed;  
}

void main() {
    // texture coordinate must be normalized
    vec3 lonLatLev = texture(currentParticlesPosition, v_textureCoordinates).rgb;
    vec3 speed = calculateSpeedByRungeKutta2(lonLatLev);

    if(speed.x == 9999.0 || speed.y==9999.0)
        FragColor = vec4(0.0);
    else
        FragColor = vec4(speed, 1.0);
}
