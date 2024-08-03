//https://www.shadertoy.com/view/4ts3DH
#version 310 es
precision highp float;       	// Set the default precision to medium

uniform float iTime;
uniform vec3 iResolution;
uniform sampler2D iChannel0;
out vec4 fragColor;

#define V vec3

V k = V(.4,-.2,.9);

V m( V p )
{
    p -= iTime;
	for( int i=0; i<16; i++ ) 
        p = reflect( abs(p)-9., k );
    return p* .5;
}

void main()
{
    V d = V(gl_FragCoord.xy,1)/iResolution, o = d;
    
    for( int i=0; i<99; i++ )  
        o += d * m(o).x;
    
    fragColor = texture( iChannel0, m(o).yz ) * (.5 + 99.*m(o-k*.02).x) * exp(.04*o.yzzz);
}
