#include "Pi3Cblocks.h"

std::vector<vec2f[6]> texPackUV; //uvs for pixel (cube) sides (top, left, right, front, back, bottom)

void createTexPackUV(std::string file)
{
	static float ts = 1.f / 16.f;
	vec2f tx[6];

	//must be called before creating maps
	for (int i = 0; i < 256; i++) {
		tx[0].x = (float)typToTex[i].topx*ts; tx[0].y = (float)typToTex[i].topy*ts;
		tx[1].x = (float)typToTex[i].sidesx*ts; tx[1].y = (float)typToTex[i].sidesy*ts;
		tx[2].x = (float)typToTex[i].sidesx*ts; tx[2].y = (float)typToTex[i].sidesy*ts;
		tx[3].x = (float)typToTex[i].frontx*ts; tx[3].y = (float)typToTex[i].fronty*ts;
		tx[4].x = (float)typToTex[i].backx*ts; tx[4].y = (float)typToTex[i].backy*ts;
		tx[5].x = (float)typToTex[i].topx*ts; tx[5].y = (float)typToTex[i].topx*ts;
		//texPackUV.push_back(tx);
	}
}

void addQuadTopBottom(Pi3Cmesh &mesh, int x, int h, int y, vec2f tex, int tb)
{
	static float ts = 1.f / 16.f;
	vec3f norm(0, 1.f - (float)(tb * 2), 0);
	float hh = (float)(h - tb);
	mesh.addPackedVert(vec3f((float)(x + 1), hh, (float)y), norm, tex + vec2f(ts, 0), 0);
	mesh.addPackedVert(vec3f((float)x, hh, (float)(y + tb)), norm, tex, 0);
	mesh.addPackedVert(vec3f((float)x, hh, (float)(y + 1 - tb)), norm, tex + vec2f(0, ts), 0);

	mesh.addPackedVert(vec3f((float)(x + 1), hh, (float)y), norm, tex + vec2f(ts, 0), 0);
	mesh.addPackedVert(vec3f((float)(x + tb), hh, (float)(y + 1)), norm, tex + vec2f(0, ts), 0);
	mesh.addPackedVert(vec3f((float)(x + 1 - tb), hh, (float)(y + 1)), norm, tex + vec2f(ts, ts), 0);
}

void addQuadLeftRight(Pi3Cmesh &mesh, int x, int h, int y, vec2f tex, int lr)
{
	static float ts = 1.f / 16.f;
	static vec3f norm((float)(lr*2) - 1.f, 0, 0);
	float xx = (float)(x + lr);
	mesh.addPackedVert(vec3f(xx, (float)h, (float)y), norm, tex, 0);
	mesh.addPackedVert(vec3f(xx, (float)(h - lr), (float)(y + 1 - lr)), norm, tex + vec2f(ts, 0), 0);
	mesh.addPackedVert(vec3f(xx, (float)(h - 1), (float)(y + lr)), norm, tex + vec2f(0, ts), 0);

	mesh.addPackedVert(vec3f(xx, (float)h, (float)(y + 1 - lr)), norm, tex + vec2f(ts, 0), 0);
	mesh.addPackedVert(vec3f(xx, (float)(h - 1), (float)(y + 1 - lr)), norm, tex + vec2f(ts, ts), 0);
	mesh.addPackedVert(vec3f(xx, (float)(h - 1), (float)(y + lr)), norm, tex + vec2f(0, ts), 0);
}

void addQuadFrontBack(Pi3Cmesh &mesh, int x, int h, int y, vec2f tex, int fb)
{
	static float ts = 1.f / 16.f;
	static vec3f norm(0, 0, (float)(2*fb) - 1.f);
	float yy = (float)(y + fb);
	mesh.addPackedVert(vec3f((float)x, (float)h, yy), norm, tex, 0);
	mesh.addPackedVert(vec3f((float)(x + fb), (float)h, yy), norm, tex + vec2f(ts, 0), 0);
	mesh.addPackedVert(vec3f((float)x, (float)(h - 1), yy), norm, tex + vec2f(0, ts), 0);

	mesh.addPackedVert(vec3f((float)(x + fb), (float)h, yy), norm, tex + vec2f(ts, 0), 0);
	mesh.addPackedVert(vec3f((float)(x + fb), (float)(h - 1), yy), norm, tex + vec2f(ts, ts), 0);
	mesh.addPackedVert(vec3f((float)x, (float)(h - 1), yy), norm, tex + vec2f(0, ts), 0);
}

void createMCmap(std::vector<uint8_t> &map, uint32_t mapWidth, uint32_t mapHeight, uint32_t mapDepth, Pi3Cmesh &mesh, int32_t x, int32_t y, int32_t w, int32_t h)
{
	//std::vector<float> &verts = mesh.verts;
	//uint32_t stride = mesh.stride;
	//uint32_t v = 0;  //vertex pointer;
	uint32_t pitch = mapWidth * mapDepth;

	//float ts = 1.f / 16.f;  //texture map step (assuming 16 images wide on texture pack)

	//vec3f norm_up(0, 1.f, 0);
	for (int32_t yy = y; yy < (y + h); yy++) {
		for (int32_t xx = x; xx < (x + w); xx++) {
			uint32_t ptr = (xx + yy * mapWidth)*mapDepth;
			uint32_t p = ptr + mapDepth;

			//Create all ground surfaces ...
			while (p > ptr) {

				//skip air blocks (but create faces adjacent to air blocks)
				while (map[--p] == 0) {
					if (map[p - mapDepth] != 0) {
						addQuadLeftRight(mesh, xx, p - ptr, yy, texPackUV[map[p]][1], 0);
					}
					if (map[p + mapDepth] != 0) {
						addQuadLeftRight(mesh, xx, p - ptr, yy, texPackUV[map[p]][2], 1);
					}
					if (map[p - pitch] != 0) {
						addQuadFrontBack(mesh, xx, p - ptr, yy, texPackUV[map[p]][3], 0);
					}
					if (map[p + pitch] != 0) {
						addQuadFrontBack(mesh, xx, p - ptr, yy, texPackUV[map[p]][4], 1);
					}
				}

				//create ground plane ...
				addQuadTopBottom(mesh, xx, p - ptr, yy, texPackUV[map[p]][0], 0);

				//skip non-air blocks (but create 'air' faces adjacent to solid blocks)
				while (p > ptr && map[--p] != 0) {
					if (map[p - mapDepth] == 0) {
						addQuadLeftRight(mesh, xx, p - ptr, yy, texPackUV[map[p]][2], 1);
					}
					if (map[p + mapDepth] == 0) {
						addQuadLeftRight(mesh, xx, p - ptr, yy, texPackUV[map[p]][1], 0);
					}
					if (map[p - pitch] == 0) {
						addQuadFrontBack(mesh, xx, p - ptr, yy, texPackUV[map[p]][4], 1);
					}
					if (map[p + pitch] == 0) {
						addQuadFrontBack(mesh, xx, p - ptr, yy, texPackUV[map[p]][3], 0);
					}
				}

				//create bottom plane (underside)
				if (p > ptr) addQuadTopBottom(mesh, xx, p - ptr, yy, texPackUV[map[p]][5], 1);
			}
		}
	}
}
