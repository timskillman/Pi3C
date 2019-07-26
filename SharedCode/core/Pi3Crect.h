#pragma once

class Pi3Cpointi {
public:
	Pi3Cpointi() : x(0), y(0) {}
	Pi3Cpointi(int32_t x, int32_t y) : x(x), y(y) {}

	Pi3Cpointi & operator += (Pi3Cpointi const & v)
	{
		x += v.x;
		y += v.y;
		return *this;
	}

	Pi3Cpointi & operator -= (Pi3Cpointi const & v)
	{
		x -= v.x;
		y -= v.y;
		return *this;
	}

	int32_t x;
	int32_t y;
};

class Pi3Crect {
public:

	Pi3Crect() : x(0), y(0), width(0), height(0) {}
	Pi3Crect(const float x, const float y, const float width, const float height) : x(x), y(y), width(width), height(height) {}

	bool touch(const float px, const float py) { return (px > x && px<(x + width) && py>y && py < (y + height)); }

	float x;
	float y;
	float width;
	float height;
};

class Pi3Crecti {
public:

	Pi3Crecti() : x(0), y(0), width(0), height(0) {}
	Pi3Crecti(const int32_t x, const int32_t y, const int32_t width, const int32_t height) : x(x), y(y), width(width), height(height) {}

	bool touch(const int32_t px, const int32_t py) { return (px > x && px<(x + width) && py>y && py < (y + height)); }

	int32_t x;
	int32_t y;
	int32_t width;
	int32_t height;
};