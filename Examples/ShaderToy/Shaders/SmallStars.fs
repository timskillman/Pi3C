//https://www.shadertoy.com/view/XtlGW4
#version 310 es
precision highp float;       	// Set the default precision to medium

uniform float iTime;
uniform vec3 iResolution;
out vec4 fragColor;

// SmallStars - @P_Malin
// My attempt to play at starfield code golf.
// Along the lines of https://www.shadertoy.com/view/MdlXWr

// Fully initialize f by using iDate.wwww - 138 characters
//*
void main()
{
    vec2 p=gl_FragCoord.xy/2e3-.2;

    float b = ceil(atan(p.x, p.y) * 6e2), h = cos(b), z = h / dot(p,p);
    fragColor = exp(fract(z + h * b + vec4(iTime,iTime,iTime,iTime)) * -1e2) / z;
    fragColor = vec4(fragColor.rgb,1.);
}
