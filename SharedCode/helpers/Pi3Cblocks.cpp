#include "Pi3Cblocks.h"

void Blocks::createTexPackUV()
{
	static float ts = 1.f / 16.f;
	std::vector<vec2f> tx;
	tx.resize(6);

	//Calculate UV texture offsets ...
	//must be called before creating maps
	for (int i = 0; i < 256; i++) {
		tx[0] = vec2f((float)typToTex[i].topx, (float)typToTex[i].topy) * ts;
		tx[1] = vec2f((float)typToTex[i].sidesx, (float)typToTex[i].sidesy) * ts;
		tx[2] = vec2f((float)typToTex[i].sidesx, (float)typToTex[i].sidesy) * ts;
		tx[3] = vec2f((float)typToTex[i].frontx, (float)typToTex[i].fronty) * ts;
		tx[4] = vec2f((float)typToTex[i].backx, (float)typToTex[i].backy) * ts;
		tx[5] = vec2f((float)typToTex[i].topx, (float)typToTex[i].topy) * ts;
		texPackUV.push_back(tx);
	}
}

Blocks::Blocks(int MapSize, int ChunkWidth, int ChunkDepth, int ChunkHeight) 
{
	createTexPackUV();

	noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
	//noise.SetRotationType3D(FastNoiseLite::RotationType3D_ImproveXZPlanes);
	noise.SetFractalType(FastNoiseLite::FractalType_Ridged);

	mapSize = MapSize;
	chunkWidth = ChunkWidth;
	chunkDepth = ChunkDepth;
	chunkHeight = ChunkHeight;

	chunkSize = chunkWidth * chunkDepth * chunkHeight;		//16x16x256
	chunkMapSize = MapSize * MapSize * chunkSize;			//Total chunk map size
	chunks.resize(chunkMapSize);

	//Chunk offsets
	chunkSlice = chunkWidth * chunkHeight;					//slice across the X axis of all the chunks in the map
	chunkPitch = chunkWidth * MapSize;						//So as to navigate forward / back a row
	chunkPitchHeight = chunkPitch * chunkHeight;			//total chunk map width

	chunkLeft = -chunkHeight;
	chunkRight = chunkHeight;
	chunkBack = -chunkPitchHeight;
	chunkFront = chunkPitchHeight;

	chunkLeftBack = chunkLeft + chunkBack;
	chunkRightBack = chunkRight + chunkBack;
	chunkLeftFront = chunkLeft + chunkFront;
	chunkRightFront = chunkRight + chunkFront;
}

void Blocks::createMapChunk(int chunkX, int chunkZ)
{
	uint32_t chunkPtr = calcChunkPtr(chunkX, chunkZ); //Ptr into the main visible chunk blocks
	int cx = chunkX * chunkWidth;
	int cz = chunkZ * chunkDepth;
	int halfDepth = chunkHeight / 2;

	for (int z = 0; z < chunkDepth; z++) {
		for (int x = 0; x < chunkWidth; x++) {

			int mp = chunkPtr + x * chunkHeight + z * chunkPitchHeight;
			//int p = map.size();
			//map.resize(p + chunkHeight);

			int glevel = halfDepth + (int)(noise.GetNoise((float)(cx + x), (float)(cz + z)) * 20.f);

			//int glevel = halfDepth + (int)(noise.GetNoise((float)x, (float)y, (float)z) * 10.f);
			for (int air = chunkHeight - 1; air > glevel; air--)
				chunks[mp + air] = blockType::Air;  //map[mp + air] = 0;

			//int blockType = 1 + rand() % 8;
			chunks[mp + glevel] = blockType::Grass;

			int caveLevel = (glevel - 5 > 0) ? glevel - 5 : 0;
			for (int ground = glevel - 1; ground >= caveLevel; ground--) {
				chunks[mp + ground] = blockType::Dirt; //map[mp + ground] = 1 + rand() % 14;

				//if ((rand() % 8) == 0) blockType = 1 + rand() % 8;
			}
			chunks[mp + caveLevel] = blockType::Granite;

			glevel = (halfDepth - 6 + (int)(-noise.GetNoise((float)(cx + x + 150), (float)(cz + z - 30)) * 15.f));

			for (int ground = caveLevel - 1; ground >= glevel; ground--) {
				chunks[mp + ground] = blockType::Granite; //map[mp + ground] = 1 + rand() % 14;
			}

			int glevel2 = halfDepth - 7 + (int)(-noise.GetNoise((float)(cx + x + 21310), (float)(cz + z - 2430)) * 20.f);

			caveLevel = glevel;

			int stalagtites = (rand() % 15 == 0) ? (rand() % 10)+3 : 0;
			glevel -= (caveLevel-glevel2<stalagtites) ? caveLevel-glevel2 : stalagtites;

			for (int st = caveLevel; st >= glevel; st--)
				chunks[mp + st] = blockType::Basalt;


			if (glevel2 < glevel) {

				for (int air = glevel - 1; air > glevel2; air--)
					chunks[mp + air] = blockType::Air;  //map[mp + air] = 0
			}
			else {
				glevel2 = glevel;
			}
			//chunks[mp + glevel] = blockType::Granite;

			for (int ground = glevel2 - 1; ground >= 0; ground--) {
				chunks[mp + ground] = blockType::Granite; //map[mp + ground] = 1 + rand() % 14;
			}

			//if (caveLevel > 1) {
			//	for (int cavel = caveLevel - 1; cavel >= 0; cavel--) {
			//		float noise3D = noise.GetNoise((float)(cx + x), (float)cavel, (float)(cz + z));
			//		if (noise3D > 0) chunks[mp + cavel] = blockType::Granite;
			//	}
			//}
		}
	}
}

