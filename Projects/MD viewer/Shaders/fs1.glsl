precision highp float;       	// Set the default precision to medium

uniform sampler2D u_Texture;   	// texture
varying vec2 v_UV;				// Texture UV coordinate
varying vec4 v_diffuseColour;	// Diffuse colour
varying vec4 v_fogColour;		// Fog colour

void main()
{
		vec4 col = texture2D(u_Texture, v_UV) * v_diffuseColour + v_fogColour;
		if (col.a > 0.01) gl_FragColor = col; else discard;
}
