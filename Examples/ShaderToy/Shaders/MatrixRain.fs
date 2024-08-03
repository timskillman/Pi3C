#version 310 es
precision highp float;       	// Set the default precision to medium

uniform float iTime;
uniform vec3 iResolution;
out vec4 fragColor;

// Created by Reinder Nijhoff 2015
// @reindernijhoff
//
// https://www.shadertoy.com/view/XdcGzr
//
// Based on matrix - 255 char by FabriceNeyret2: https://www.shadertoy.com/view/4tlXR4
// compacting to 2-tweets patriciogv's Matrix shader https://www.shadertoy.com/view/MlfXzN ( 819 -> 255 chars ) 
// But first go see patriciogv's comments and readable sources :-D
//
// All credits go to FabriceNeyret2
//

#define R fract(43.*sin(p.x*7.+p.y*8.))

void main() {
    vec2 i = gl_FragCoord.xy;
    vec2 j = fract(i*=.1), 
         p = vec2(9,int(iTime*(9.+8.*sin(i-=j).x)))+i;
    vec4 o; o-=o; o.g=R; p*=j; o*=R>.5&&j.x<.6&&j.y<.8?1.:0.;
    fragColor = vec4(o.rgb,1.);
}