void Blocks::insertBlock(uint8_t val, uint32_t mp, int x, int y, int z)
{
	if (x < 0 || z < 0 || x >= chunkWidth || z >= chunkDepth) return;
	uint32_t ptr = mp + x * chunkHeight + z * chunkSlice + y;
	chunks[ptr] = val;
}

void Blocks::createTrees(int chunkX, int chunkZ)
{
	uint32_t chunkPtr = calcChunkPtr(chunkX, chunkZ);

	for (int z = 0; z < chunkDepth; z++) {
		for (int x = 0; x < chunkWidth; x++) {
			int mp = chunkPtr + x * chunkHeight + z * chunkSlice;
			srand(mp);
			if (rand() % 50 == 0)
				addTree(chunkPtr, x, z, 5 + rand() % 15);
		}
	}
}

void Blocks::createMeshChunk(Pi3Cmesh& mesh, int chunkX, int chunkZ)
{

	uint32_t chunkPtr = calcChunkPtr(chunkX, chunkZ);
	//std::vector<float> &verts = mesh.verts;
	//uint32_t stride = mesh.stride;
	//uint32_t v = 0;  //vertex pointer;
	int cx = chunkX * chunkWidth;
	int cz = chunkZ * chunkDepth;
	int halfDepth = chunkHeight / 2;
	//float ts = 1.f / 16.f;  //texture map step (assuming 16 images wide on texture pack)

	//vec3f norm_up(0, 1.f, 0);
	for (int32_t zz = 0; zz < chunkDepth; zz++) {
		for (int32_t xx = 0; xx < chunkWidth; xx++) {

			uint32_t ptr = chunkPtr + (xx + zz * chunkPitch) * chunkHeight;
			uint32_t p = ptr + chunkHeight - 1;
			int light = 255; //start with max light

			//Create all ground surfaces ...
			while (p > ptr) {

				//skip air blocks (but create faces adjacent to air blocks)
				while (p > ptr && chunks[p] == blockType::Air) {
					int ht = p - ptr;
					if (chunks[p + chunkLeft] != blockType::Air) {
						addQuadLeft(mesh, cx + xx, ht - halfDepth, cz + zz, chunks[p + chunkLeft], 1, light);
					}
					//if (xx == chunkWidth-1 && chunks[p + chunkHeight] != blockType::Air) {
					//	addQuadLeftRight(mesh, cx + xx, ht, cz + zz, chunks[p + chunkHeight], 2, 1, light);
					//}
					if (chunks[p + chunkBack] != blockType::Air) {
						addQuadFrontBack(mesh, cx + xx, ht - halfDepth, cz + zz, chunks[p + chunkBack], 3, 0, light);
					}
					//if (zz == chunkDepth-1 && chunks[p + chunkFront] != blockType::Air) {
					//	addQuadFrontBack(mesh, cx + xx, ht, cz + zz, chunks[p + chunkFront], 4, 1, light);
					//}
					if (light < 255) light++;
					p--;
				}

				//work out shadow edges for ground plain ...
				int ht = p - ptr;
				int shadow = 0;
				if (chunks[p + 1 + chunkLeftBack]) shadow = shadow | 1;
				if (chunks[p + 1 + chunkRightBack]) shadow = shadow | 2;
				if (chunks[p + 1 + chunkRightFront]) shadow = shadow | 4;
				if (chunks[p + 1 + chunkLeftFront]) shadow = shadow | 8;
				if (chunks[p + 1 + chunkBack]) shadow = shadow | 3;
				if (chunks[p + 1 + chunkFront]) shadow = shadow | 12;
				if (chunks[p + 1 + chunkLeft]) shadow = shadow | 9;
				if (chunks[p + 1 + chunkRight]) shadow = shadow | 6;

				//create ground plane ...
				addQuadTopBottom(mesh, cx + xx, ht - halfDepth, cz + zz, chunks[p], 0, 0, shadow);

				int mpv = (mesh.verts.size() / mesh.stride); //TODO:... adding line verts wont work until they're loaded into vert buffers (this must be fixed!)
				mesh.lineIndexes.push_back(mpv - 1);
				mesh.lineIndexes.push_back(mpv - 2);
				//mesh.lineIndexes.push_back(mpv - 3);
				//mesh.lineIndexes.push_back(mpv - 4);
				//mesh.lineIndexes.push_back(mpv - 5);
				//mesh.lineIndexes.push_back(mpv - 6);

				//skip non-air blocks (but create 'air' faces adjacent to solid blocks)
				while (p > ptr && chunks[p] != blockType::Air) {
					int ht = p - ptr;
					if (chunks[p + chunkLeft] == blockType::Air) {
						addQuadLeftRight(mesh, cx + xx, ht - halfDepth, cz + zz, chunks[p], 2, 1, light);
					}
					//if (chunks[p + chunkHeight] == blockType::Air) {
					//	//addQuadLeftRight(mesh, cx + xx, ht, cz + zz, texPackUV[map[p]][1], 0);
					//}
					if (chunks[p + chunkBack] == blockType::Air) {
						addQuadFrontBack(mesh, cx + xx, ht - halfDepth, cz + zz, chunks[p], 4, 1, light);
					}
					//if (chunks[p + chunkPitchHeight] == blockType::Air) {
					//	//addQuadFrontBack(mesh, cx + xx, ht, cz + zz, texPackUV[map[p]][3], 0);
					//}
					p--;
					if (light < 255) light++;
				}

				//create bottom plane (underside)
				if (p > ptr) {
					addQuadTopBottom(mesh, cx + xx, (p - ptr) - halfDepth, cz + zz, chunks[p + 1], 0, 1, light);
					light = 64;
				}
			}
		}
	}
}




