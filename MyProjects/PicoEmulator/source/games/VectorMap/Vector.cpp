#include "Vector.h"
#include <iostream>

#define TRIANGLES 0
#define TRIANGLE_FAN 1
#define TRI_3D 2
#define TRIFAN_3D 3

int8_t vectorArt::SHEILD[] = {
	TRIANGLE_FAN,
	255,255,0, 8, 0,0,3,2,4,6,2,5,0,6,-2,5,-4,6,-3,2
};

int8_t vectorArt::CASTLE_TURRET[] = {
	TRIFAN_3D,
	80,80,80, 2, -10,0,0, -10,0,5, -10,7,5, //Entrance
	180,180,180, 2, 0,0,0, 0,20,0, -5,7,0, -5,0,0, //Front wall
	180,180,180, 4, -15,20,0, 0,20,0, -5,7,0, -10,7,0, -10,0,0, -15,0,0,
	80,80,80, 2, 0,0,0, 0,0,15, 0,20,15, 0,20,0, //Right wall
	220,50,50, 2, -15,20,0, -15,20,15, 0,20,15, 0,20,0, //Roof floor
	180,180,180, 2, -16,20,-1, 1,20,-1, 1,22,-1, -16,22,-1, //Front lintel
	80,80,80, 2, 1,20,-1, 1,20,16, 1,22,16, 1,22,-1, //Right lintel
};

int VectorMap::fast_rand() {
	g_seed = (214013 * g_seed + 2531011);
	return (g_seed >> 16) & 0x7FFF;
}

void VectorMap::drawWall(PicoDisplay& pico_display, int32_t x, int32_t z, int32_t y, int32_t w, int32_t h, bool leftwall, Pen col)
{
	pico_display.set_pen(col);
	Point p1 = calc3D(x, y, z);
	int xw = (leftwall) ? x + w : x;
	int zw = (leftwall) ? z : z + w;
	Point p2 = calc3D(xw, y + h, zw);
	Point p3 = calc3D(xw, y, zw);
	drawTriangle(pico_display, p1, p2, p3);
	p3 = calc3D(x, y + h, z);
	drawTriangle(pico_display, p1, p2, p3);
}

void VectorMap::drawRoof(PicoDisplay& pico_display, int32_t x, int32_t z, int32_t y, int32_t w, int32_t h, int32_t d, int32_t slope, Pen roofCol, Pen endCol)
{
	//Draw left front triangle section
	pico_display.set_pen(roofCol);
	Point p1 = calc3D(x + w / 2, y + h, z + slope);
	Point p2 = calc3D(x - 1, y, z - 1);
	Point p3 = calc3D(x + w + 1, y, z - 1);
	drawTriangle(pico_display, p1, p2, p3);

	pico_display.set_pen(endCol);
	p2 = calc3D(x + w / 2, y + h, z + d - slope);
	drawTriangle(pico_display, p1, p2, p3);
	p1 = calc3D(x + w + 1, y, z + d + 1);
	drawTriangle(pico_display, p1, p2, p3);
}

void VectorMap::drawShield(PicoDisplay& pico_display, int32_t x, int32_t y, int32_t w, int32_t h, Pen col)
{
	pico_display.set_pen(col);
	drawTriangle(pico_display, Point(x, y), Point(x - w / 2, y + h), Point(x + w / 2, y + h));
	drawTriangle(pico_display, Point(x, y), Point(x - w / 2.5, y + h * .2), Point(x - w / 2, y + h));
	drawTriangle(pico_display, Point(x, y), Point(x + w / 2.5, y + h * .2), Point(x + w / 2, y + h));
}

