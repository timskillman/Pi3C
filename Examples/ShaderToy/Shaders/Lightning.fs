//https://www.shadertoy.com/view/MdSGW1
#version 310 es
precision highp float;       	// Set the default precision to medium

uniform float iTime;
uniform vec3 iResolution;

uniform sampler2D iChannel3;

out vec4 fragColor;

void main()
{
	vec2 o = vec2(iResolution.x/2.0, iResolution.y*0.9);
	vec2 d = vec2(0.0, -1.0);
	vec2 uv = gl_FragCoord.xy;
	vec3 col = vec3(0.0);
	for (int i=0; i<100; i++) {
		vec4 tex = texture(iChannel3, vec2(float(i)/256.0, iTime), -100.0);
		vec2 tgt = vec2(iResolution.x/2.0, iResolution.y*0.1)-o;
		vec2 seek = normalize(tgt)*(16.0/(length(tgt)+1.0));
		d = normalize(seek+vec2(1.5, -1.0)*(vec2(-0.5, 0.0)+tex.gb));
		float len = min(length(tgt), 9.0 * (tex.r+0.1));
		float dist = abs(dot(o-uv, d.yx*vec2(1.0,-1.0)));
		o += d*len;
		if (dist < 1.5 && length(o-uv) < len*0.75) {
			col = vec3(1.0);
			break;
		}
	}
	fragColor = vec4(col, 1.0);
}