uint32_t shadow(uint32_t col, int light)
{
	float shadow = (float)light / 255.f;
	return (uint32_t)((float)(col & 255) * shadow) | ((uint32_t)((float)((col >> 8) & 255) * shadow) << 8) | ((uint32_t)((float)((col >> 16) & 255) * shadow) << 16) | 0xff000000;
}

void Blocks::addQuadTopBottom(Pi3Cmesh &mesh, int x, int h, int z, uint8_t mapVal, uint8_t faceVal, int tb, int light)
{
	vec3f norm(0, -(1.f - (float)((1 - tb) * 2)), 0);
	float hh = (float)(h);
	vec2f tex = texPackUV[mapVal][faceVal];
	float ftb = (float)tb;
	//float shadow = (float)light / 255.f;
	//uint32_t col = shadow(typToTex[mapVal].col, light);
	//col = (uint32_t)((float)(col & 255) * shadow) | ((uint32_t)((float)((col >> 8) & 255) * shadow) << 8) | ((uint32_t)((float)((col >> 16) & 255) * shadow) << 16) | 0xff000000;

	uint32_t col = typToTex[mapVal].col;
	uint32_t topLeftLight = (light & 1) ? shadow(col, 128) : col;
	uint32_t topRightLight = (light & 2) ? shadow(col, 128) : col;
	uint32_t botRightLight = (light & 4) ? shadow(col, 128) : col;
	uint32_t botLeftLight = (light & 8) ? shadow(col, 128) : col;

	mesh.addPackedVert(vec3f((float)(x + 1), hh, (float)z), norm, tex + vec2f(textureDiv, 0), topRightLight);
	mesh.addPackedVert(vec3f((float)x, hh, (float)(z + tb)), norm, tex + vec2f(0, textureDiv * ftb), (tb==0) ? topLeftLight : botLeftLight);
	mesh.addPackedVert(vec3f((float)x, hh, (float)(z + (1 - tb))), norm, tex + vec2f(0, textureDiv * (1.f - ftb)), (tb == 0) ? botLeftLight : topLeftLight);

	mesh.addPackedVert(vec3f((float)(x + 1), hh, (float)z), norm, tex + vec2f(textureDiv, 0), topRightLight);
	mesh.addPackedVert(vec3f((float)(x + tb), hh, (float)(z + 1)), norm, tex + vec2f(textureDiv * ftb, textureDiv), (tb == 0) ? botLeftLight : botRightLight);
	mesh.addPackedVert(vec3f((float)(x + (1 - tb)), hh, (float)(z + 1)), norm, tex + vec2f(textureDiv * (1.f - ftb), textureDiv), (tb == 0) ? botRightLight : botLeftLight);
}

