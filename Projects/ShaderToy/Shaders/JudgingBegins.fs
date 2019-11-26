

// Neon Light Illusion by nimitz (twitter: @stormoid)
// https://www.shadertoy.com/view/llXGzB
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License
// Contact the author for other licensing options

/*
	The Two tweets challenge 2015 is over.

	An overwhelming	amount of great stuff has been submitted in
	the past 2 weeks, I was expecting maybe a dozen entries and
	we are at 105 at the time of this writing.

	Thanks to everyone who participated and helped make this
	fun and interesting.



	Judging:
		Fow anyone who wants to help with judging, you can
		send you top 5 picks in order (from #1 to #5, best being #1)
		via	email at: nimitzstg at gmail.com
		Anyone who participated in the tournament can
		be a judge, just let me know what's your shadertoy
		username in the email. Only restriction is you can't
		vote for your own shaders and you must send exactly
		5 picks. You can vote for multiple shaders of a single
		user in your top 5.

	TLDR:
		-Send your top 5 shaders in order from best to worst
		-Don't vote for yourself
		-Mention your shadertoy username
		-You can vote for multiple shaders of a single user

	Points Calculation:
		Every judge has the same voting weight, your
		first pick gets 1 point, second 0.75 points,
		third 0.5625 points and so on (n+1 = n*0.75).
		Winners are the shaders with most points.
*/

#version 300 es
precision mediump float;       	// Set the default precision to medium

uniform float iTime;
uniform vec3 iResolution;
out vec4 fragColor;

void main(){
    vec4 o = gl_FragCoord/iResolution.xyxx-.5, d=o, r, z=d-d;
    float t = iTime, c;
    o.xz += t;
    for(int i=0;i<99;i++)
    	c= dot( (r = cos(o + o.w*sin(t*.9)*.1)).xyz, r.xyz),
    	o -= d* dot( (r = cos(r/c*7.)).zy, r.zy ) *c*.2,
    	z += abs(sin(vec4(9,0,1,1)+(o.y + t + o.w)))*.12*c/(o.w*o.w+ 6.5);
    z = z*z*z;
	fragColor = vec4(z.xyz,1.);
}
