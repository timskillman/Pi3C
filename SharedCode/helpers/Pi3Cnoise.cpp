#include "Pi3Cnoise.h"

// Code based on Adrian Biagioli's (flalfa2) implementation of Perlin noise 
// https://gist.github.com/Flafla2/f0260a861be0ebdeef76

static const int permutation[] = { 151,160,137,91,90,15,							// Hash lookup table as defined by Ken Perlin.  This is a randomly
	131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,	// arranged array of all numbers from 0-255 inclusive.
	190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
	88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
	77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
	102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
	135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
	5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
	223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
	129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
	251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
	49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
	138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
};

void perlin::init(int repeat)
{
	this->repeat = repeat;
	p.resize(512);
	for (int x = 0; x < 512; x++) {
		p[x] = permutation[x % 256];
	}
}

float perlin::octave(float x, float y, float z, int octaves, float persistence)
{
	float total = 0;
	float frequency = 1.f;
	float amplitude = 1.f;
	float max = 0;
	for (int i = 0; i < octaves; i++) {
		total += noise(x * frequency, y * frequency, z * frequency) * amplitude;

		max+= amplitude;

		amplitude *= persistence;
		frequency *= 2;
	}

	return total / max;
}

float perlin::noise(float x, float y, float z)
{

	if (repeat > 0) {									// If we have any repeat on, change the coordinates to their "local" repetitions
		if (x > (float)repeat) x -= (float)repeat;
		if (y > (float)repeat) y -= (float)repeat;
		if (z > (float)repeat) z -= (float)repeat;
	}

	int xi = (int)x & 255;								// Calculate the "unit cube" that the point asked will be located in
	int yi = (int)y & 255;								// The left bound is ( |_x_|,|_y_|,|_z_| ) and the right bound is that
	int zi = (int)z & 255;								// plus 1.  Next we calculate the location (from 0.0 to 1.0) in that cube.
	float xf = x - (float)xi;								// We also fade the location to smooth the result.
	float yf = y - (float)yi;
	float zf = z - (float)zi;
	float u = fade(xf);
	float v = fade(yf);
	float w = fade(zf);

	int aaa = p[p[p[xi] + yi] + zi];
	int aba = p[p[p[xi] + inc(yi)] + zi];
	int aab = p[p[p[xi] + yi] + inc(zi)];
	int abb = p[p[p[xi] + inc(yi)] + inc(zi)];
	int baa = p[p[p[inc(xi)] + yi] + zi];
	int bba = p[p[p[inc(xi)] + inc(yi)] + zi];
	int bab = p[p[p[inc(xi)] + yi] + inc(zi)];
	int bbb = p[p[p[inc(xi)] + inc(yi)] + inc(zi)];

	float x1, x2, y1, y2;
	x1 = lerp(grad(aaa, xf, yf, zf),				// The gradient function calculates the dot product between a pseudorandom
		grad(baa, xf - 1, yf, zf),					// gradient vector and the vector from the input coordinate to the 8
		u);											// surrounding points in its unit cube.
	x2 = lerp(grad(aba, xf, yf - 1, zf),			// This is all then lerped together as a sort of weighted average based on the faded (u,v,w)
		grad(bba, xf - 1, yf - 1, zf),				// values we made earlier.
		u);
	y1 = lerp(x1, x2, v);

	x1 = lerp(grad(aab, xf, yf, zf - 1),
		grad(bab, xf - 1, yf, zf - 1),
		u);
	x2 = lerp(grad(abb, xf, yf - 1, zf - 1),
		grad(bbb, xf - 1, yf - 1, zf - 1),
		u);
	y2 = lerp(x1, x2, v);

	return (lerp(y1, y2, w) + 1) / 2;
}

int perlin::inc(int num) {
	return (repeat > 0) ? (num++) % repeat : num++;
}

float perlin::grad(int hash, float x, float y, float z) {
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

float perlin::fade(float t) {
	return t * t * t * (t * (t * 6 - 15) + 10);			// 6t^5 - 15t^4 + 10t^3
}

float perlin::lerp(float a, float b, float x) {
	return a + x * (b - a);
}


//// Function to linearly interpolate between a0 and a1
//// Weight w should be in the range [0.0, 1.0]
//float lerp(float a0, float a1, float w) {
//	return a0 + w*(a1 - a0);
//}
//
//// Computes the dot product of the distance and gradient vectors.
//float dotGridGradient(int ix, int iy, float x, float y) {
//
//	// Precomputed (or otherwise) gradient vectors at each grid node
//	extern float Gradient[IYMAX][IXMAX][2];
//
//	// Compute the distance vector
//	float dx = x - (float)ix;
//	float dy = y - (float)iy;
//
//	// Compute the dot-product
//	return (dx*Gradient[iy][ix][0] + dy * Gradient[iy][ix][1]);
//}
//
//// Compute Perlin noise at coordinates x, y
//float perlin(float x, float y) {
//
//	// Determine grid cell coordinates
//	int x0 = int(x);
//	int x1 = x0 + 1;
//	int y0 = int(y);
//	int y1 = y0 + 1;
//
//	// Determine interpolation weights
//	// Could also use higher order polynomial/s-curve here
//	float sx = x - (float)x0;
//	float sy = y - (float)y0;
//
//	// Interpolate between grid point gradients
//	float n0, n1, ix0, ix1, value;
//	n0 = dotGridGradient(x0, y0, x, y);
//	n1 = dotGridGradient(x1, y0, x, y);
//	ix0 = lerp(n0, n1, sx);
//	n0 = dotGridGradient(x0, y1, x, y);
//	n1 = dotGridGradient(x1, y1, x, y);
//	ix1 = lerp(n0, n1, sx);
//	value = lerp(ix0, ix1, sy);
//
//	return value;
//}
