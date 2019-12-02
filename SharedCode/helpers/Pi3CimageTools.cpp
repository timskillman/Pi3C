#include "Pi3CimageTools.h"

namespace Pi3CimageTools {

	void drawLine(uint32_t* pic, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t col, uint32_t width)
	{
		int dx = x2 - x1;
		int sx = (dx > 0) - (dx < 0); //sign(dx)
		int dy = y2 - y1;
		int sy = ((dy > 0) - (dy < 0)) * width; //sign(dy)
		dx = abs(dx);
		dy = abs(dy);
		y1 *= width;
		y2 *= width;

		if (dx > dy)
		{
			int e = dx / 2;
			while (x1 != x2) {
				pic[x1 + y1] = col;
				x1 += sx;
				//pic[x1+y1] = col; //solid steps
				e -= dy;
				if (e < 0) { e += dx; y1 += sy; }
			}
		}
		else
		{
			int e = dy / 2;
			while (y1 != y2) {
				pic[x1 + y1] = col;
				y1 += sy;
				//pic[x1+y1] = col; //solid steps
				e -= dx;
				if (e < 0) { e += dy; x1 += sx; }
			}
		}
		pic[x1 + y1] = col;
	}

	//void stackPoint(int32_t x, int32_t y, uint32_t& w, uint32_t* stack, uint32_t& stp, int32_t& sx, int32_t& sy, int32_t& bx, int32_t& by)
	//{
	//	stack[stp++] = x + y * w;
	//	if (x < sx) sx = x; else if (x > bx) bx = x;
	//	if (y < sy) sy = y; else if (y > by) by = y;
	//}

#define stackPos(x,y) stack[stp++] = x + y * width

	void floodfill(uint32_t* pic, int32_t x, int32_t y, uint32_t fill_colour, uint32_t width, uint32_t height, uint32_t* stack, uint32_t stack_size)
	{
		uint32_t stp = 0; //stack pointer
		uint32_t stc = 0; //stack chaser

		int32_t sx = width; //Initialize fill bounding area (sx,sy,bx,by)
		int32_t sy = height;
		int32_t bx = 0;
		int32_t by = 0;

		uint32_t pix = pic[x + y*width];	//Fetch colour from this initial point (the colour to be filled)
		if (pix == fill_colour) return;		//if colour at (x,y) is the same as the fill_colour then exit.

		bool own_stack = false;
		if (stack == nullptr) {
			stack_size = width * height;
			stack = new uint32_t[stack_size];
			own_stack = true;
		}

		stackPos(x, y); //Add this initial point to the stack
		pic[x + y * width] = fill_colour;

		while (stc < stp)
		{
			uint32_t p = stack[stc++]; //pointer to pixel in the image
			if (stc > stack_size) break;
			uint32_t tx = p % width;
			uint32_t ty = p / width;

			if ((tx > 0) && (pic[p - 1] == pix)) { stackPos(tx - 1, ty); pic[p - 1] = fill_colour; }
			if ((tx < (width - 1)) && (pic[p + 1] == pix)) { stackPos(tx + 1, ty); pic[p + 1] = fill_colour; }
			if ((ty > 0) && (pic[p - width] == pix)) { stackPos(tx, ty - 1); pic[p - width] = fill_colour; }
			if ((ty < (height - 1)) && (pic[p + width] == pix)) { stackPos(tx, ty + 1); pic[p + width] = fill_colour; }
		}

		if (own_stack) {
			delete stack;
		}
	}

	void fillStack(uint32_t* pic, uint32_t* stack, uint32_t fill_colour, uint32_t maxstack)
	{
		for (uint32_t i = 0; i < maxstack; i++) pic[stack[i]] = fill_colour;
	}

	float linePoint(float p1x, float p1y, float p2x, float p2y, float x, float y)
	{
		int ca = p2x - p1x;
		int cb = p1y - p2y;
		float fv1 = (float)(ca * y + cb * x - p1x * cb - p1y * ca);
		return fv1 / sqrtf((float)(ca * ca + cb * cb));
	}

	float angleBetweenTwoVectors(int v1x, int v1y, int v2x, int v2y)
	{
		float prod = (float)(v1x * v2y - v1y * v2x);
		int d1 = v1x * v1x + v1y * v1y;
		return fabs((prod < 0 ? -1.0 : 1.0) * acos((float)(v1x * v2x + v1y * v2y) / (sqrt(v1x * v1x + v1y * v1y) * sqrt(v2x * v2x + v2y * v2y))));
	}

	bool nopixelsabove(uint32_t *image, uint32_t &p, uint32_t width, uint32_t colour, std::vector<uint32_t> &startPoints)
	{
		//p = ptr to pixel found in image that might be a starting point

		//check top line points ...
		uint32_t sp = p;
		//if (p < width) {
		//	while (p < width && image[p++] != 0);
		//	return true;
		//}

		//check if pixels above remain clear while pixels on line are not.
		//Continue until either pixels above are not clear or pixels on line become clear.
		uint32_t sx = p % width;
		uint32_t pixelAbove = p - width;
		if ((sx > 0 && (image[pixelAbove - 1] & 1) != 0) || (image[pixelAbove] & 1) != 0) {
			if ((image[p] & 1) != 0) while ((image[++p] & 1) != 0);
			return false;
		}

		while ((image[pixelAbove++] & 1) == 0 && (image[p++] & 1) != 0);

		if ((image[--pixelAbove] & 1) != 0) {
			if ((image[p] & 1) != 0) while ((image[++p] & 1) != 0);
			return false;
		}
		image[sp] = colour;
		startPoints.push_back(sp);

		return true;
	}