void Blocks::addQuadLeft(Pi3Cmesh& mesh, int x, int y, int z, uint8_t mapVal, uint8_t faceVal, int light)
{
	static vec3f norm(-1.f, 0, 0);
	float xx = (float)x, yy = (float)y, zz = (float)z;

	vec2f texUV = texPackUV[mapVal][faceVal];
	uint32_t col = typToTex[mapVal].colSides;

	mesh.addPackedVert(vec3f(xx, yy, zz), norm, texUV + vec2f(textureDiv, 0), col);
	mesh.addPackedVert(vec3f(xx, yy, zz + 1), norm, texUV, col);
	mesh.addPackedVert(vec3f(xx, yy - 1, zz + 1), norm, texUV + vec2f(0, textureDiv), col);

	mesh.addPackedVert(vec3f(xx, yy, zz), norm, texUV + vec2f(textureDiv, 0), col);
	mesh.addPackedVert(vec3f(xx, yy - 1, zz + 1), norm, texUV + vec2f(0, textureDiv), col);
	mesh.addPackedVert(vec3f(xx, yy - 1, zz), norm, texUV + vec2f(textureDiv, textureDiv), col);
}


void Blocks::addQuadLeftRight(Pi3Cmesh &mesh, int x, int y, int z, uint8_t mapVal, uint8_t faceVal, int leftRight, int light)
{
	float xx = (float)x, yy = (float)y, zz = (float)z, lr = (float)leftRight, ilr = 1-lr;
	static vec3f norm((-1.f - (1 - lr) * 2), 0, 0);

	vec2f tex = texPackUV[mapVal][faceVal];
	uint32_t col = typToTex[mapVal].colSides;

	mesh.addPackedVert(vec3f(xx, yy, zz + lr), norm, tex + vec2f(textureDiv, 0), col);
	mesh.addPackedVert(vec3f(xx, yy, zz + ilr), norm, tex, col);
	mesh.addPackedVert(vec3f(xx, yy - 1, zz + ilr), norm, tex + vec2f(0, textureDiv), col);

	mesh.addPackedVert(vec3f(xx, yy, zz + lr), norm, tex + vec2f(textureDiv, 0), col);
	mesh.addPackedVert(vec3f(xx, yy - 1, zz + ilr), norm, tex + vec2f(0, textureDiv), col);
	mesh.addPackedVert(vec3f(xx, yy - 1, zz + lr), norm, tex + vec2f(textureDiv, textureDiv), col);
}

