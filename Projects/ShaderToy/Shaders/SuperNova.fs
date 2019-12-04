//https://www.shadertoy.com/view/XdVXWc

#version 300 es
precision highp float;       	// Set the default precision to medium

uniform float iTime;
uniform vec3 iResolution;
uniform vec3 iMouse;
uniform sampler2D iChannel3;
out vec4 fragColor;

// "Type 2 Supernova" by Duke
//-------------------------------------------------------------------------------------
// Based on "Supernova remnant" (https://www.shadertoy.com/view/MdKXzc)
// "Volumetric explosion" (https://www.shadertoy.com/view/lsySzd)
// otaviogood's "Alien Beacon" (https://www.shadertoy.com/view/ld2SzK)
// Shane's "Cheap Cloud Flythrough" (https://www.shadertoy.com/view/Xsc3R4) shaders
// Some ideas came from other shaders from this wonderful site
// Press 1-2-3 to zoom in and zoom out.
// License: Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License
//-------------------------------------------------------------------------------------

#define DITHERING
#define BACKGROUND

//#define TONEMAPPING

//-------------------
#define pi 3.14159265
#define R(p, a) p=cos(a)*p+sin(a)*vec2(p.y, -p.x)
#define tex(a,b) textureLod(a,b,0.)

// iq's noise
float noise( in vec3 x )
{
    vec3 p = floor(x);
    vec3 f = fract(x);
	f = f*f*(3.0-2.0*f);
	vec2 uv = (p.xy+vec2(37.0,17.0)*p.z) + f.xy;
	vec2 rg = tex( iChannel3, (uv+ 0.5)/256.0).yx;
	return 1. - 0.82*mix( rg.x, rg.y, f.z );
}

float fbm( vec3 p )
{
   return noise(p*.06125)*.5 + noise(p*.125)*.25 + noise(p*.25)*.125 + noise(p*.4)*.2;
}

float Sphere( vec3 p, float r )
{
    return length(p)-r;
}

//==============================================================
// otaviogood's noise from https://www.shadertoy.com/view/ld2SzK
//--------------------------------------------------------------
// This spiral noise works by successively adding and rotating sin waves while increasing frequency.
// It should work the same on all computers since it's not based on a hash function like some other noises.
// It can be much faster than other noise functions if you're ok with some repetition.
const float nudge = 4.;	// size of perpendicular vector
float normalizer = 1.0 / sqrt(1.0 + nudge*nudge);	// pythagorean theorem on that perpendicular to maintain scale
float SpiralNoiseC(vec3 p)
{
    float n = 1.-mod((iTime - 1.) * 0.1,-1.); // noise amount
    float iter = 2.0;
    for (int i = 0; i < 8; i++)
    {
        // add sin and cos scaled inverse with the frequency
        n += -abs(sin(p.y*iter) + cos(p.x*iter)) / iter;	// abs for a ridged look
        // rotate by adding perpendicular and scaling down
        p.xy += vec2(p.y, -p.x) * nudge;
        p.xy *= normalizer;
        // rotate on other axis
        p.xz += vec2(p.z, -p.x) * nudge;
        p.xz *= normalizer;
        // increase the frequency
        iter *= 1.733733;
    }
    return n;
}

float VolumetricExplosion(vec3 p)
{
    float final = Sphere(p,4.);
    final += noise(p*20.)*.4;
    final += SpiralNoiseC(p.zxy*fbm(p*10.))*2.5; //1.25;

    return final;
}

float map(vec3 p) 
{
	R(p.xz, iMouse.x*0.008*pi+(iTime - 1.)*0.05);
	float igt = 2.0*sqrt(1.0 + mod((iTime - 1.)*0.1,-1.));
	float VolExplosion = VolumetricExplosion(p/igt)*igt; // scale
	return abs(VolExplosion) + 0.05 + igt * 0.0175;
}
//--------------------------------------------------------------

// assign color to the media
vec3 computeColor( float density, float radius )
{
	// color based on density alone, gives impression of occlusion within
	// the media
	vec3 result = mix( vec3(0.25, 0.5, 1.0), vec3(1.0, 0.5, 0.25), density);
	
	// color added to the media
	vec3 colCenter = 2.0 * vec3(0.1, 0.1, 4.0);
	vec3 colEdge = 2.0 * vec3(0.6, 0.1, 0.1);
	result *= mix( colCenter, colEdge, min( (radius+.05)/.9, 1.15 ) );
	
	return result;
}