void VectorMap::drawFace(PicoDisplay& pico_display, int32_t x, int32_t y, int32_t w, int32_t h, Pen facecol, Pen haircol)
{
	//torso
	pico_display.set_pen(pico_display.create_pen(20, 20, 200));
	drawTriangle(pico_display, Point(x - w, y - h *0.2), Point(x + w, y - h * 0.2), Point(x, y + h * 0.2));

	//hair
	pico_display.set_pen(haircol);
	//pico_display.circle(Point(x - w / 2, PicoDisplay::HEIGHT - (y + h)), h / 5);
	//pico_display.circle(Point(x + w / 2, PicoDisplay::HEIGHT - (y + h)), h / 5);
	pico_display.circle(Point(x, PicoDisplay::HEIGHT - (y + h)), h / 3);
	pico_display.circle(Point(x, PicoDisplay::HEIGHT - (y + h * 0.7)), h / 2);

	//face
	drawShield(pico_display, x, y, w, h, facecol);

	//eyes
	pico_display.set_pen(pico_display.create_pen(0, 0, 0));
	pico_display.circle(Point(x + w / 4, PicoDisplay::HEIGHT - (y + h * 0.6)), h / 14);
	pico_display.circle(Point(x - w / 4, PicoDisplay::HEIGHT - (y + h * 0.6)), h / 14);

	//mouth
	pico_display.set_pen(pico_display.create_pen(80, 20, 20));
	drawTriangle(pico_display, Point(x - w / 5, y + h * 0.3), Point(x + w / 5, y + h * 0.3), Point(x, y + h * 0.2));
}

void VectorMap::drawManZ(PicoDisplay& pico_display, int32_t x, int32_t z, int32_t y, int32_t w, int32_t h, int32_t d)
{
	pico_display.set_pen(pico_display.create_pen(240, 120, 80));
	drawTriangle(pico_display, calc3D(x + 1, y + 4, z), calc3D(x + 3, y + 4, z), calc3D(x + 3, y - 1, z + d));
	drawTriangle(pico_display, calc3D(x - 1, y + 4, z), calc3D(x - 3, y + 4, z), calc3D(x - 3, y - 1, z - d));
	//legs
	drawTriangle(pico_display, calc3D(x, y - 2, z), calc3D(x + 2, y - 2, z), calc3D(x + 1, y - 8, z + d));
	drawTriangle(pico_display, calc3D(x, y - 2, z), calc3D(x - 2, y - 2, z), calc3D(x - 1, y - 8, z - d));
	//head
	drawTriangle(pico_display, calc3D(x, y + 4, z), calc3D(x - 2, y + 5, z), calc3D(x + 2, y + 7, z));
	pico_display.set_pen(pico_display.create_pen(80, 20, 0));
	drawTriangle(pico_display, calc3D(x, y + 8, z), calc3D(x - 2, y + 5, z), calc3D(x + 2, y + 7, z));
	//body
	pico_display.set_pen(pico_display.create_pen(0, 0, 120));
	drawTriangle(pico_display, calc3D(x, y-2, z), calc3D(x-2, y+4, z), calc3D(x+2, y+4, z));
	drawTriangle(pico_display, calc3D(x, y+3, z), calc3D(x - 2, y - 2, z), calc3D(x + 2, y - 2, z));
}

void VectorMap::drawManX(PicoDisplay& pico_display, int32_t x, int32_t z, int32_t y, int32_t w, int32_t h, int32_t d)
{
	pico_display.set_pen(pico_display.create_pen(240, 120, 80));
	drawTriangle(pico_display, calc3D(x , y + 4, z + 1), calc3D(x , y + 4, z + 3), calc3D(x + d, y - 1, z  + 3));
	drawTriangle(pico_display, calc3D(x, y + 4, z - 1), calc3D(x , y + 4, z - 3), calc3D(x - d, y - 1, z  - 3));
	//legs
	drawTriangle(pico_display, calc3D(x, y - 2, z), calc3D(x , y - 2, z + 2), calc3D(x + d, y - 8, z  + 1));
	drawTriangle(pico_display, calc3D(x, y - 2, z), calc3D(x , y - 2, z - 2), calc3D(x - d, y - 8, z  - 1));
	//head
	drawTriangle(pico_display, calc3D(x, y + 4, z), calc3D(x , y + 7, z - 2), calc3D(x , y + 5, z + 2));
	pico_display.set_pen(pico_display.create_pen(80, 20, 0));
	drawTriangle(pico_display, calc3D(x, y + 8, z), calc3D(x , y + 7, z - 2), calc3D(x , y + 5, z + 2));
	//body
	pico_display.set_pen(pico_display.create_pen(0, 0, 120));
	drawTriangle(pico_display, calc3D(x, y - 2, z), calc3D(x, y + 4, z - 2), calc3D(x, y + 4, z + 2));
	drawTriangle(pico_display, calc3D(x, y + 3, z), calc3D(x, y - 2, z - 2), calc3D(x, y - 2, z + 2));
}

