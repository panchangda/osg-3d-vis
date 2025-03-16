uniform mat4 mvp;

vec3 cartesianToSpherical(vec3 cartesian) {
    float x = cartesian.x;
    float y = cartesian.y;
    float z = cartesian.z;

    float r = length(cartesian);

    float phi = atan(y, x);


    float theta = acos(z / r);

    return vec3(r, phi, theta);
}

vec3 sphericalToCartesian(vec3 spherical) {
    float r = spherical.x;
    float phi = spherical.y;
    float theta = spherical.z;

    float x = r * sin(theta) * cos(phi);
    float y = r * sin(theta) * sin(phi);
    float z = r * cos(theta);
    return vec3(x, y, z);
}

uniform vec2 phiThetaSpeed;
uniform float t;

void main()             
{
    vec3 rpt = cartesianToSpherical(gl_Vertex.xyz);
    rpt.y += phiThetaSpeed.x * t ;
    rpt.z += phiThetaSpeed.y * t ;
    vec3 p = sphericalToCartesian(rpt);
    gl_Position = mvp * vec4(p, 1.0f);
}