bool RaySphereIntersect(vec3 org, vec3 dir, out float near, out float far)
{
	float b = dot(dir, org);
	float c = dot(org, org) - 8.;
	float delta = b*b - c;
	if( delta < 0.0) 
		return false;
	float deltasqrt = sqrt(delta);
	near = -b - deltasqrt;
	far = -b + deltasqrt;
	return far > 0.0;
}

void main()
{  
    float key = 0.0;

    vec2 uv = gl_FragCoord.xy/iResolution.xy;
    
	// ro: ray origin
	// rd: direction of the ray
	vec3 rd = normalize(vec3((gl_FragCoord.xy-0.5*iResolution.xy)/iResolution.y, 1.));
	vec3 ro = vec3(0., 0., -5.+key*1.6);
    
	// ld, td: local, total density 
	// w: weighting factor
	float ld=0., td=0., w=0.;

	// t: length of the ray
	// d: distance function
	float d=1., t=0.;
    
    const float h = 0.1;
   
	vec4 sum = vec4(0.0);
   
    float min_dist=0.0, max_dist=0.0;

    if(RaySphereIntersect(ro, rd, min_dist, max_dist))
    {
       
	t = min_dist*step(t,min_dist);
   
	// raymarch loop
    for (int i=0; i<64; i++)
	{
	 
		vec3 pos = ro + t*rd;
  
		// Loop break conditions.
	    if(td>0.9 || d<0.11*t || t>10. || sum.a > 0.99 || t>max_dist) break;
        
        // evaluate distance function
        float d = map(pos);
        
        // point light calculations
        vec3 ldst = vec3(0.0)-pos;
        float lDist = max(length(ldst), 0.001);

        // the color of light 
        vec3 lightColor=vec3(0.5,0.5,0.5);
        
        sum.rgb+=(vec3(0.67,0.75,1.00)/(lDist*lDist*15.)/100.); // star itself
        sum.rgb+=(lightColor/exp(lDist*lDist*lDist*.08)/30.); // bloom
        
		if (d<h) 
		{
			// compute local density 
			ld = h - d;
            
            // compute weighting factor 
			w = (1. - td) * ld;
     
			// accumulate density
			td += w + 1./200.;
		
			vec4 col = vec4( computeColor(td,lDist), td );
            
            // emission
            sum += sum.a * vec4(sum.rgb, 0.0) * 0.2 / lDist;	
            
			// uniform scale density
			col.a *= 0.2;
			// colour by alpha
			col.rgb *= col.a;
			// alpha blend in contribution
			sum = sum + col*(1.0 - sum.a);  
       
		}
      
		td += 1./70.;

        #ifdef DITHERING
        // idea from https://www.shadertoy.com/view/lsj3Dw
        vec2 uvd = uv;
        uvd.y*=120.;
        uvd.x*=280.;
        d=abs(d)*(.8+0.08*tex(iChannel3,vec2(uvd.y,-uvd.x+0.5*sin(4.*(iTime - 1.)+uvd.y*4.0))).r);
        #endif 
		
        // trying to optimize step size
        t += max(d * 0.1 * max(min(length(ldst),length(ro)),0.1), 0.01);

	}
    
    // simple scattering
    sum *= 1. / exp( ld * 0.2 ) * 0.8;
        
   	sum = clamp( sum, 0.0, 1.0 );
   
    sum.xyz = sum.xyz*sum.xyz*(3.0-2.0*sum.xyz);
    
	}
    
    #ifdef BACKGROUND
    // stars background
    if (td<.8)
    {
        vec3 stars = vec3(noise(rd*500.0)*0.5+0.5);
        vec3 starbg = vec3(0.0);
        starbg = mix(starbg, vec3(0.8,0.9,1.0), smoothstep(0.99, 1.0, stars)*clamp(dot(vec3(0.0),rd)+0.75,0.0,1.0));
        starbg = clamp(starbg, 0.0, 1.0);
        sum.xyz += starbg; 
    }
	#endif
   
    float tm = mod((iTime - 1.), 10.);
	fragColor = vec4(sum.xyz,1.0) + (0.1/tm);
}
