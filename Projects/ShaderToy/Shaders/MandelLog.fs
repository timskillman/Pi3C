// https://www.shadertoy.com/view/XsjBD1
// 269
// Modified version of inigo quilez - iq/2015
// https://www.shadertoy.com/view/lllGWH
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.

#version 300 es
precision highp float;       	// Set the default precision to medium

uniform float iTime;
uniform vec3 iResolution;
uniform vec3 iMouse;

out vec4 fragColor;


void main()
{
    float n = 0.,m; // need them after the loop
    vec4 z; vec2 p;
    for(  z -= z;
           n++ < 99.
        && m   < 1e7;
       )
        // z.xy is Z, z.zw id dZ/dC
        z = vec4(
            (z.x*z.x - z.y*z.y)/2. - 2.08 ,
            z.x*z.y + .6975 ,
            z.x*z.z-z.y*z.w + 1.,
            dot(z.xy,z.wz)
        ),
		m = length(z.xy +=
              (gl_FragCoord.xy/iResolution.y-.5)*exp(sin(iTime)*4.-3.) // C
        );
    
    // from https://www.shadertoy.com/view/MttXz8
	// d(c) = |Z|^2·log|Z|/|Z'|
    // here we take log of 1/d(c) and length(z) instead of length(z.zw)
    n = log(length(z/m/log(m)));
    fragColor = vec4(.5 + .5*cos( vec4(3,2,7,1)  - n ).rgb,1.);
    //z *= sqrt(1.3 - n/8. - sin(iTime)*.5)*1.5; // removes flickering, 2 many chars :-(
}

/* // 280, mandelbrot formulae is broken
precision highp float;

void mainImage( out vec4 z,  vec2 p )
{
    float n = 0.,m; // need them after the loop
    vec4 z;
    for( ; n<99. && m < 3e5; n++ )
        // z.xy is Z, z.zw id dZ/dC
        z = vec4(
            (z.x*z.x - z.y*z.y)/2. - 2.1 ,
            z.x*z.y + .5 ,
            z.x*z.z-z.y*z.w + 1.,
            dot(z.xy,z.wz)
        ),
		m = length(z.xy +=
              (gl_FragCoord.xy/iResolution.y-.6)*exp(sin(iTime)*5.-3.) // C
        );
    
    // from https://www.shadertoy.com/view/MttXz8
	// d(c) = |Z|^2·log|Z|/|Z'|
    // here we take log of d(c) and length(z) instead of length(z.zw)
    
    fragColor = .5 + .5*cos( vec4(3,2,7,1) +
                    log( m/length(z) * log(m) )
                   );
}
*/

/* // 267 with derivative and optim, without using dZ/dC
void mainImage( out vec4 z,  vec2 p )
{
    float n = 0.,m;
    p = 4. * (p/iResolution.y-.5)*exp(-iTime*.2);
    
    for( ; n<99. && m < 1e4; n++ )
        z = vec4(
            z.x*z.x - z.y*z.y - 1.26 ,
            2.*z.x*z.y + 0.34 ,
            2.*z.x*z.z-z.y*z.w + 2.,
            2.*(z.x*z.w + z.y*z.z)
        ),
		m = length(z.xy+=p);
    
    
    z = .5 + .5*cos( vec4(3,4,11,0) + .05*(n - log2(log2(m))) );
}
*/
/* // 230 optim from IQ original Mandelbrot 2 tweets
void mainImage( out vec4 f,  vec2 p )
{
    float n = 0.,m;
    vec2 p = 4. * (gl_FragCoord.xy/iResolution.y-.5)*exp(-iTime*.2); 
    vec2 z = p*n;
    
    for( ; n<99. && m < 1e4; n++ )
        z = vec2(
            z.x*z.x - z.y*z.y -1.26,
            2.*z.x*z.y + 0.34
        ) + p,
		m = dot(z,z);
    
    
    fragColor = .5 + .5*cos( vec4(3,4,11,0) + .05*(n - log2(log2(m))) );
}
*/
