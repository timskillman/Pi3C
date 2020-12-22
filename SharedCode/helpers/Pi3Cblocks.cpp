#include "Pi3Cblocks.h"
#include "FastNoiseLite.h"

void Blocks::createTexPackUV()
{
	static float ts = 1.f / 16.f;
	std::vector<vec2f> tx;
	tx.resize(6);

	//must be called before creating maps
	for (int i = 0; i < 256; i++) {
		tx[0].x = (float)typToTex[i].topx*ts; tx[0].y = (float)typToTex[i].topy*ts;
		tx[1].x = (float)typToTex[i].sidesx*ts; tx[1].y = (float)typToTex[i].sidesy*ts;
		tx[2].x = (float)typToTex[i].sidesx*ts; tx[2].y = (float)typToTex[i].sidesy*ts;
		tx[3].x = (float)typToTex[i].frontx*ts; tx[3].y = (float)typToTex[i].fronty*ts;
		tx[4].x = (float)typToTex[i].backx*ts; tx[4].y = (float)typToTex[i].backy*ts;
		tx[5].x = (float)typToTex[i].topx*ts; tx[5].y = (float)typToTex[i].topx*ts;
		texPackUV.push_back(tx);
	}
}

void Blocks::addQuadTopBottom(Pi3Cmesh &mesh, int x, int h, int z, uint8_t mapVal, uint8_t faceVal, int tb)
{
	static float ts = 1.f / 16.f;
	vec3f norm(0, -(1.f - (float)((1-tb) * 2)), 0);
	float hh = (float)(h);
	vec2f tex = texPackUV[mapVal][faceVal];
	uint32_t col = typToTex[mapVal].col;
	mesh.addPackedVert(vec3f((float)(x + 1), hh, (float)z), norm, tex + vec2f(ts, 0), col);
	mesh.addPackedVert(vec3f((float)x, hh, (float)(z + tb)), norm, tex, col);
	mesh.addPackedVert(vec3f((float)x, hh, (float)(z + (1 - tb))), norm, tex + vec2f(0, ts), col);

	mesh.addPackedVert(vec3f((float)(x + 1), hh, (float)z), norm, tex + vec2f(ts, 0), col);
	mesh.addPackedVert(vec3f((float)(x + tb), hh, (float)(z + 1)), norm, tex + vec2f(0, ts), col);
	mesh.addPackedVert(vec3f((float)(x + (1 - tb)), hh, (float)(z + 1)), norm, tex + vec2f(ts, ts), col);
}

void Blocks::addQuadLeftRight(Pi3Cmesh &mesh, int x, int h, int z, uint8_t mapVal, uint8_t faceVal, int lr)
{
	static float ts = 1.f / 16.f;
	static vec3f norm((1.f - (float)((1 - lr) * 2)), 0, 0);
	float xx = (float)(x);
	vec2f tex = texPackUV[mapVal][faceVal];
	uint32_t col = typToTex[mapVal].colSides;
	mesh.addPackedVert(vec3f(xx, (float)h, (float)(z + lr)), norm, tex + vec2f(ts, 0), col);
	mesh.addPackedVert(vec3f(xx, (float)h, (float)(z + (1 - lr))), norm, tex, col);
	mesh.addPackedVert(vec3f(xx, (float)(h - 1), (float)(z + (1 - lr))), norm, tex + vec2f(0, ts), col);

	mesh.addPackedVert(vec3f(xx, (float)h, (float)(z + lr)), norm, tex + vec2f(ts, 0), col);
	mesh.addPackedVert(vec3f(xx, (float)(h - 1), (float)(z + (1 - lr))), norm, tex + vec2f(0, ts), col);
	mesh.addPackedVert(vec3f(xx, (float)(h - 1), (float)(z + lr)), norm, tex + vec2f(ts, ts), col);
}

void Blocks::addQuadFrontBack(Pi3Cmesh &mesh, int x, int h, int z, uint8_t mapVal, uint8_t faceVal, int fb)
{
	static float ts = 1.f / 16.f;
	static vec3f norm(0, 0, 1.f); // (1.f - (float)((1 - fb) * 2)));
	float zz = (float)(z);
	vec2f tex = texPackUV[mapVal][faceVal];
	uint32_t col = typToTex[mapVal].colSides;
	mesh.addPackedVert(vec3f((float)(x +(1-fb)), (float)h, zz), norm, tex + vec2f(ts, 0), col);
	mesh.addPackedVert(vec3f((float)(x + fb), (float)h, zz), norm, tex, col);
	mesh.addPackedVert(vec3f((float)(x + fb), (float)(h - 1), zz), norm, tex + vec2f(0, ts), col);

	mesh.addPackedVert(vec3f((float)(x + (1 - fb)), (float)h, zz), norm, tex + vec2f(ts, 0), col);
	mesh.addPackedVert(vec3f((float)(x + fb), (float)(h - 1), zz), norm, tex + vec2f(0, ts), col);
	mesh.addPackedVert(vec3f((float)(x + (1 - fb)), (float)(h - 1), zz), norm, tex + vec2f(ts, ts), col);
}