	const int r[16] = { 0, -1, 1, -1, 1, 0, 1, 1, 0, 1, -1, 1, -1, 0, -1, -1 };
	const float hr[16] = { 0, -0.5f, 0.5f, -0.5f, 0.5f, 0, 0.5f, 0.5f, 0, 0.5f, -0.5f, 0.5f, -0.5f, 0, -0.5f, -0.5f };
	int points[1000000];

	void crawlImage(uint32_t * image, uint32_t width, uint32_t height, Pi3ClinPath& contours, std::vector<uint32_t>& startPoints, uint32_t colour)
	{
		int x = 0; int y = 0;
		int px = 0; int py = 0;
		int pc = 1; int opc = 2;
		//for (size_t sp = 0; sp < 2; sp++) {

		colour = 0xff000000 | colour & 0xfffffe;

		//uint32_t thisColour = (0xff000000 | rand()) & 0xfffffffe; //cannot have the bottom bit set

		for (size_t sp = 0; sp < startPoints.size() - 1; sp++) {
			//contours.linContours.emplace_back();
			//Pi3ClinContour &contour = contours.linContours.back();
			Pi3ClinContour contour;

			uint32_t startx = startPoints[sp] % width;
			uint32_t starty = startPoints[sp] / width;

			//if (!lonepixel(image, startx, starty * width, width)) {

				x = startx;
				y = starty;
				int xr = x + r[0];
				int yr = y + r[1];
				int lpx = x;
				int lpy = y;
				uint32_t rot = 0;
				uint32_t rots = 0;

				size_t p = 0;
				points[p++] = x;
				points[p++] = y;
				size_t lp = 0;

				while ((xr != startx || yr != starty) && rots < 8) {

					uint32_t pp = xr + yr * width;
					uint32_t& pix = image[pp];

					//if ((pix != 0) && (pix & 1) == 0 && pix!=thisColour) {
					//	rots = 8; break;
					//}

					//only crawl white or thisColour pixels ...
					if ((pix & 1) || (pix == colour)) {

						rot = (rot + 12) % 16;	//rotate 'anticlockwise' -2 positions (effectively -90 degrees) to check continuation

						if ((p - lp) > 2) {
							size_t tp = lp + 2;
							while ((tp < p) && (std::abs(linePoint(lpx, lpy, xr, yr, points[tp], points[tp + 1])) < 1.02f)) tp += 2;
							if (tp < p) {		//found a stray point ...
								int ax = points[p - 4]; int ay = points[p - 3];
								//float d = std::abs(linePoint(lpx, lpy, xr, yr, ax, ay));
								float a = angleBetweenTwoVectors(lpx - ax, lpy - ay, points[p - 2] - ax, points[p - 1] - ay);
								//if (d > 0.5f) {
								if (a < 2.6f) {
									contour.points.push_back(vec2f((float)ax, (float)ay));
								}

								lpx = points[p - 2]; // points[tp];
								lpy = points[p - 1]; // points[tp + 1];

								contour.points.push_back(vec2f((float)lpx, (float)lpy));
								lp = p;
							}
						}

						points[p++] = xr;
						points[p++] = yr;

						x = xr; y = yr;
						pix = colour; //tread yellow pixels
						rots = 0;
					}
					else
					{
						rot = (rot + 2) % 16;
						rots++;
					}

					xr = x + r[rot];
					yr = y + r[rot + 1];
				}

				if (rots > 7) {
					//undo path if found another path ...
					//if (p < 40) {
					for (uint32_t j = 0; j < p; j += 2) {
						uint32_t pp = points[j] + points[j + 1] * width;
						image[pp] = colour; // 0xffffffff;
					}
					//}
				}
				else {
					contour.points.push_back(vec2f((float)startx, (float)starty));
					contours.append(contour);
					//SDL_Log("Finished path %d", sp);
				}

			}
	}

	void traceImage(uint32_t* image, uint32_t width, uint32_t height, Pi3ClinPath &PathOut)
	{
		bool finished = false;
		uint32_t count = 0;
		uint32_t imagesize = width * height;
		uint32_t colour = 0xfffffffe;

		while (!finished) {
			uint32_t i = 0, c = 0;
			std::vector<uint32_t> startPoints;
			while (i < imagesize) {
				while ((image[++i] & 1) == 0);
				if (nopixelsabove(image, i, width, 0xff000001 | (c << 1), startPoints)) c++;
			}
			count++;
			finished = (c < 2 || count>50);

			crawlImage(image, width, height, PathOut, startPoints, colour);

			for (uint32_t i = 0; i < imagesize; i++) {
				if ((image[i] & 1) && (image[i] != 0xffffffff)) image[i] = 0;
			}
		}
	}

}
