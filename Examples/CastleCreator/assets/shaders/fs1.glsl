#version 310 es
precision highp float;       	// Set the default precision to medium
//Note high precision is needed to NVidia RTX2060 card

uniform sampler2D u_Texture;   	// texture
in vec2 v_UV;				// Texture UV coordinate (i_nterpolated)
in vec4 v_diffuseColour;	// Diffuse colour (i_nterpolated)
in vec4 v_fogColour;		// Fog colour (i_nterpolated)



out vec4 FragColor;

void main()
{
		vec4 col1 = texture(u_Texture, v_UV) * v_diffuseColour + v_fogColour;
		if (col1.a > 0.01) FragColor = col1; else discard;
}
