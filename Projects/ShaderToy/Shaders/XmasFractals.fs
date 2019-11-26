//https://www.shadertoy.com/view/Xdj3Wh
#version 300 es
precision mediump float;       	// Set the default precision to medium

uniform float iTime;
uniform vec3 iResolution;
out vec4 fragColor;

void main()
{
	float t = .01 * iTime + .3;
	vec4 p = abs(4.-8.*gl_FragCoord.xyxx / iResolution.x), c=p*.0;
	p.yx *= 6. * t;
	for (float d=.2;d<2.;d+=.3) {
		p -= .5*d;
		for (int i=0;i<60;i++) p.xy=mat2(cos(t),sin(t),-sin(t),cos(t))*(p.xy+sign(p.yx)*vec2(-.2,.6));
		c += .03*p;
	}
	
	fragColor = vec4(c.xyz,1.);
}
