//#version 330
//layout(location = 0) in vec2 pos;

// out vec2 texCoord;
varying vec2 texCoord;

void main()
{
    vec2 pos = gl_Vertex.xy;
    texCoord=(pos+1)/2;
    gl_Position = vec4(pos,0,1.0);
}
