#version 310 es
precision highp float; 

uniform sampler2D u_Texture;   	// texture
in vec2 v_UV;				// Texture UV coordi_nate
in vec4 v_diffuseColour;	// Diffuse colour
in vec4 v_fogColour;		// Fog colour

void main()
{
		vec4 col = texture2D(u_Texture, v_UV) * v_diffuseColour + v_fogColour;
		if (col.a > 0.01) gl_FragColor = col; else discard;
}
