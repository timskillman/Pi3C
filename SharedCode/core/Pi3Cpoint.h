#pragma once
#include <stdint.h>

class Pi3Cpointi {
public:
	Pi3Cpointi() : x(0), y(0) {}
	Pi3Cpointi(int32_t x, int32_t y) : x(x), y(y) {}

	Pi3Cpointi& operator += (Pi3Cpointi const& v)
	{
		x += v.x;
		y += v.y;
		return *this;
	}

	Pi3Cpointi& operator -= (Pi3Cpointi const& v)
	{
		x -= v.x;
		y -= v.y;
		return *this;
	}

	Pi3Cpointi& operator - (Pi3Cpointi const& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		return *this;
	}

	int32_t x;
	int32_t y;
};


