

// [2TC 15] Gas giant
// by Andrew Baldwin.
// This work is licensed under a Creative Commons Attribution 4.0 International License.

#version 300 es
precision highp float;       	// Set the default precision to medium

uniform float iTime;
uniform vec3 iResolution;
uniform sampler2D iChannel2;
out vec4 fragColor;

void main()
{
	vec4 u = .5-vec4(gl_FragCoord.xy,0.,-.5)/iResolution.xyzz;
    u.x+=.3*cos(u.y);
	fragColor = texture(iChannel2,.3*(u.yw/u.x+vec2(iTime*0.01666,iTime)))*step(.1,u.x);
}
