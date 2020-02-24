//https://www.shadertoy.com/view/4tXGW4

#version 300 es
precision highp float;       	// Set the default precision to medium

uniform float iTime;
uniform vec3 iResolution;
out vec4 fragColor;

void main() {
	
	vec2  r = iResolution.xy, p = gl_FragCoord.xy - r*.5;
	float d = length(p) / r.y, c=1., x = pow(d, .1), y = atan(p.x, p.y) / 6.28;
	
	for (float i = 0.; i < 3.; ++i)    
		c = min(c, length(fract(vec2(x - iTime*i*.005, fract(y + i*.125)*.5)*20.)*2.-1.));

	fragColor = vec4(d+20.*c*d*d*(.6-d));
}
