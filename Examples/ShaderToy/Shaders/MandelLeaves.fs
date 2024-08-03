//https://www.shadertoy.com/view/MttBz8
#version 310 es
precision highp float;       	// Set the default precision to medium

uniform float iTime;
uniform vec3 iResolution;
uniform vec3 iMouse;

out vec4 fragColor;

vec2 cmul(vec2 a, vec2 b) { return vec2(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x); }

vec3 hsv(float h, float s, float v) {
	vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
	vec3 p = abs(fract(vec3(h) + K.xyz) * 6.0 - K.www);
	return v * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), s);
}


void main()
{
	vec2 surfacePosition = 0.5 * (2.0 * gl_FragCoord.xy - iResolution.xy) / min(iResolution.x, iResolution.y);
    
    float mouseY = iMouse.y == 0.0 ? 0.0 : iMouse.y / iResolution.y - 0.5;
	float zoom = exp(mouseY * 8.0);
    
    //vec2 p = zoom * 2.0 * surfacePosition - vec2(0.7, 0.0);
	vec2 p = zoom * 0.016 * surfacePosition - vec2(0.805, -0.176);
	//vec2 p = zoom * 0.001 * surfacePosition - vec2(1.924, 0.0);

	vec2 z = p;
	vec2 c = p;
	vec2 dz = vec2(1.0, 0.0);
	float it = 0.0;
	for(float i = 0.0; i<1024.0; i+=1.0) {
		dz = 2.0 * cmul(z, dz) + vec2(1.0, 0.0);
		z = cmul(z, z) + c;

        float a = sin(iTime * 1.5 + i * 2.0) * 0.3 + i * 1.3;
		vec2 t = mat2(cos(a), sin(a), -sin(a), cos(a)) * z;
		if(abs(t.x) > 2.0 && abs(t.y) > 2.0) { it = i; break; }
	}

	if (it == 0.0) {
		fragColor = vec4(vec3(0.0), 1.0);
	} else {
		float z2 = z.x * z.x + z.y * z.y;
		float dist = log(z2) * sqrt(z2) / length(dz);
		float r = sqrt(z2);

		float pixelsize = fwidth(p.x);
		float diagonal = length(iResolution.xy);
		float glowsize = pixelsize * diagonal / 400.0;
		float shadowsize = pixelsize * diagonal / 80.0;

		float fadeout = 0.0, glow = 0.0;
		if(dist < pixelsize) {
			fadeout = dist / pixelsize;
			glow = 1.0;
 		} else {
			fadeout = min(shadowsize / (dist + shadowsize - pixelsize) + 1.0 / (r + 1.0), 1.0);
			glow = min(glowsize / (dist + glowsize - pixelsize), 1.0);
		}

		fragColor = vec4(hsv(
			it / 32.0 + 0.4,
			1.0 - glow,
			fadeout
		), 1.0);		
	}
}

