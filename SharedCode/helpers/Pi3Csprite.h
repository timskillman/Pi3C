#pragma once
#include "Pi3Cvector.h"
#include <stdint.h>
#include <vector>

struct Pi3CspriteTransition {

	enum transition {
		snap,
		fade,
		leftRight,
		rightLeft,
		topBottom,
		bottomTop,
		zoomIn,
		zoomOut,
	};

	int TransitionTime = 1000;		//Total time to do transition (milliseconds)
	vec3f startPos{ 0,0,0 };
	vec3f endPos{ 0,0,0 };

};

struct Pi3CspriteSheetInfo {

	Pi3CspriteSheetInfo() {}

	Pi3CspriteSheetInfo(std::string file, int tilex, int tiley, int maxframes, int framestart, int framecount) {
		filename = file;
		tileX = tilex;
		tileY = tiley;
		maxFrames = maxframes;
		frameStart = framestart;
		frameCount = framecount;
	}

	vec2f getTilePosition(uint32_t framePos) {
		int tilePos = frameStart + (framePos % maxFrames);
		float tx = 1.f / (float)tileX, ty = 1.f / (float)tileY;
		return vec2f((float)(tilePos % tileX) * tx, (float)(tilePos / tileX) * ty);
	}

	vec2f getTileSize() { return vec2f(1.f / (float)tileX, 1.f / (float)tileY); }

	std::string filename;
	int tileX = 1;					//No of tiles across
	int tileY = 1;					//No of tiles high
	int maxFrames = 1;				//Max no of animation frames in sheet 
	int frameStart = 0;				//Start of frame in sheet
	int frameCount = 1;				//No of frames in this animation
	int animSpeed = 1;				//Frame speed in milliseconds

};