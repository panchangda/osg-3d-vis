varying vec3 Nor;

uniform mat4 mvp;

void main()                     
{
    Nor = normalize(gl_Vertex.xyz);
    gl_Position = mvp * vec4(gl_Vertex.xyz,1.f);
}
