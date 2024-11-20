# version 330

layout(location = 0) in vec3 pos;

uniform mat4 mvp;

// ���룺�ѿ������� (x, y, z)
vec3 cartesianToSpherical(vec3 cartesian) {
    float x = cartesian.x;
    float y = cartesian.y;
    float z = cartesian.z;

    // ����뾶 r
    float r = length(cartesian); // ��Ч�� sqrt(x*x + y*y + z*z)

    // ���㷽λ�� �գ�ʹ�� atan ����ȷ��������
    float phi = atan(y, x); // ���ػ���

    // ���㸩���� ��
    float theta = acos(z / r); // ���ػ��ȣ�z / r ȷ���� [-1, 1] ��Χ��

    // ���ؽ����vec3(r, phi, theta)
    return vec3(r, phi, theta);
}

// ���룺������ (r, phi, theta)
vec3 sphericalToCartesian(vec3 spherical) {
    float r = spherical.x;
    float phi = spherical.y;
    float theta = spherical.z;

    // ����ѿ�������
    float x = r * sin(theta) * cos(phi);
    float y = r * sin(theta) * sin(phi);
    float z = r * cos(theta);

    // ���ؽ����vec3(x, y, z)
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
