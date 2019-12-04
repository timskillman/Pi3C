//https://www.shadertoy.com/view/XtlGW4
#version 300 es
precision highp float;       	// Set the default precision to medium

uniform float iTime;
uniform vec3 iResolution;
out vec4 fragColor;

void main(){
    
    vec3 q=iResolution,d=vec3(gl_FragCoord.xy-.5*q.xy,q.y)/q.y,c=vec3(0,.5,.7);
    
    q=d/(.1-d.y);
    float a=iTime, k=sin(.2*a), w = q.x *= q.x-=.05*k*k*k*q.z*q.z;

    fragColor.xyz=d.y>.04?c:
    	sin(4.*q.z+40.*a)>0.?
        w>2.?c.xyx:w>1.2?d.zzz:c.yyy:
	    w>2.?c.xzx:w>1.2?c.yxx*2.:(w>.004?c:d).zzz;
		fragColor = vec4(fragColor.rgb,1.);
    
}
