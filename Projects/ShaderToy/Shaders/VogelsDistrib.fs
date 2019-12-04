//https://www.shadertoy.com/view/XtlXDM

#version 300 es
precision highp float;       	// Set the default precision to medium
uniform vec3 iResolution;
out vec4 fragColor;

// compacification of  fernlightning's shader https://www.shadertoy.com/view/XtsSWM
// with the help of coyote and fernlightning (see fernlightning's shader forum for compaction steps)

void main() {

    vec4 o = iResolution.xyyy;
    vec2 i = gl_FragCoord.xy + gl_FragCoord.xy - o.xy;
    float r = length(i/=o.y/1e2)-2.6, 
          v = floor(r*r*.06);  // floor(...-.5) to add one circle at center
          r+=5.2; 

    for (int k = 0; k < 128; k++)
        v++ < r*r*.06 ? 
          o = min(o, length( sqrt(v/.06) * cos(v*2.4+vec2(1.57,0)) -i ) )
        : o ;

    fragColor = 1.-abs(o-2.);   // or o = abs(o-2.); for reverse video (-3 chars)
}

