#include "PerlinGUI.h"

perlinGUI::perlinGUI(Pi3Cimgui& gui)
{
	gui.setImagePath("icons");
	
	bsHeading.textColour = 0xffffff;
	bsHeading.highlightColour = 0x00ffff;
	bsHeading.buttonColour = 0x808080;
	bsHeading.top = 0;
	bsHeading.bottom = 0;
	bsHeading.left = 0;
	bsHeading.right = 0;
}

void perlinGUI::update(Pi3Cimgui& gui, float& amplification, float& pz, float& frequency, uint8_t& octaves, bool& updateMap)
{
	double v = (double)amplification;
	gui.Begin();
	gui.setButtonStyle(bsHeading);

	gui.Label("Perlin Noise Map Generator");
	gui.nextLine();

	gui.Label("Smoothing:", 120);
	gui.sameLine();
	gui.SliderH("", 0, 2, v, 300, 24);
	if ((float)v != amplification) { amplification = (float)v; updateMap = true; }
	gui.nextLine();

	gui.Label("Octaves:", 120);
	gui.sameLine();
	v = (double)octaves;
	gui.SliderH("", 1, 10, v, 300, 24);
	if ((uint8_t)v != octaves) { octaves = (uint8_t)v; updateMap = true; }
	gui.nextLine();

	gui.Label("Z position:", 120);
	gui.sameLine();
	v = (double)pz;
	gui.SliderH("", 0, 100, v, 300, 24);
	if ((uint8_t)v != pz) { pz = (uint8_t)v; updateMap = true; }
	gui.nextLine();

	gui.Label("Scale:", 120);
	gui.sameLine();
	v = (double)frequency;
	gui.SliderH("", 0, 100, v, 300, 24);
	if ((uint8_t)v != frequency) { frequency = (uint8_t)v; updateMap = true; }
	gui.nextLine();
}