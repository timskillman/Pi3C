// https://www.shadertoy.com/view/4tl3W8
// 2 Tweets Challenge by nimitz 2015 (twitter: @stormoid)
// https://www.shadertoy.com/view/4tl3W8
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License
// Contact the author for other licensing options

#version 310 es
precision highp float;       	// Set the default precision to medium

uniform float iTime;
uniform vec3 iResolution;
uniform vec3 iMouse;

out vec4 fragColor;

/*
The rules:
	-Label your shaders with [2TC 15]
	-280 Chars or less (as counted by shadertoy)
	-Submissions end Feb 2 2015 (2 weeks)
	-You can use all the shadertoy uniforms (textures, audio, input, time, etc..)
	-You can use macros as much as you want
 	-You can have as many entries as you like
  	-Don't expect any sort of large prize
*/
void main(){
    vec4 p = vec4(gl_FragCoord.xy,0.,1.)/iResolution.y - vec4(.9,.5,0,0), c=p-p;
	float t=iTime,r=length(p.xy+=sin(t+sin(t*.8))*.4),a=atan(p.y,p.x);
	for (float i = 0.;i<60.;i++)
        c = c*.98 + (sin(i+vec4(5,3,2,1))*.5+.5)*smoothstep(.99, 1., sin(log(r+i*.05)-t-i+sin(a +=t*.01)));
    fragColor = vec4((c*r).xyz,1.);
}
