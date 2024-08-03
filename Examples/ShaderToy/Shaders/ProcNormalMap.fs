//https://www.shadertoy.com/view/4ss3W7
#version 310 es
precision highp float;       	// Set the default precision to medium

uniform float iTime;
uniform vec3 iResolution;
uniform vec3 iMouse;
uniform sampler2D iChannel0;
out vec4 fragColor;

// by Nikos Papadopoulos, 4rknova / 2013
// Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.

#define ENABLE_LIGHTING
#define ENABLE_SPECULAR

#define OFFSET_X 1
#define OFFSET_Y 1
#define DEPTH	 7.5

vec3 texsample(const int x, const int y, in vec2 fragCoord)
{
	vec2 uv = fragCoord.xy / iResolution.xy * iResolution.xy;
	uv = (uv + vec2(x, y)) / iResolution.xy;
	return texture(iChannel0, uv).xyz;
}

float luminance(vec3 c)
{
	return dot(c, vec3(.2126, .7152, .0722));
}

vec3 normal(in vec2 fragCoord)
{
	float R = abs(luminance(texsample( OFFSET_X,0, fragCoord)));
	float L = abs(luminance(texsample(-OFFSET_X,0, fragCoord)));
	float D = abs(luminance(texsample(0, OFFSET_Y, fragCoord)));
	float U = abs(luminance(texsample(0,-OFFSET_Y, fragCoord)));
				 
	float X = (L-R) * .5;
	float Y = (U-D) * .5;

	return normalize(vec3(X, Y, 1. / DEPTH));
}

void main()
{
	vec3 n = normal(gl_FragCoord.xy);

	vec3 lp = vec3(iMouse.xy / iResolution.xy * iResolution.xy, 200.);
	vec3 sp = vec3(gl_FragCoord.xy / iResolution.xy * iResolution.xy, 0.);
	
	vec3 c = texsample(0, 0, gl_FragCoord.xy) * dot(n, normalize(lp - sp));

    float e = 64.;
    vec3 ep = vec3(iResolution.x * .5, (iResolution.y) * .5, 500.);
	c += pow(clamp(dot(normalize(reflect(lp - sp, n)), 
					   normalize(sp - ep)), 0., 1.), e);

	//vec3 c = n;
	
	fragColor = vec4(c, 1.);
}