Pi3Cmesh Blocks::createTestMap(int w, int h, int d) 
{
	FastNoiseLite noise;
	noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
	noise.SetFractalType(FastNoiseLite::FractalType_Ridged);

	std::vector<uint8_t> map;
	//map.resize(w * h * d);
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			int mp = x * y * d;
			//int glevel = d / 2 + (rand() % 40) - 20;
			int glevel = d/2 + (int)(noise.GetNoise((float)x, (float)y)*20.f);
			int blockType = 1 + rand() % 8;
			for (int ground = 0; ground < glevel; ground++) {
				map.push_back(blockType); //map[mp + ground] = 1 + rand() % 14;
				if ((rand() % 8) == 0) blockType = 1 + rand() % 8;
			}
			for (int air = glevel; air < d; air++)
				map.push_back(0);  //map[mp + air] = 0;
				
		}
	}
	Pi3Cmesh mesh;
	createMapMeshChunk(map, w, h, d, mesh, 1, 1, w - 2, h - 2);
	return mesh;
}

void Blocks::createMapMeshChunk(std::vector<uint8_t> &map, uint32_t mapWidth, uint32_t mapHeight, uint32_t mapDepth, Pi3Cmesh &mesh, int32_t x, int32_t y, int32_t w, int32_t h)
{
	//std::vector<float> &verts = mesh.verts;
	//uint32_t stride = mesh.stride;
	//uint32_t v = 0;  //vertex pointer;
	uint32_t pitch = mapWidth * mapDepth;
	int halfDepth = mapDepth / 2;
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
					int ht = p - ptr;
					if (map[p - mapDepth] != 0) {
						addQuadLeftRight(mesh, xx, ht - halfDepth, yy, map[p - mapDepth], 1, 0);
					}
					if (map[p + mapDepth] != 0) {
						//addQuadLeftRight(mesh, xx, ht, yy, texPackUV[map[p + mapDepth]][2], 1);
					}
					if (map[p - pitch] != 0) {
						addQuadFrontBack(mesh, xx, ht - halfDepth, yy, map[p - pitch], 3, 0);
					}
					if (map[p + pitch] != 0) {
						//addQuadFrontBack(mesh, xx, ht, yy, texPackUV[map[p + pitch]][4], 1);
					}
				}

				//create ground plane ...
				int ht = p - ptr;
				addQuadTopBottom(mesh, xx, ht - halfDepth, yy, map[p],0, 0);
				int mpv = (mesh.verts.size() / mesh.stride);
				mesh.lineIndexes.push_back(mpv - 1);
				mesh.lineIndexes.push_back(mpv - 2);
				//mesh.lineIndexes.push_back(mpv - 3);
				//mesh.lineIndexes.push_back(mpv - 4);
				//mesh.lineIndexes.push_back(mpv - 5);
				//mesh.lineIndexes.push_back(mpv - 6);

				//skip non-air blocks (but create 'air' faces adjacent to solid blocks)
				while (p > ptr && map[p] != 0) {
					int ht = p - ptr;
					if (map[p - mapDepth] == 0) {
						addQuadLeftRight(mesh, xx, ht - halfDepth, yy, map[p],2, 1);
					}
					if (map[p + mapDepth] == 0) {
						//addQuadLeftRight(mesh, xx, ht, yy, texPackUV[map[p]][1], 0);
					}
					if (map[p - pitch] == 0) {
						addQuadFrontBack(mesh, xx, ht - halfDepth, yy, map[p],4, 1);
					}
					if (map[p + pitch] == 0) {
						//addQuadFrontBack(mesh, xx, ht, yy, texPackUV[map[p]][3], 0);
					}
					p--;
				}

				//create bottom plane (underside)
				if (p > ptr) addQuadTopBottom(mesh, xx, p - ptr + 1 - halfDepth, yy, map[p], 5, 1);
			}
		}
	}
}
