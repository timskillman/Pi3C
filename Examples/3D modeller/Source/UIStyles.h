#pragma once
#include "Pi3Ccolours.h"
#include <string>

class UI_Style {
public:

	uint32_t backColour = Pi3Ccolours::Raspberry; // 0xcccccc;
	uint32_t iconColour = Pi3Ccolours::Linen;
	uint32_t selectColour = Pi3Ccolours::LightCoral; // 0x00ffff;
	uint32_t highlight = Pi3Ccolours::Salmon;
	uint32_t borderColour = Pi3Ccolours::Red;

	UI_Style() {}

	void LoadUIstyle(const std::string& file);

};