//https://www.shadertoy.com/view/lllGWH
#version 310 es
precision highp float;       	// Set the default precision to medium

uniform float iTime;
uniform vec3 iResolution;
uniform vec3 iMouse;

out vec4 fragColor;

void main()
{
    vec2 p = vec2(-.745,.186) + 3.*(gl_FragCoord.xy/iResolution.y-.5)*pow(.01,1.+cos(.2*iTime));
    
    float n = 0.;
    vec2 z = p*n;
    
    for( ; n<128. && dot(z,z)<1e4; n++ )
        z = vec2( z.x*z.x - z.y*z.y, 2.*z.x*z.y ) + p;
    
    fragColor = vec4(.5 + .5*cos( vec4(3,4,11,0) + .05*(n - log2(log2(dot(z,z)))) ).rgb,1.);
}
