#pragma once

#include "Pi3Ccontour.h"

class Pi3CmlinContour : Pi3ClinContour
{
public:


};


class Pi3CmlinPath : Pi3ClinPath
{
public:

	Pi3CmlinPath(const char* file) { open(file); }

	void open(const char* filepath);
	void drawToTexture(std::shared_ptr<Pi3Ctexture> tex, float sc);
	void fillToTexture(std::shared_ptr<Pi3Ctexture> tex, float sc);

};

