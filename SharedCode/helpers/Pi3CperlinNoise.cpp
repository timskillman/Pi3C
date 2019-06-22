#include "Pi3CperlinNoise.h"
#include "Pi3CGL.h"
#include <iostream>
#include <cmath>
#include <random>
#include <algorithm>
#include <numeric>

// Code taken from https://github.com/sol-prog/async_tutorial/blob/master/

// 99% OF THIS CODE IS A DIRECT TRANSLATION TO C++11 FROM THE REFERENCE
// JAVA IMPLEMENTATION OF THE IMPROVED PERLIN FUNCTION (see http://mrl.nyu.edu/~perlin/noise/)
// THE ORIGINAL JAVA IMPLEMENTATION IS COPYRIGHT 2002 KEN PERLIN

// I ADDED AN EXTRA METHOD THAT GENERATES A NEW PERMUTATION VECTOR (THIS IS NOT PRESENT IN THE ORIGINAL IMPLEMENTATION)
// 21 OCT 2012 ADDED SOME PREPROCESSORS GUARDS FOR VISUAL STUDIO 2012

// Initialize with the reference values for the permutation vector
Pi3CperlinNoise::Pi3CperlinNoise() {

	// Initialize the permutation vector with the reference values
//#ifdef _MSC_VER
//	int pp[] = {
//		151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,
//		8,99,37,240,21,10,23,190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
//		35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,
//		134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,
//		55,46,245,40,244,102,143,54, 65,25,63,161,1,216,80,73,209,76,132,187,208, 89,
//		18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,
//		250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,
//		189,28,42,223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167,
//		43,172,9,129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,
//		97,228,251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,
//		107,49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
//		138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180 };
//	p.insert(p.begin(), pp, pp + 256);
//#else
	// More elegant BUT initializer lists are not present in VS2012 at 19 Oct 2012
	p = {
		151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,
		8,99,37,240,21,10,23,190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
		35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,
		134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,
		55,46,245,40,244,102,143,54, 65,25,63,161,1,216,80,73,209,76,132,187,208, 89,
		18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,
		250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,
		189,28,42,223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167,
		43,172,9,129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,
		97,228,251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,
		107,49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
		138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180 };
//#endif	

	// Duplicate the permutation vector
	p.insert(p.end(), p.begin(), p.end());
}

// Generate a new permutation vector based on the value of seed
Pi3CperlinNoise::Pi3CperlinNoise(const unsigned int seed) {
	p.resize(256);

	// Fill p with values from 0 to 255
#ifdef _MSC_VER
	int indx = 0;
	std::generate(p.begin(), p.end(), [&indx] {return indx++; });
#else
	std::iota(p.begin(), p.end(), 0);		// more elegant BUT std::iota not present in VS2012 at 19 Oct 2012
#endif

	// Initialize a random engine with seed
	std::default_random_engine engine(seed);

	// Shuffle  using the above random engine
	std::shuffle(p.begin(), p.end(), engine);

	// Duplicate the permutation vector
	p.insert(p.end(), p.begin(), p.end());
}

float Pi3CperlinNoise::octave(float x, float y, float z, int octaves, float persistence, float rof)
{
	float total = 0;
	float frequency = 1.f;
	float amplitude = 1.f;
	float max = 0;
	for (int i = 0; i < octaves; i++) {
		total += noise(x * frequency, y * frequency, z * frequency) * amplitude;

		max += amplitude;

		amplitude *= persistence;
		frequency *= rof;
	}

	return total / max;
}

#define fast (false)

float Pi3CperlinNoise::noise(float x, float y, float z) {
	// Find the unit cube that contains the point
	float xf = floorf(x);
	float yf = floorf(y);
	float zf = floorf(z);

	int X = (int)xf & 255;
	int Y = (int)yf & 255;
	int Z = (int)zf & 255;

	// Find relative x, y,z of point in cube
	x -= xf;
	y -= yf;
	z -= zf;

	// Compute fade curves for each of x, y, z
	float u = fade(x);
	float v = fade(y);
	float w = fade(z);

	// Hash coordinates of the 8 cube corners
	int A = p[X] + Y;
	int AA = p[A] + Z;
	int AB = p[A + 1] + Z;
	int B = p[X + 1] + Y;
	int BA = p[B] + Z;
	int BB = p[B + 1] + Z;

#if fast
	float aaa = grad(p[AA], x, y, z);
	float bbb = grad(p[AB], x, y - 1.f, z);
	float ccc = grad(p[AA + 1], x, y, z - 1.f);
	float ddd = grad(p[AB + 1], x, y - 1.f, z - 1.f);

	float eee = aaa + u * (grad(p[BA], x - 1.f, y, z) - aaa);
	float fff = ccc + u * (grad(p[BA + 1], x - 1.f, y, z - 1.f) - ccc);
#endif

	//float vvv = eee + v * (bbb + u * (grad(p[BB], x - 1.f, y - 1.f, z) - bbb) - eee);
	//float www = ccc + v * (ddd + u * (grad(p[BB + 1], x - 1.f, y - 1.f, z - 1.f) - ddd) - ccc);
	// Add blended results from 8 corners of cube
	float res =
		lerp(w,
#if fast
		//lerp(w, //vvv, www);
			eee + v * (bbb + u * (grad(p[BB], x - 1.f, y - 1.f, z) - bbb) - eee),
			ccc + v * (ddd + u * (grad(p[BB + 1], x - 1.f, y - 1.f, z - 1.f) - ddd) - ccc));
#else
			lerp(v,
			//	aaa + u * (grad(p[BA], x - 1.f, y, z) - aaa),
			//	bbb + u * (grad(p[BB], x - 1.f, y - 1.f, z) - bbb)),
			lerp(u, grad(p[AA], x, y, z), grad(p[BA], x - 1.f, y, z)), 
			lerp(u, grad(p[AB], x, y - 1.f, z), grad(p[BB], x - 1.f, y - 1.f, z))),
			lerp(v,
			//	ccc + u * (grad(p[BA + 1], x - 1.f, y, z - 1.f) - ccc),
			//	ddd + u * (grad(p[BB + 1], x - 1.f, y - 1.f, z - 1.f) - ddd)));
				lerp(u, grad(p[AA + 1], x, y, z - 1.f), grad(p[BA + 1], x - 1.f, y, z - 1.f)),
				lerp(u, grad(p[AB + 1], x, y - 1.f, z - 1.f), grad(p[BB + 1], x - 1.f, y - 1.f, z - 1.f))));
#endif
	return (res + 1.0f) / 2.0f;
}

void Pi3CperlinNoise::createMap(uint8_t* map, uint32_t w, uint32_t h, float z, int octaves, float amplitude, float rof)
{
	int min = 255;
	int max = 0;
	float wh = 1.f / 1024.f, hh = 1.f / 1024.f;
	for (uint32_t y = 0; y < h; y++) {
		for (uint32_t x = 0; x < w; x++) {
			uint32_t p = (y * w + x) * 4;
			float fv = octave((float)x * wh, (float)y * hh, z * wh, octaves, amplitude, rof) * 255.f;
			int v = (int)fv;
			if (v < min) min = v;
			if (v > max) max = v;
			map[p] = v;
			map[p + 1] = v;
			map[p + 2] = v;
			map[p + 3] = 255;
		}
	}
	SDL_Log("Range = %d - %d", min, max);
}