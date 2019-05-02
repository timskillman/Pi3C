#pragma once
#include <vector>
#include "glutess.h"

extern void tessclean(std::vector<std::vector<float>> *contours_in, std::vector<std::vector<float>> *contours_out);
extern void tessunion(std::vector<std::vector<float>> *contours_in, std::vector<float> *tris_out, std::vector<std::vector<float>> *edges);
extern void tessellate(std::vector<std::vector<float>> *contours_in, std::vector<float> *tris_out, std::vector<std::vector<float>> *edges, bool getBounds = false, double winding = GLU_TESS_WINDING_ODD);
