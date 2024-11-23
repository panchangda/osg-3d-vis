#version 330 core
layout(location = 0) in vec2 texCoord;
layout(location = 1) in float sign;

uniform sampler2D prevParticlesPosition;
uniform sampler2D currentParticlesPosition;
uniform sampler2D nextParticlesPosition;
uniform sampler2D particleSpeed;

uniform float hueOffset;
uniform float hueRange;
uniform float S;
uniform float V;
uniform float Opacity;

uniform int curveAlgorithm;
#define STRAIGHT_LINE 0
#define CIRCULAR_ARC 1
#define BERZIER_CURVE 2

const float M_PI = 3.1415926;

out vec4 interpColor;

float getHueByVector(vec2 dir){
	float hue = (atan(dir.y, dir.x) / 3.1415926 + 1) * 180.0;
    return mod(hue * hueRange / 360.0 + hueOffset, 360.0);
}
vec3 hsvToRgb(vec3 hsv){
		int H = (int(hsv.x)) % 360;
		float S = hsv.y;
		float V = hsv.z;
		int Hi = (H / 60) % 6;
		float f = (H / 60.0) - Hi;
		float p = V * (1 - S);
		float q = V * (1 - f * S);
		float t = V * (1 - (1 - f) * S);
		float R, G, B;
		switch (Hi) {
		case 0:
			R = V; G = t; B = p; break;
		case 1:
			R = q; G = V; B = p; break;
		case 2:
			R = p; G = V; B = t; break;
		case 3:
			R = p; G = q; B = V; break;
		case 4:
			R = t; G = p; B = V; break;
		case 5:
			R = V; G = p; B = q; break;
		}
		return vec3(R, G, B);
}

vec4 getColorBySpeed(vec4 speed){
    float H = getHueByVector(speed.xy);
    vec3 hsv = vec3(H,S,V);
    vec3 rgb = hsvToRgb(hsv);
    return vec4(rgb, Opacity);
}

vec4 constructStraightLine(float sign, vec4 prevPosition, vec4 currentPosition, vec4 nextPosition){
    if(sign == 0.0){
        return vec4(currentPosition.rg, 0, 1.0);
    }
    else{
        return vec4(nextPosition.rg, 0, 1.0);
    }
}
vec4 constructLinearLine(float sign, vec4 prevPosition, vec4 currentPosition, vec4 nextPosition){
    if(sign == 0.0){
        return vec4(currentPosition.rg, 0, 1.0);
    }
    else{
        float t = sign;
        vec2 p0 = prevPosition.xy;
        vec2 p1 = nextPosition.xy;
        vec2 p = (1-t)*p0 + t*p1;
        return vec4(p.xy, 0.0, 1.0);
    }
}
vec4 constructBezierCurve(float sign, vec4 prevPosition, vec4 currentPosition, vec4 nextPosition){
    if(sign == 0.0){
        return vec4(prevPosition.xy, 0, 1.0);
    }else if (sign == 1.0){
        return vec4(nextPosition.xy, 0, 1.0);
    }else{
        float t = sign;
        float u = 1.0 - t;
        float tt = t * t;
        float uu = u * u;
        float ut = u * t;
        
        vec2 p0 = prevPosition.xy;
        vec2 p1 = currentPosition.xy;
        vec2 p2 = nextPosition.xy;
        vec2 p;
        p.x = uu * p0.x + 2 * ut * p1.x + tt * p2.x;
        p.y = uu * p0.y + 2 * ut * p1.y + tt * p2.y;
        return vec4(p.xy, 0, 1.0);
    }
}
vec4 constructCircularArcSpline(float sign, vec4 prevPosition, vec4 currentPosition, vec4 nextPosition){
    if(sign == 0.0){
        return vec4(prevPosition.xy, 0, 1.0);
    }else if (sign == 1.0){
        return vec4(nextPosition.xy, 0, 1.0);
    }else{
        float h = 0.5;
        vec2 A = prevPosition.xy; 
        vec2 B = nextPosition.xy;
        vec2 midpoint = vec2((A.x + B.x) / 2, (A.y + B.y) / 2);
        vec2 perpendicular = vec2(-(B.y - midpoint.y), (B.x - midpoint.x));
        vec2 H = midpoint + h * perpendicular;
        vec2 center = midpoint - perpendicular / h;
        float radius = sqrt(pow(A.x - center.x, 2) + pow(A.y - center.y, 2));
        float startAngle = atan(A.y - center.y, A.x - center.x);
        float endAngle = atan(B.y - center.y, B.x - center.x);
        float angleDelta = endAngle - startAngle;

        if (angleDelta * h > 0) {
            angleDelta -= (h > 0) ? 2 * M_PI : -2 * M_PI;
        }
        float t = sign;
        float angle = startAngle + angleDelta * t;
        float x = center.x + radius * cos(angle);
        float y = center.y + radius * sin(angle);
        return vec4(x, y, 0, 1.0);
    }
}
void main() {
    vec4 prevPosition = texture(prevParticlesPosition, texCoord);
    vec4 currentPosition = texture(currentParticlesPosition, texCoord);
    vec4 nextPosition = texture(nextParticlesPosition, texCoord);
    vec4 speed = texture(particleSpeed, texCoord);

    switch(curveAlgorithm){
        case STRAIGHT_LINE:
            gl_Position = constructStraightLine(sign, prevPosition, currentPosition, nextPosition);   
            break;
        case CIRCULAR_ARC:
            gl_Position = constructCircularArcSpline(sign, prevPosition, currentPosition, nextPosition);   
            break;
        case BERZIER_CURVE:
            gl_Position = constructBezierCurve(sign, prevPosition, currentPosition, nextPosition);   
            break;
    }

	
    if(currentPosition.a == 0.0 || nextPosition.a == 0.0){
        gl_Position = vec4(-2.0, -2.0, 0.0, 1.0);
        interpColor = vec4(0.0);
    }    
    else{
        gl_Position = vec4( gl_Position.rg*2 - 1, 0.0, 1.0);
        interpColor = getColorBySpeed(speed);
    }	
	




}