void Blocks::addQuadFrontBack(Pi3Cmesh &mesh, int x, int h, int z, uint8_t mapVal, uint8_t faceVal, int fb, int light)
{
	static vec3f norm(0, 0, -(1.f - (float)((1 - fb) * 2))); // (1.f - (float)((1 - fb) * 2)));
	float zz = (float)(z);
	vec2f tex = texPackUV[mapVal][faceVal];
	uint32_t col = typToTex[mapVal].colSides;
	mesh.addPackedVert(vec3f((float)(x +(1-fb)), (float)h, zz), norm, tex + vec2f(textureDiv, 0), col);
	mesh.addPackedVert(vec3f((float)(x + fb), (float)h, zz), norm, tex, col);
	mesh.addPackedVert(vec3f((float)(x + fb), (float)(h - 1), zz), norm, tex + vec2f(0, textureDiv), col);

	mesh.addPackedVert(vec3f((float)(x + (1 - fb)), (float)h, zz), norm, tex + vec2f(textureDiv, 0), col);
	mesh.addPackedVert(vec3f((float)(x + fb), (float)(h - 1), zz), norm, tex + vec2f(0, textureDiv), col);
	mesh.addPackedVert(vec3f((float)(x + (1 - fb)), (float)(h - 1), zz), norm, tex + vec2f(textureDiv, textureDiv), col);
}



void Blocks::addTree(uint32_t chunkPtr, int x, int z, int size)
{
	uint32_t ptr = chunkPtr + x * chunkHeight + z * chunkSlice + chunkHeight - 1;
	uint32_t endPtr = ptr - chunkHeight;

	if (ptr<0 || ptr>=chunkMapSize || chunks[ptr] != blockType::Air) 
		return; //exit if coords outside or no air found

	if (size > (ptr - endPtr)) size = ptr - endPtr; //tree can't go above sky limit

	while (ptr > endPtr && (chunks[ptr] == blockType::Air || chunks[ptr] == blockType::OakLeaves || chunks[ptr] == blockType::OakWood)) ptr--; //find ground

	int xtr = x;
	int ytr = ptr - endPtr;
	int ztr = z;
	int branches = 1;
	int leaves = 30;
	float bh = size * 0.7;
	float br = 1 + size * 0.05;
	float jh = (size * 0.7) / branches;

	if (bh < 3) return;

	for (int t = ptr; t < (ptr + (int)bh); t++) 
		chunks[t] = blockType::OakWood;

	return;


	for (int yl = 2; yl < (int)bh; yl++) {
		for (int zl = 0; zl < 5; zl++) {
			for (int xl = 0; xl < 5; xl++) {
				if (!(zl == 2 && xl == 2) && rand() % 3 == 0) {
					insertBlock(blockType::OakLeaves, chunkPtr, x + xl - 2, ytr + yl, z + zl - 2);
				}
			}
		}
	}

	//for (int j = 0; j < leaves; j++) {
	//	int xx = x + rand() % 5 - 2;
	//	int yy = ytr + 2 + (rand() % ((int)bh - 2));
	//	int zz = z + rand() % 5 - 2;
	//	insertBlock(blockType::OakLeaves, map, w, h, d, xx, yy, zz);
	//}

	//for (int j = 0; j < branches; j++) {
	//	float yr = (size * 0.3) + jh * j;
	//	float hr = 2.0f + (size - yr) * 0.5f;
	//	float r = (rand() % 628) / 100.0f;
	//	float xr = cos(r) * hr * 0.7; // rand() % (int)hr) - hr / 2;
	//	float zr = sin(r) * hr * 0.7; //(rand() % (int)hr) - hr / 2;

	//	for (int k = 0; k < leaves; k++) {
	//		float r = 3 + (rand() % (int)size / 2);
	//		float a = (rand() % 628) / 100;
	//		float b = (rand() % 628) / 100;
	//		float x = cos(a) * r;
	//		float y = sin(a) * r;

	//		insertBlock(blockType::OakLeaves, map, w, h, d, xtr + xr * 0.9 + cos(b) * x, ytr + yr + y, ztr + zr + sin(b) * x);
	//	}
	//}

	for (int t = ptr; t < (ptr + (int)bh); t++) chunks[t] = blockType::OakWood;

}
