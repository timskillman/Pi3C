#pragma once

#include <Pi3Cimgui.h>

class perlinGUI {
public:

	Pi3Cimgui::rectStyle bsHeading;

	perlinGUI(Pi3Cimgui& gui);

	void update(Pi3Cimgui& gui, float& amplification, float& pz, float& frequency, uint8_t& octaves, bool& updateMap);
	
};
