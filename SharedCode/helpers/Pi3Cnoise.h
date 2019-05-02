#pragma once

#include <vector>

class perlin {
public:

	perlin(int repeat) { init(repeat); }
	~perlin() { p.resize(0); }

	float noise(float x, float y, float z);
	float octave(float x, float y, float z, int octaves, float persistence);

private:
	void init(int repeat);
	int inc(int num);
	float grad(int hash, float x, float y, float z);
	float fade(float t);
	float lerp(float a, float b, float x);

	std::vector<int> p;
	int repeat;
};
