#version 310 es
precision highp float;       	// Set the default precision to medium

uniform float iTime;
uniform vec3 iResolution;
uniform sampler2D iChannel0;
out vec4 fragColor;

void main(){
	vec4 O; O-=O;
    for(vec2 x=gl_FragCoord.xy/iResolution.y-.5;
		O.w<2.;
		O+=texture(iChannel0,(x*asin(dot(x,x)*O.w)/dot(x,x)-iTime)*.2)*.01);
	fragColor = vec4(O.rgb,1.);
}
