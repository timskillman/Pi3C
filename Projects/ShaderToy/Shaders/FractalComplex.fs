//https://www.shadertoy.com/view/ltfGzS

#version 300 es
precision highp float;       	// Set the default precision to medium

uniform float iTime;
uniform vec3 iResolution;
out vec4 fragColor;

#define X r = max( r = abs(mod(q*s+1.,2.)-1.) , r.yzxw ), d = max(d, (.29-min(r.x,min(r.y,r.z))) / s ),  s *= 3.
void main()
{
    vec4 p = vec4(gl_FragCoord.xy,0,1)/iResolution.x-.5, r, q = p-p;
    q.x = .3*sin(q.z=-iTime);
    fragColor = p/p; //hm, can this fail? for example for gl_FragCoord.x == iResolution.x / 2.

    for (int i=0; i < 99; i++) {
        float d=0.,s=1.;
        X; X; X; X; X;
        d > 1e-5 ? q += p*d, fragColor -= .01 : fragColor;
    }
}
