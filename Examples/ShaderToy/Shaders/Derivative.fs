// Created by inigo quilez - iq/2013
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.

#version 310 es
precision highp float;       	// Set the default precision to medium

uniform float iTime;
uniform vec3 iResolution;
uniform vec3 iMouse;

out vec4 fragColor;

float f(vec3 p) 
{ 
	p.z+=iTime;return length(.05*cos(9.*p.y*p.x)+cos(p)-.1*cos(9.*(p.z+.3*p.x-p.y)))-1.; 
}
void main()
{
    vec3 d=.5-vec3(gl_FragCoord.xy,1)/iResolution.x,o=d;for(int i=0;i<128;i++)o+=f(o)*d;
    fragColor = vec4(abs(f(o-d)*vec3(0,1,2)+f(o-.6)*vec3(2,1,0))*(1.-.1*o.z),1.);
}
