
#version 310 es
precision highp float;       	// Set the default precision to medium

uniform float iTime;
uniform vec3 iResolution;
out vec4 fragColor;

/**/ // variant of https://shadertoy.com/view/MljBDG

#define l(a)  2.6*(a)*(a)                             // approx of spiral arc-length

void main() 
{
	vec2 R = iResolution.xy, V;
    vec2 U = gl_FragCoord.xy; U=5.* ( U+U - R ) / R.y;                        // normalized coordinates
    U = vec2( atan(U.y,U.x)/6.283 +.5, length(U) );   // polar coordinates
    U.y-= U.x;                                        // lenght along spiral
    U.x = l( ceil(U.y)+U.x ) - iTime;                 // arc-length
    vec4 O = vec4(1.- pow( abs( 2.*fract(U.y)-1.),10.)); // inter-spires antialiasing
    V   = ceil(U); U = fract(U)-.5;                   // cell along spiral: id + loc coords
 // vortices (small spirals) : assume col = step(0,y) then rotate( (0,0), space&time*(.5-dist) )
    U.y = dot( U, cos( vec2(-33,0)                    // U *= rot, only need U.y -> (-sin,cos)
                       +  .3*( iTime + V.x )          // rot amount inc with space/time
                         * max( 0., .5 - length(U) )  // rot amount dec with dist
             )       );
	fragColor = O*smoothstep( -1., 1., U/fwidth(U) ).y;        // draw antialiased vortices
}

