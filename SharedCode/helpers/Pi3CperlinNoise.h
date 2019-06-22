#pragma once

#include <stdint.h>
#include <vector>

// Code taken from https://github.com/sol-prog/async_tutorial/blob/master/

// THIS CLASS IS A TRANSLATION TO C++11 FROM THE REFERENCE
// JAVA IMPLEMENTATION OF THE IMPROVED PERLIN FUNCTION (see http://mrl.nyu.edu/~perlin/noise/)
// THE ORIGINAL JAVA IMPLEMENTATION IS COPYRIGHT 2002 KEN PERLIN

// I ADDED AN EXTRA METHOD THAT GENERATES A NEW PERMUTATION VECTOR (THIS IS NOT PRESENT IN THE ORIGINAL IMPLEMENTATION)

class Pi3CperlinNoise {
	// The permutation vector
	std::vector<int> p;

public:
	// Initialize with the reference values for the permutation vector
	Pi3CperlinNoise();
	// Generate a new permutation vector based on the value of seed
	Pi3CperlinNoise(const unsigned int seed);
	// Get a noise value, for 2D images z can have any value
	float noise(const float x, const float y, const float z);
	float octave(float x, float y, float z, int octaves, float persistence, float rof);

	void createMap(uint8_t * map, uint32_t w, uint32_t h, float z, int octaves, float amplitude, float rof);

private:
	//double fade(double t);
	//double lerp(double t, double a, double b);
	//double grad(int hash, double x, double y, double z);

	inline float fade(const float t) {
		return t * t * t * (t * (t * 6.f - 15.f) + 10.f);
	}

	inline float lerp(const float t, const float a, const float b) {
		return a + t * (b - a);
	}

	float grad(const int hash, const float x, const float y, const float z) {
		switch (hash & 0xF)
		{
		case 0x0: return  x + y;
		case 0x1: return -x + y;
		case 0x2: return  x - y;
		case 0x3: return -x - y;
		case 0x4: return  x + z;
		case 0x5: return -x + z;
		case 0x6: return  x - z;
		case 0x7: return -x - z;
		case 0x8: return  y + z;
		case 0x9: return -y + z;
		case 0xA: return  y - z;
		case 0xB: return -y - z;
		case 0xC: return  y + x;
		case 0xD: return -y + z;
		case 0xE: return  y - x;
		case 0xF: return -y - z;
		}
	}

	//inline float grad(const int hash, const float x, const float y, const float z) {
	//	int h = hash & 15;
	//	// Convert lower 4 bits of hash into 12 gradient directions
	//	float u = (h < 8 ? x : y);
	//	float v = h < 4 ? y : h == 12 || h == 14 ? x : z;
	//	return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
	//}

};

