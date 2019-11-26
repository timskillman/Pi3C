//https://www.shadertoy.com/view/ldlGz8

#version 300 es
precision mediump float;       	// Set the default precision to medium

uniform float iTime;
uniform vec3 iResolution;
uniform sampler2D iChannel0;
out vec4 fragColor;


/**
 * Estimate fragment normal by using a sobel filter
 */
vec3 estimateNormal(in vec2 fragCoord)
{
	mat3 img;

	for (int y = -1; y <= 1; ++y)
	{
		for (int x = -1; x <= 1; ++x)
		{
			img[x + 1][y + 1] = length(texture(iChannel0, (vec2(fragCoord.x, 1.0 - fragCoord.y) + vec2(x, y)) / iResolution.xy).rgb);
		}
	}
	
	float sobelX = img[2][0] + 2.0 * img[2][1] + img[2][2] - img[0][0] - 2.0 * img[0][1] - img[0][2]; 
	float sobelY = img[0][2] + 2.0 * img[1][2] + img[2][2] - img[0][0] - 2.0 * img[1][0] - img[2][0];

	return vec3(sobelX, -sobelY, 1.0);
}

void mainI()
{
	vec3 n = estimateNormal(gl_FragCoord.xy);
	vec3 l = normalize(vec3(iMouse.xy / iResolution.xy - vec2(0.5, 0.5), -1.0));
	vec3 v = vec3(0.0, 0.0, -1.0);
	
	float ambient = 0.8;
	float diffuse = max(dot(n, l), 0.0);
	float specular = max(pow(dot((2.0 * n * dot(n, l)) - l, v), 6.0), 0.0);
	
	float intensity = ambient + 0.01 * (diffuse + specular);
	
	vec3 color = texture(iChannel0, vec2(fragCoord.x, 1.0 - fragCoord.y) / iResolution.xy).rgb;
	fragColor = vec4(color * intensity, 1.0);
}