void VectorMap::drawHouse(PicoDisplay& pico_display, int32_t x, int32_t z, int32_t y, int32_t w, int32_t h, int32_t d, int32_t roofHeight, int32_t slope, int8_t windows)
{
	drawWall(pico_display, x, z, y, w, h, true, pico_display.create_pen(200, 200, 180));
	drawWall(pico_display, x + w, z, y, d, h, false, pico_display.create_pen(180, 180, 150));
	drawRoof(pico_display, x, z, y + h, w, roofHeight, d, slope / 4, pico_display.create_pen(255, 50, 50), pico_display.create_pen(200, 0, 0));

	for (int b = 0; b < 7; b++) {
		if (windows & (1 << b)) {
			drawWall(pico_display, x + 1 + b * 2, z, y + 2, 3, 4, true, pico_display.create_pen(0, 0, 0));
			drawWall(pico_display, x + w, z + 1 + b * 2, y + 2, 3, 4, false, pico_display.create_pen(0, 0, 0));
		}
	}
}

void VectorMap::drawTree(PicoDisplay& pico_display, int32_t x, int32_t z, int32_t y, int32_t w, Pen leaves, Pen bark)
{
	drawWall(pico_display, x - 1, z - 1, y, 2, w, true, bark);
	drawWall(pico_display, x + 1, z - 1, y, 2, w, false, bark);
	pico_display.set_pen(leaves);
	drawTriangle(pico_display, calc3D(x - w, y+w, z - w), calc3D(x, y + w*5, z), calc3D(x + w, y+w, z - w));
	drawTriangle(pico_display, calc3D(x + w, y+w, z - w), calc3D(x, y + w*5, z), calc3D(x + w, y+w, z + w));
}

void VectorMap::init(PicoDisplay& pico_display)
{

}

