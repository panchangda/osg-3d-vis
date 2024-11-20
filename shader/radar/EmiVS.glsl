# version 330

layout(location = 0) in vec3 pos;

uniform mat4 mvp;

// 输入：笛卡尔坐标 (x, y, z)
vec3 cartesianToSpherical(vec3 cartesian) {
    float x = cartesian.x;
    float y = cartesian.y;
    float z = cartesian.z;

    // 计算半径 r
    float r = length(cartesian); // 等效于 sqrt(x*x + y*y + z*z)

    // 计算方位角 φ，使用 atan 来正确处理象限
    float phi = atan(y, x); // 返回弧度

    // 计算俯仰角 θ
    float theta = acos(z / r); // 返回弧度，z / r 确保在 [-1, 1] 范围内

    // 返回结果：vec3(r, phi, theta)
    return vec3(r, phi, theta);
}

// 输入：球坐标 (r, phi, theta)
vec3 sphericalToCartesian(vec3 spherical) {
    float r = spherical.x;
    float phi = spherical.y;
    float theta = spherical.z;

    // 计算笛卡尔坐标
    float x = r * sin(theta) * cos(phi);
    float y = r * sin(theta) * sin(phi);
    float z = r * cos(theta);

    // 返回结果：vec3(x, y, z)
    return vec3(x, y, z);
}

uniform vec2 phiThetaSpeed;
uniform float t;

void main()             
{
    vec3 rpt = cartesianToSpherical(pos);
    rpt.y += phiThetaSpeed.x * t* 0 ;
    rpt.z += phiThetaSpeed.y * t * 0 ;
    vec3 p = sphericalToCartesian(rpt);
    gl_Position = mvp * vec4(pos, 1.0);
}
