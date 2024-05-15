#version 330
out vec4 fragmentColor;
in vec2 texCoord;
uniform sampler2D velocityTex;
uniform sampler2D noiseTex;
uniform vec4 texSize;
uniform int isUniformColor;
uniform int isSingleMoving;
uniform int T;
uniform float phi;
uniform float alpha;

float PI = 3.1415926536;

float getWindowCoef(int i, int isSingleMoving)
{
    if(isSingleMoving == 0)
        return 1;
    return 1 + sin(2 * PI * (i + phi) / T) / 2;
}
// 将所有texture(noiseTex, temptexCoord).x改为windowCoef * texture(noiseTex, temptexCoord).x，实现时变效果

float getHueByVector(vec2 v)
{
    return (atan(v.y, v.x) / PI + 1)*180;
}

vec3 hsvToRgb(vec3 hsv)
{
    int H = (int(hsv.x))%360;
	float S = hsv.y;
	float V = hsv.z;
	int Hi = (H / 60) % 6;
	float f = (H / 60.0) - Hi;
	float p = V * (1 - S);
	float q = V * (1 - f * S);
	float t = V * (1 - (1 - f)* S);
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

void main()
{
    vec2 velocity = vec2(texture(velocityTex, texCoord));

	velocity.x = velocity.x * 2 - 1;
	velocity.y = velocity.y * 2 - 1;
	if(texture(velocityTex, texCoord).z >0.5)discard;

    float windowCoef = getWindowCoef(0, isSingleMoving);
    float Fsum = windowCoef * texture(noiseTex, texCoord).x;
    float count = windowCoef;
    vec2 temptexCoord = texCoord;

    for(int i=0;i<64;i++){
        vec2 dir = texture(velocityTex, temptexCoord).xy;

        dir.x = dir.x * 2 - 1;
        dir.y = dir.y * 2 - 1;
        if(texture(velocityTex, temptexCoord).z >0.5)break;

        dir=normalize(dir);
        temptexCoord+=vec2(dir.x/texSize.x,dir.y/texSize.y);
        if(temptexCoord.x>1||temptexCoord.x<0||temptexCoord.y>1||temptexCoord.y<0) break;
        windowCoef = getWindowCoef(i, isSingleMoving);
        Fsum += windowCoef * texture(noiseTex, temptexCoord).x;
        count += windowCoef;
    }
    
    temptexCoord = texCoord;

    for(int i=0;i<64;i++){
        vec2 dir = texture(velocityTex, temptexCoord).xy;

        dir.x = dir.x * 2 - 1;
        dir.y = dir.y * 2 - 1;
        if(texture(velocityTex, temptexCoord).z >0.5)break;

        dir=normalize(dir);
        temptexCoord-=vec2(dir.x/texSize.x,dir.y/texSize.y);
        if(temptexCoord.x>1||temptexCoord.x<0||temptexCoord.y>1||temptexCoord.y<0) break;
        windowCoef = getWindowCoef(-i, isSingleMoving);
        Fsum += windowCoef * texture(noiseTex, temptexCoord).x;
        count += windowCoef;
    }

    Fsum /= count;

    // Strenthen the line
    //Fsum = Fsum*2-0.5;
    Fsum = Fsum * 4 - 1.25;

    if(isUniformColor != 0)fragmentColor = vec4(vec3(1)*Fsum,1);
    else
    {
        float hue =  getHueByVector(velocity);
        vec3 color = hsvToRgb(vec3(hue,0.75,1));
        fragmentColor = vec4(color*Fsum, alpha);
    }
}