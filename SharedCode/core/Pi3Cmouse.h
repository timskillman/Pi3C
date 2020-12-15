#pragma once
#include "Pi3Cvector.h"

struct MouseParams {

	bool anyButton() { return LeftButton || MiddleButton || RightButton; }

	void resetButtons() {
		RightButton = LeftButton = MiddleButton = drag = false;
	}

	void reset() {
		x = 0; y = 0; wheel = 0;
		up = false; wasDragging = false;
		resetButtons();
		deltaXY = {}; XY = {};
	}

	int x = 0;
	int y = 0;
	int wheel = 0;
	bool drag = false;
	bool up = false;
	bool wasDragging = false;
	bool RightButton = false;
	bool MiddleButton = false;
	bool LeftButton = false;
	vec2f deltaXY = {};
	vec2f XY = {};
};
