#version 300 es
precision mediump float;       	// Set the default precision to medium

uniform float iTime;
uniform vec3 iResolution;
out vec4 fragColor;

vec2 thingPosition(float t, float aspect) {
    float tx = t / aspect;
    vec2 p = vec2(sin(2.2 * tx) - cos(1.4 * tx), cos(1.3 * t) + sin(-1.9 * t));
    p.y *= 0.2;
    p.x *= 0.4;
 	return p;
}

void main()
{
	vec2 uv = vec2(0.5) - gl_FragCoord.xy / iResolution.xy;
    float aspect = iResolution.x / iResolution.y;
    uv.x *= aspect;
    vec3 cFinal = vec3(0.0);
    
    vec3 color1 = vec3(0.9, 0.2, 0.4);
    //vec3 color2 = vec3(0.8, 0.3, 0.2);
    const float radius = 0.035;
    const float tailLength = 0.7;
    const float edgeWidth = 0.03;
    for (int j = 0; j < 11; j++) {
        float thisRadius = radius + sin(float(j) * 0.7 + iTime * 1.2) * 0.02;
        float dMin = 1.0;
        const int iMax = 12;
        for (int i = 0; i < iMax; i++) {
            float iPct = float(i) / float(iMax);
            float segmentDistance = length(thingPosition(iTime * 2.0 + float(j) * 1.5 - iPct * tailLength, aspect) - uv);
            dMin = min(dMin, segmentDistance + pow(iPct, 0.8) * (thisRadius + edgeWidth));
        }
        cFinal += 5.0 * (1.0 - smoothstep(thisRadius, thisRadius + edgeWidth, dMin)) * color1; //mix(color1, color2, mod(float(j), 2.0));
    }
    
	fragColor = vec4(vec3(1.0) - cFinal, 1.0);
}
