//https://www.shadertoy.com/view/4slXWn
#version 300 es
precision highp float;       	// Set the default precision to medium

uniform float iTime;
uniform vec3 iResolution;
out vec4 fragColor;

#define f(a,b) sin( 50.3* length( gl_FragCoord.xy/iResolution.xy*4.-vec2(cos(a),sin(b) ) -3.))

void main()
{
	float t=iTime; 
	fragColor = vec4(f(t,t)*f(1.4*t,.7*t));
}