void VectorMap::update(PicoDisplay& pico_display)
{
	pico_display.set_pen(0, 80, 0);
	pico_display.clear();

	int pw = PicoDisplay::WIDTH;
	int ph = PicoDisplay::HEIGHT;

	Pen p1, p2;

	pico_display.set_pen(20, 150, 20);
	Point xz = calc3D(-x, 0, -z);
	Point xz2 = calc3D(x, 0, z);
	int dx = xz.x / pw;
	int dy = xz.y / ph;
	for (int yr = 0; yr < 3; yr++) {
		for (int xr = 0; xr < 3; xr++) {
			g_seed = ((xr + dx) + (yr + dy) * 100000);
			if (fast_rand() % 10 == 0) {
				drawHouse(pico_display, ((dx + xr) * pw) - xz2.x , ((dy + yr) * ph) - xz2.y, y, w, h, w, h, 0, 0b10100101);
			}
			int c = fast_rand() % 100;
			for (int i = 0; i < c; i++) {
				int tx = ((dx + xr) * pw) - xz.x - fast_rand() % pw;
				int ty = ((dy + yr) * ph) - xz.y - fast_rand() % ph;
				int sz = fast_rand() % 5 + 2;

				int distrib = fast_rand() % 50;
				if (distrib == 0) {
					//pico_display.set_pen(180, 250, 50);
					//tx = ((dx + xr) * pw) + x - fast_rand() % pw;
					//ty = ((dy + yr) * ph) + z - fast_rand() % ph;
					//drawHouse(pico_display, tx, ty, y, w, h, w, h, 0, 0b10100101);
					//drawGrass(pico_display, Point(tx, ty));
				}
				else if (distrib < 5) {
					switch (fast_rand() % 3) {
					case 0: p1 = pico_display.create_pen(20, 150, 20); p2 = pico_display.create_pen(0, 130, 0); break;
					case 1: p1 = pico_display.create_pen(60, 180, 0); p2 = pico_display.create_pen(40, 160, 0); break;
					case 2: p1 = pico_display.create_pen(0, 50, 0); p2 = pico_display.create_pen(0, 30, 0); break;
					}
					pico_display.set_pen(p1);
					drawTriangle(pico_display, Point(tx, ty), Point(tx + sz*2, ty - sz), Point(tx + sz*2, ty + sz * 4));
					pico_display.set_pen(p2);
					drawTriangle(pico_display, Point(tx + sz * 4, ty), Point(tx + sz * 2, ty - sz), Point(tx + sz * 2, ty + sz * 4));
				}
				else if (distrib < 8) {
					pico_display.set_pen(80, 80, 80);
					drawTriangle(pico_display, Point(tx, ty), Point(tx + sz * 4, ty), Point(tx + sz*2, ty + sz));
					drawTriangle(pico_display, Point(tx, ty), Point(tx + sz * 4, ty), Point(tx + sz*2, ty - sz));
				}
				else {
					pico_display.set_pen(250, 10 + distrib * 8, 10);
					pico_display.pixel(Point(tx, PicoDisplay::HEIGHT - ty));
					pico_display.pixel(Point(tx - 1, PicoDisplay::HEIGHT - ty));
					pico_display.pixel(Point(tx + 1, PicoDisplay::HEIGHT - ty));
					pico_display.pixel(Point(tx, PicoDisplay::HEIGHT - ty - 1));
					pico_display.pixel(Point(tx, PicoDisplay::HEIGHT - ty + 1));
				}

			}
		}
	}
	//house
	drawHouse(pico_display, x-40, z+20, y, w, h, w, h, 0, 0b10100101);
	//church
	drawHouse(pico_display, x, z + d, y, w / 2, h * 3, w / 2, h * 4, w,0);
	drawHouse(pico_display, x, z, y, w, h, d, h * 2, d / 2, 0b10101010);

	//town center
	int ox = 40, oz = 40;
	drawHouse(pico_display, x + ox, z + oz + w, y, w, h*2, w, h*2, d,0);
	drawHouse(pico_display, x + ox, z + oz, y, w, h, w, h, d, 0b00111100);
	drawHouse(pico_display, x + ox + w, z + oz + w, y, w, h, w, h, d, 0b00111100);

	ox = 30; oz = 0;
	drawTree(pico_display, x + ox, z + oz, y, 6, pico_display.create_pen(20, 180, 20), pico_display.create_pen(80, 40, 0));
	drawTree(pico_display, x + 40, z + 8, y, 8, pico_display.create_pen(20, 180, 20), pico_display.create_pen(80, 40, 0));
	drawTree(pico_display, x + 60, z - 10, y, 7, pico_display.create_pen(20, 180, 20), pico_display.create_pen(80, 40, 0));

	drawShield(pico_display, 20, 5, 30, 40, pico_display.create_pen(200, 120, 0));
	drawFace(pico_display, 80, 5, 30, 40, pico_display.create_pen(200, 180, 50), pico_display.create_pen(100, 80, 0));

	uint8_t buttonsPressed = (pico_display.is_pressed(pico_display.Y) ? 1 : 0);
	buttonsPressed = buttonsPressed | (pico_display.is_pressed(pico_display.B) ? 2 : 0);
	buttonsPressed = buttonsPressed | (pico_display.is_pressed(pico_display.A) ? 4 : 0);
	buttonsPressed = buttonsPressed | (pico_display.is_pressed(pico_display.X) ? 8 : 0);

	if (buttonsPressed == 1 || buttonsPressed == 4) {
		mandir = 0; walk += wd;
		if (walk < -1 || walk>1) wd = -wd;
	}
	if (buttonsPressed == 2 || buttonsPressed == 8) {
		mandir = 1; walk += wd;
		if (walk < -1 || walk>1) wd = -wd;
	}

	if (mandir==0) drawManX(pico_display, 0, pw / 4, 0, 3, 3, walk); else drawManZ(pico_display, 0, pw / 4, 0, 3, 3, walk);

	x = x + ((buttonsPressed == 1) ? -1 : (buttonsPressed == 4) ? 1 : 0);
	z = z + ((buttonsPressed == 2) ? 1 : (buttonsPressed == 8) ? -1 : 0);


	//if (buttonsPressed == 3) sc = sc + 0.1f;
	//if (buttonsPressed == 12) sc = sc - 0.1f;
	//Check controls
	//if (pico_display.is_pressed(pico_display.Y)) {
	//	x -= 2;
	//}

	//if (pico_display.is_pressed(pico_display.B)) {
	//	z += 2;
	//}

	//if (pico_display.is_pressed(pico_display.A)) {
	//	x += 2;
	//}

	//if (pico_display.is_pressed(pico_display.X)) {
	//	z -= 2;
	//}
}
