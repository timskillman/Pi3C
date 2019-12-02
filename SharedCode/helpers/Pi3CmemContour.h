#pragma once

#include "Pi3Ccontour.h"
#include "Pi3Ctexture.h"
#include <memory>


class Pi3CmlinPath : public Pi3ClinPath
{
public:

	Pi3CmlinPath(const char* file) { open(file); }

	void open(const char* filepath);
	void drawToTexture(std::shared_ptr<Pi3Ctexture> tex, float sc);
	void fillToTexture(std::shared_ptr<Pi3Ctexture> tex, float sc);
	
};

