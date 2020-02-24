#version 300 es
precision highp float;       	// Set the default precision to medium

uniform float iTime;
uniform vec3 iResolution;
uniform sampler2D iChannel3;
out vec4 fragColor;

#define T texture(iChannel3,(s*p.zw+ceil(s*p.x))/200.).y/(s+=s)*4.

void main(){
    vec4 p,d=vec4(.8,0,gl_FragCoord.xy/iResolution.y-.8),c=vec4(.6,.7,d);
    vec4 O=c-d.w;
    for(float f,s,t=200.+sin(dot(gl_FragCoord.xy,gl_FragCoord.xy));--t>0.;p=.05*t*d)
        p.xz+=iTime,
        s=2.,
        f=p.w+1.-T-T-T-T,
    	f<0.?O+=(O-1.-f*c.zyxw)*f*.4:O;
	fragColor = O;
}
