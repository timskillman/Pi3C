//precision highp float;       // Set the default precision to medium. We don't need as high of a

uniform mat4 u_ProjMatrix;     // view/projection matrix.
uniform mat4 u_ModelMatrix;    // model matrix.
uniform vec3 u_LightPos;       // The position of the light in eye space.
uniform vec4 u_lightColour;    // The colour of light in eye space.
uniform int u_illuminationModel;		// If ==2 then apply illumation model
uniform int u_reflective;		//

uniform vec2 u_animoffset;
uniform vec4 u_diffuseColour;
uniform vec4 u_emissiveColour;
uniform vec4 u_ambientColour;
uniform vec4 u_specularColour;

uniform vec3 u_fogColour;
uniform float u_fogMaxDist;
uniform float u_fogRange;  	// effectively 1.0 / (fogMaxDist-fogMinDist)

attribute vec3 a_Position;
attribute vec3 a_Normal;
attribute vec2 a_UV;
 
varying vec2 v_UV;
varying vec4 v_diffuseColour;
varying vec4 v_fogColour;
//varying vec3 v_Normal;
///varying vec3 v_LightPos;

void main()
{
    // Transform position into model space
    vec3 Position = vec3(u_ModelMatrix * vec4(a_Position, 1.0));
	vec3 Normal = normalize(vec3(u_ModelMatrix * vec4(a_Normal, 0.0)));
	vec3 lightVector = normalize(u_LightPos - Position);

	// Calc UV with animation offset
	v_UV = vec2(a_UV.x, 1.0 - a_UV.y) + u_animoffset;
	if (u_reflective > 0) {
		vec3 pseudoreflect = (lightVector + Normal) *0.5;
		v_UV=v_UV + vec2(pseudoreflect.x, -pseudoreflect.y);
	}

	// Calc fog
	vec4 emitColour = max(u_lightColour, u_emissiveColour);
	float fogFactor = (Position.z + u_fogMaxDist) * u_fogRange; //  / (fogMaxDist-fogMinDist)
	fogFactor = clamp(fogFactor, 0.0, 1.0);
	//if (u_illuminationModel == 1) fogFactor = 1.0;
	v_fogColour = vec4((u_fogColour * (1.0 - fogFactor)),0.0) * u_lightColour;
	
	// Calc lighting and specular and mix into fogColour
	vec4 ambcol = vec4(0,0,0,0);
	//vec4 diffuseCol = vec4(u_diffuseColour.rgb * max(u_lightColour.rgb, u_emissiveColour.rgb*(1.0-fogFactor)), u_diffuseColour.a);
	vec4 diffuseCol = u_diffuseColour * emitColour;

	// apply shade and fog ...
	if (u_illuminationModel == 2) {
		float rDotV = max(dot(Normal, lightVector), 0.1);
		fogFactor = fogFactor * rDotV;
		//rDotV = max(0.0, dot(lightVector, Normal));
		ambcol = u_ambientColour * diffuseCol; 
		v_fogColour = v_fogColour + vec4(u_specularColour.rgb * pow(rDotV, 50.0), 1.0);
	}
	
	v_diffuseColour = vec4((diffuseCol + ambcol).rgb * fogFactor, u_diffuseColour.a) ; //preserve alpha
		
    gl_Position = u_ProjMatrix * vec4(Position, 1.0);
}

