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
	createBlocks(MapSize, ChunkWidth, ChunkDepth, ChunkHeight);
}

void Blocks::createBlocks(int MapSize, int ChunkWidth, int ChunkDepth, int ChunkHeight) 
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

			int mp = calcVoxelPtr(chunkPtr, x, z);

			int glevel = halfDepth + (int)(noise.GetNoise((float)(cx + x), (float)(cz + z)) * 20.f);
			//if (chunkX==1 && chunkZ==1) glevel = 250;

			for (int air = chunkHeight - 2; air > glevel; air--)
				chunks[mp + air] = blockType::Air;

			chunks[mp + chunkHeight - 1] = chunkHeight-glevel; //store ground level in top byte of chunk column

			if ((rand() % 5) == 0) {
				switch (rand() % 10) {
				case 5:
				case 6:
				case 9:
				case 0:chunks[mp + glevel + 1] = blockType::TallGrass; break;
				case 1:chunks[mp + glevel + 1] = blockType::RedFlower; break;
				case 2:chunks[mp + glevel + 1] = blockType::PurpleFlower; break;
				case 3:chunks[mp + glevel + 1] = blockType::WhiteFlower; break;
				case 7:
				case 8:
				case 4:chunks[mp + glevel + 1] = blockType::DryBush; break;
				}
			}
			chunks[mp + glevel] = blockType::Grass;

			int caveLevel = (glevel - 5 > 0) ? glevel - 5 : 0;
			for (int ground = glevel - 1; ground >= caveLevel; ground--) {
				chunks[mp + ground] = blockType::Dirt;
			}
			chunks[mp + caveLevel] = blockType::Granite;

			glevel = (halfDepth - 6 + (int)(-noise.GetNoise((float)(cx + x + 150), (float)(cz + z - 30)) * 15.f));

			for (int ground = caveLevel - 1; ground >= glevel; ground--) {
				chunks[mp + ground] = blockType::Granite;
			}

			int glevel2 = halfDepth - 7 + (int)(-noise.GetNoise((float)(cx + x + 21310), (float)(cz + z - 2430)) * 20.f);

			caveLevel = glevel;

			//int stalagtites = (rand() % 15 == 0) ? (rand() % 10)+3 : 0;
			//glevel -= (caveLevel-glevel2<stalagtites) ? caveLevel-glevel2 : stalagtites;

			//for (int st = caveLevel; st >= glevel; st--)
			//	chunks[mp + st] = blockType::Basalt;


			if (glevel2 < glevel) {
				for (int air = glevel - 1; air > glevel2; air--) chunks[mp + air] = blockType::Air;  //map[mp + air] = 0
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

void Blocks::insertBlock(uint8_t blockType, const vec3f& chunkCorner, const vec3f& position)
{
	uint32_t chunkXZ = calcRelativePositionPtr(chunkCorner, position);
	int ptr = chunkXZ + ((int)(position.y)+ chunkHeight / 2);
	if (ptr<0 || ptr>chunkMapSize) return;
	chunks[ptr] = blockType;
}

void Blocks::insertBlock(uint8_t blockType, uint32_t chunkPtr, int x, int y, int z)
{
	uint32_t ptr = calcVoxelPtr(chunkPtr, x, z) + y;
	if (ptr<0 || ptr>chunkMapSize) return;
	if (chunks[ptr]==blockType::Air) chunks[ptr] = blockType;
}

void lightRecovery(int& light, int lightRecover)
{
	light = (light + lightRecover < 255) ? light + lightRecover : 255;
}

void Blocks::updateMeshChunk(Pi3Cresource* resource, Pi3Cscene* scene, const vec3f& chunkCorner, const vec3f& position)
{
	vec3f offset = position - chunkCorner;
	int chunkOffsetX = (int)(offset.x / chunkWidth);
	int chunkOffsetZ = (int)(offset.z / chunkDepth);
	int blockId = chunkOffsetX + chunkOffsetZ * mapSize;
	Pi3Cmesh newmesh;
	createMeshChunk(newmesh, chunkOffsetX, chunkOffsetZ);
	scene->models[blockId].updateMesh(resource, newmesh);
}

void Blocks::createMeshChunk(Pi3Cmesh& mesh, int chunkX, int chunkZ)
{
	uint32_t chunkPtr = calcChunkPtr(chunkX, chunkZ);
	int cx = chunkX * chunkWidth;
	int cz = chunkZ * chunkDepth;
	int halfDepth = chunkHeight / 2;
	int lightRecover = 5;
	int lightMax = 255 - lightRecover;

	for (int32_t zz = 0; zz < chunkDepth; zz++) {
		for (int32_t xx = 0; xx < chunkWidth; xx++) {

			uint32_t ptr = calcVoxelPtr(chunkPtr, xx, zz);
			uint32_t p = ptr + chunkHeight - 2; //skip top byte height level
			int light = 255; //start with max light
			int geomtype;
			//Create all surfaces ...
			while (p > ptr) {

				//edges should read values on opposite sides (i.e. wrapping)
				bool onfarLeft = chunkX == 0 && xx == 0;
				bool onfarRight = chunkX == mapSize - 1 && xx == chunkWidth - 1;
				bool onfarBack = chunkZ == 0 && zz == 0;
				bool onfarFront = chunkZ == mapSize - 1 && zz == chunkDepth - 1;

				//skip air blocks (but create faces adjacent to air blocks)
				int shadow = 0;
				while (p > ptr && chunks[p] == blockType::Air) {
					int ht = p - ptr;

					if (!onfarLeft) {
						geomtype = typToTex[chunks[p + chunkLeft]].geomType;
						if (geomtype < blockTypes::xshape && geomtype > blockTypes::air) {
							addQuadLeftRight(mesh, cx + xx, ht - halfDepth, cz + zz, chunks[p + chunkLeft], 1, 0, light, shadow);
						}
						else {
							if (light < 255) lightRecovery(light, lightRecover);
						}
					}

					if (!onfarBack) {
						geomtype = typToTex[chunks[p + chunkBack]].geomType;
						if (geomtype < blockTypes::xshape && geomtype > blockTypes::air) {
							addQuadFrontBack(mesh, cx + xx, ht - halfDepth, cz + zz, chunks[p + chunkBack], 3, 0, light, shadow);
						}
						else {
							if (light < 255) lightRecovery(light, lightRecover);
						}
					}

					if (light < 255) {
						if (!onfarRight && chunks[p + chunkRight] == blockType::Air) lightRecovery(light, lightRecover);
						if (!onfarFront && chunks[p + chunkFront] == blockType::Air) lightRecovery(light, lightRecover);
					}
					p--;
				}

				//work out shadow edges for ground quad ...
				shadow = 0;
				int blockAbove = p + 1;

					if (!(onfarLeft || onfarBack) && chunks[blockAbove + chunkLeftBack]) shadow = shadow | 1;
					if (!(onfarRight || onfarBack) && chunks[blockAbove + chunkRightBack]) shadow = shadow | 2;
					if (!(onfarRight || onfarFront) && chunks[blockAbove + chunkRightFront]) shadow = shadow | 4;
					if (!(onfarLeft || onfarFront) && chunks[blockAbove + chunkLeftFront]) shadow = shadow | 8;
					if (!onfarBack && chunks[blockAbove + chunkBack]) shadow = shadow | 3;
					if (!onfarFront && chunks[blockAbove + chunkFront]) shadow = shadow | 12;
					if (!onfarLeft && chunks[blockAbove + chunkLeft]) shadow = shadow | 9;
					if (!onfarRight && chunks[blockAbove + chunkRight]) shadow = shadow | 6;


				//create ground plane ...
				if (typToTex[chunks[p]].geomType < blockTypes::xshape) {
					int ht = p - ptr;
					addQuadTopBottom(mesh, cx + xx, ht - halfDepth, cz + zz, chunks[p], 0, 0, light, shadow);
				}

				//TODO:... adding line verts wont work until they're loaded into vert buffers (this must be fixed!)
				int mpv = (mesh.verts.size() / mesh.stride); 
				mesh.lineIndexes.push_back(mpv - 1);
				mesh.lineIndexes.push_back(mpv - 2);
				//mesh.lineIndexes.push_back(mpv - 3);
				//mesh.lineIndexes.push_back(mpv - 4);
				//mesh.lineIndexes.push_back(mpv - 5);
				//mesh.lineIndexes.push_back(mpv - 6);

				shadow = 0;
				//skip non-air blocks (but create 'air' faces adjacent to solid blocks)
				while (p > ptr && chunks[p] != blockType::Air) {
					int ht = p - ptr - halfDepth;
					int geomtype = typToTex[chunks[p]].geomType;
					if (geomtype < blockTypes::xshape) {
						if (!onfarLeft && chunks[p + chunkLeft] == blockType::Air) {
							if (light < 255) lightRecovery(light, lightRecover);
							addQuadLeftRight(mesh, cx + xx, ht, cz + zz, chunks[p], 2, 1, light, shadow);
						}
						if (!onfarBack && chunks[p + chunkBack] == blockType::Air) {
							if (light < 255) lightRecovery(light, lightRecover);
							addQuadFrontBack(mesh, cx + xx, ht, cz + zz, chunks[p], 4, 1, light, shadow);
						}
					}
					else {
						switch (geomtype) {
						case blockTypes::xshape: 
							addXshape(mesh, cx + xx, ht, cz + zz, chunks[p], light, 0);
							break;
						case blockTypes::stick: break;
						case blockTypes::table: break;
						}
						shadow = 15;
					}
					//If this is a 'see-through' block, then create edges of blocks adjacent to it ...
					if (geomtype > 1) {
						if (geomtype == 2) { shadow = 15; }
						if (!onfarLeft && typToTex[chunks[p + chunkLeft]].geomType == 1) {
							addQuadLeftRight(mesh, cx + xx, ht, cz + zz, chunks[p + chunkLeft], 1, 0, light, shadow);
						}
						if (!onfarRight && typToTex[chunks[p + chunkRight]].geomType == 1) {
							addQuadLeftRight(mesh, cx + xx + 1, ht, cz + zz, chunks[p + chunkRight], 2, 1, light, shadow);
						}
						if (!onfarBack && typToTex[chunks[p + chunkBack]].geomType == 1) {
							addQuadFrontBack(mesh, cx + xx, ht, cz + zz, chunks[p + chunkBack], 3, 0, light, shadow);
						}
						if (!onfarFront && typToTex[chunks[p + chunkFront]].geomType == 1) {
							addQuadFrontBack(mesh, cx + xx, ht, cz + zz + 1, chunks[p + chunkFront], 4, 1, light, shadow);
						}
						if ((p - 1) > ptr && typToTex[chunks[p - 1]].geomType == 1) {
							addQuadTopBottom(mesh, cx + xx, ht-1, cz + zz, chunks[p - 1], 0, 0, light, shadow);
						}
					}

					if (light < 255) {
						if (!onfarRight && chunks[p + chunkRight] == blockType::Air) lightRecovery(light, lightRecover);
						if (!onfarFront && chunks[p + chunkFront] == blockType::Air) lightRecovery(light, lightRecover);
					}
					p--;
				}

				//create bottom plane (underside)
				if (p > ptr) {
					shadow = 0;
					light = 128;
					//if (light < 80) light = 80;
					addQuadTopBottom(mesh, cx + xx, (p - ptr) - halfDepth, cz + zz, chunks[p + 1], 0, 1, light, shadow);
				}
			}
		}
	}
	mesh.updateBounds();
}




uint32_t shadow(uint32_t col, float shadow)
{
	//float shadow = (float)light / 255.f;
	return (uint32_t)((float)(col & 255) * shadow) | ((uint32_t)((float)((col >> 8) & 255) * shadow) << 8) | ((uint32_t)((float)((col >> 16) & 255) * shadow) << 16) | 0xff000000;
}

void Blocks::addQuadTopBottom(Pi3Cmesh &mesh, int x, int h, int z, uint8_t mapVal, uint8_t faceVal, int tb, int light, int shadowEdge)
{
	vec3f norm(0, -(1.f - (float)((1 - tb) * 2)), 0);
	float hh = (float)(h);
	vec2f tex = texPackUV[mapVal][faceVal];
	float ftb = (float)tb;
	//float shadow = (float)light / 255.f;
	//uint32_t col = shadow(typToTex[mapVal].col, light);
	//col = (uint32_t)((float)(col & 255) * shadow) | ((uint32_t)((float)((col >> 8) & 255) * shadow) << 8) | ((uint32_t)((float)((col >> 16) & 255) * shadow) << 16) | 0xff000000;

	uint32_t col = typToTex[mapVal].col;
	float shadowLight = (float)light / 400.f;
	uint32_t shadowCol = shadow(col, (float)light / 255.f);
	uint32_t topLeftLight = (shadowEdge & 1) ? shadow(col, shadowLight) : shadowCol;
	uint32_t topRightLight = (shadowEdge & 2) ? shadow(col, shadowLight) : shadowCol;
	uint32_t botRightLight = (shadowEdge & 4) ? shadow(col, shadowLight) : shadowCol;
	uint32_t botLeftLight = (shadowEdge & 8) ? shadow(col, shadowLight) : shadowCol;

	mesh.addPackedVert(vec3f((float)(x + 1), hh, (float)z), norm, tex + vec2f(textureDiv, 0), topRightLight);
	mesh.addPackedVert(vec3f((float)x, hh, (float)(z + tb)), norm, tex + vec2f(0, textureDiv * ftb), (tb==0) ? topLeftLight : botLeftLight);
	mesh.addPackedVert(vec3f((float)x, hh, (float)(z + (1 - tb))), norm, tex + vec2f(0, textureDiv * (1.f - ftb)), (tb == 0) ? botLeftLight : topLeftLight);

	mesh.addPackedVert(vec3f((float)(x + 1), hh, (float)z), norm, tex + vec2f(textureDiv, 0), topRightLight);
	mesh.addPackedVert(vec3f((float)(x + tb), hh, (float)(z + 1)), norm, tex + vec2f(textureDiv * ftb, textureDiv), (tb == 0) ? botLeftLight : botRightLight);
	mesh.addPackedVert(vec3f((float)(x + (1 - tb)), hh, (float)(z + 1)), norm, tex + vec2f(textureDiv * (1.f - ftb), textureDiv), (tb == 0) ? botRightLight : botLeftLight);
}

void Blocks::addQuadLeftRight(Pi3Cmesh &mesh, int x, int y, int z, uint8_t mapVal, uint8_t faceVal, int leftRight, int light, int shadowEdge)
{
	float xx = (float)x, yy = (float)y, zz = (float)z, lr = (float)leftRight, ilr = 1-lr;
	static vec3f norm(1.f - ilr * 2, 0, 0);

	vec2f tex = texPackUV[mapVal][faceVal];
	uint32_t col = typToTex[mapVal].colSides;
	uint32_t shadowCol = shadow(col, (float)light / 255.f);

	mesh.addPackedVert(vec3f(xx, yy, zz + lr), norm, tex + vec2f(textureDiv, 0), shadowCol);
	mesh.addPackedVert(vec3f(xx, yy, zz + ilr), norm, tex, shadowCol);
	mesh.addPackedVert(vec3f(xx, yy - 1, zz + ilr), norm, tex + vec2f(0, textureDiv), shadowCol);

	mesh.addPackedVert(vec3f(xx, yy, zz + lr), norm, tex + vec2f(textureDiv, 0), shadowCol);
	mesh.addPackedVert(vec3f(xx, yy - 1, zz + ilr), norm, tex + vec2f(0, textureDiv), shadowCol);
	mesh.addPackedVert(vec3f(xx, yy - 1, zz + lr), norm, tex + vec2f(textureDiv, textureDiv), shadowCol);
}

void Blocks::addQuadFrontBack(Pi3Cmesh &mesh, int x, int y, int z, uint8_t mapVal, uint8_t faceVal, int frontBack, int light, int shadowEdge)
{
	float xx = (float)x, yy = (float)y, zz = (float)z, fb = (float)frontBack, ifb = 1 - fb;
	static vec3f norm(0, 0, 1.f - ifb * 2); // (1.f - (float)((1 - fb) * 2)));

	vec2f tex = texPackUV[mapVal][faceVal];
	uint32_t col = typToTex[mapVal].colSides;
	uint32_t shadowCol = shadow(col, (float)light / 255.f);

	mesh.addPackedVert(vec3f(xx + ifb, yy, zz), norm, tex + vec2f(textureDiv, 0), shadowCol);
	mesh.addPackedVert(vec3f(xx + fb, yy, zz), norm, tex, shadowCol);
	mesh.addPackedVert(vec3f(xx + fb, yy - 1, zz), norm, tex + vec2f(0, textureDiv), shadowCol);

	mesh.addPackedVert(vec3f(xx + ifb, yy, zz), norm, tex + vec2f(textureDiv, 0), shadowCol);
	mesh.addPackedVert(vec3f(xx + fb, yy - 1, zz), norm, tex + vec2f(0, textureDiv), shadowCol);
	mesh.addPackedVert(vec3f(xx + ifb, yy - 1, zz), norm, tex + vec2f(textureDiv, textureDiv), shadowCol);
}

void Blocks::addXshape(Pi3Cmesh& mesh, int x, int y, int z, uint8_t mapVal, int light, int shadowEdge)
{
	float xx = (float)x, yy = (float)y, zz = (float)z;

	vec2f tex = texPackUV[mapVal][0];
	uint32_t col = typToTex[mapVal].colSides;
	uint32_t shadowCol = shadow(col, (float)light / 255.f);

	vec3f v1(xx + 1, yy, zz + 1), v2(xx, yy, zz),  v3(xx, yy - 1, zz), v4(xx + 1, yy - 1, zz + 1);
	vec3f norm = v1.trinormal(v2, v3);
	mesh.addPackedVert(v1, norm, tex + vec2f(textureDiv, 0), shadowCol);
	mesh.addPackedVert(v2, norm, tex, shadowCol);
	mesh.addPackedVert(v3, norm, tex + vec2f(0, textureDiv), shadowCol);

	mesh.addPackedVert(v1, norm, tex + vec2f(textureDiv, 0), shadowCol);
	mesh.addPackedVert(v3, norm, tex + vec2f(0, textureDiv), shadowCol);
	mesh.addPackedVert(v4, norm, tex + vec2f(textureDiv, textureDiv), shadowCol);

	mesh.addPackedVert(v2, -norm, tex, shadowCol);
	mesh.addPackedVert(v1, -norm, tex + vec2f(textureDiv, 0), shadowCol);
	mesh.addPackedVert(v4, -norm, tex + vec2f(textureDiv, textureDiv), shadowCol);

	mesh.addPackedVert(v2, -norm, tex, shadowCol);
	mesh.addPackedVert(v4, -norm, tex + vec2f(textureDiv, textureDiv), shadowCol);
	mesh.addPackedVert(v3, -norm, tex + vec2f(0, textureDiv), shadowCol);

	v1 = vec3f(xx + 1, yy, zz);
	v2 = vec3f(xx, yy, zz + 1);
	v3 = vec3f(xx, yy - 1, zz + 1);
	v4 = vec3f(xx + 1, yy - 1, zz);
	norm = v1.trinormal(v2, v3);

	mesh.addPackedVert(v1, norm, tex + vec2f(textureDiv, 0), shadowCol);
	mesh.addPackedVert(v2, norm, tex, shadowCol);
	mesh.addPackedVert(v3, norm, tex + vec2f(0, textureDiv), shadowCol);

	mesh.addPackedVert(v1, norm, tex + vec2f(textureDiv, 0), shadowCol);
	mesh.addPackedVert(v3, norm, tex + vec2f(0, textureDiv), shadowCol);
	mesh.addPackedVert(v4, norm, tex + vec2f(textureDiv, textureDiv), shadowCol);

	mesh.addPackedVert(v2, -norm, tex, shadowCol);
	mesh.addPackedVert(v1, -norm, tex + vec2f(textureDiv, 0), shadowCol);
	mesh.addPackedVert(v4, -norm, tex + vec2f(textureDiv, textureDiv), shadowCol);

	mesh.addPackedVert(v2, -norm, tex, shadowCol);
	mesh.addPackedVert(v4, -norm, tex + vec2f(textureDiv, textureDiv), shadowCol);
	mesh.addPackedVert(v3, -norm, tex + vec2f(0, textureDiv), shadowCol);

}

uint32_t Blocks::calcRelativePositionPtr(const vec3f& chunkCorner, const vec3f& position)
{
	vec3f offset = position - chunkCorner;
	//offset.z = (float)(chunkDepth * mapSize) - offset.z;
	int chunkOffsetX = (int)(offset.x / chunkWidth);
	int chunkOffsetZ = (int)(offset.z / chunkDepth);
	int x = (int)offset.x % chunkWidth;
	int z = (int)offset.z % chunkDepth;
	uint32_t chunkPtr = calcChunkPtr(chunkOffsetX, chunkOffsetZ);
	//SDL_Log("Offsets: chunkX:%d, chunkZ:%d, x:%d, z:%d", chunkOffsetX, chunkOffsetZ, x, z);
	return calcVoxelPtr(chunkPtr, x, z);
}

int Blocks::getGroundHeight(const vec3f& chunkCorner, const vec3f& position)
{
	uint32_t chunkXZ = calcRelativePositionPtr(chunkCorner, position);
	int ht = chunks[chunkXZ + chunkHeight - 1] - chunkHeight / 2;
	return ht;
}

int Blocks::getHeightAtPoint(const vec3f& chunkCorner, const vec3f& position)
{
	uint32_t chunkXZ = calcRelativePositionPtr(chunkCorner, position);
	int y = (int)(position.y - chunkCorner.y) + chunkHeight / 2;
	while (chunks[chunkXZ + y] == blockType::Air && y > 0) y--;
	return (y+1) - chunkHeight / 2;
}

// Shapes ...

void Blocks::fillCirc(int blockType, uint32_t chunkPtr, int xc, int yc, int zc, int radius)
{
	int x = 0, y = radius, z = 0;
	int d = 3 - 2 * radius;
	fillCircXY(blockType, chunkPtr, xc, yc, zc, x, y, z);
	while (y >= x)
	{
		x++;
		if (d > 0)
		{
			y--;
			d = d + 4 * (x - y) + 10;
		}
		else
			d = d + 4 * x + 6;
		fillCircXY(blockType, chunkPtr, xc, yc, zc, x, y, z);
	}
}

void Blocks::fillCircXY(int blockType, uint32_t chunkPtr, int xc, int yc, int zc, int x, int y, int z)
{
	int rd = 80;
	for (int xx = xc - x; xx <= xc + x; xx++) {
		if (rand() % 100 > rd) insertBlock(blockType, chunkPtr, xx, yc + y, zc);
		if (rand() % 100 > rd) insertBlock(blockType, chunkPtr, xx, yc - y, zc);
	}

	for (int xx = xc - y; xx <= xc + y; xx++) {
		if (rand() % 100 > rd) insertBlock(blockType, chunkPtr, xx, yc + x, zc);
		if (rand() % 100 > rd) insertBlock(blockType, chunkPtr, xx, yc - x, zc);
	}
} 

void Blocks::fillDisk(int blockType, uint32_t chunkPtr, int xc, int yc, int zc, int radius)
{
	int x = 0, z = radius, y = 0;
	int d = 3 - 2 * radius;
	fillCircXZ(blockType, chunkPtr, xc, yc, zc, x, y, z);
	while (z >= x)
	{
		x++;
		if (d > 0)
		{
			z--;
			d = d + 4 * (x - z) + 10;
		}
		else
			d = d + 4 * x + 6;
		fillCircXZ(blockType, chunkPtr, xc, yc, zc, x, y, z);
	}
}

void Blocks::fillCircXZ(int blockType, uint32_t chunkPtr, int xc, int yc, int zc, int x, int y, int z)
{
	int rd = 70;
	for (int xx = xc - x; xx <= xc + x; xx++) {
		if (rand() % 100 > rd) insertBlock(blockType, chunkPtr, xx, yc, zc + z);
		if (rand() % 100 > rd) insertBlock(blockType, chunkPtr, xx, yc, zc - z);
	}

	for (int xx = xc - z; xx <= xc + z; xx++) {
		if (rand() % 100 > rd) insertBlock(blockType, chunkPtr, xx, yc, zc + x);
		if (rand() % 100 > rd) insertBlock(blockType, chunkPtr, xx, yc, zc - x);
	}
}

void Blocks::bresSphere(int blockType, uint32_t chunkPtr, int xc, int yc, int zc, int radius)
{
	int x = 0, z = radius, y = 0;
	int d = 3 - 2 * radius;
	fillCirc(blockType, chunkPtr, xc, yc, zc + z, x);
	fillCirc(blockType, chunkPtr, xc, yc, zc - z, x);
	fillCirc(blockType, chunkPtr, xc, yc, zc + x, z);
	fillCirc(blockType, chunkPtr, xc, yc, zc - x, z);
	while (z >= x)
	{
		x++;
		if (d > 0)
		{
			z--;
			d = d + 4 * (x - z) + 10;
		}
		else
			d = d + 4 * x + 6;
		fillCirc(blockType, chunkPtr, xc, yc, zc + z, x);
		fillCirc(blockType, chunkPtr, xc, yc, zc - z, x);
		fillCirc(blockType, chunkPtr, xc, yc, zc + x, z);
		fillCirc(blockType, chunkPtr, xc, yc, zc - x, z);
	}
}

void Blocks::bresCone(int blockType, uint32_t chunkPtr, int xc, int yc, int zc, int radius, int height)
{
	float r = radius, rd = (float)radius / (float)(height+1);
	for (int h = 0; h < height; h++) {
		fillDisk(blockType, chunkPtr, xc, yc+h, zc, r);
		r -= rd;
	}
}



void Blocks::createTrees(int chunkX, int chunkZ, int dispersion)
{
	uint32_t chunkPtr = calcChunkPtr(chunkX, chunkZ);

	//set random seed for this chunk based on chunk position
	srand(chunkX + chunkZ * 10000); 

	for (int z = 0; z < chunkDepth; z++) {
		for (int x = 0; x < chunkWidth; x++) {
			if (rand() % dispersion == 0) {
				uint16_t bark = blockType::OakWood, leaves = blockType::OakLeaves;
				switch (rand() % 7)
				{
				case 3: bark = blockType::BirchWood; leaves = blockType::BirchLeaves; break;
				case 4: bark = blockType::BirchWood; leaves = blockType::BirchLeaves; break;
				case 5: bark = blockType::SpruceWood; leaves = blockType::SpruceLeaves; break;
				case 6: bark = blockType::SpruceWood; leaves = blockType::SpruceLeaves; break;
				}

				addTree(chunkPtr, x, z, 5 + (rand() % 12), bark, leaves);
			}
		}
	}
}

void Blocks::addTree(uint32_t chunkPtr, int x, int z, int size, int bark, int leaves)
{
	uint32_t basePtr = chunkPtr + (x + z * chunkPitch) * chunkHeight;
	uint32_t ptr = basePtr + chunkHeight - 2; //start at top of chunks (but skip ground level byte)

	//exit if coords outside or no air found
	if (ptr<0 || ptr>=chunkMapSize || chunks[ptr] != blockType::Air) return; 

	//Find solid ground ...
	while (ptr > basePtr && (typToTex[chunks[ptr]].geomType != 1)) ptr--; //find solid ground

	//Create trunk
	int trunkHeight = (uint32_t)((float)size * 0.6f);


	//Create leaves (eroded sphere)
	int y = ptr - basePtr;
	if (bark == blockType::SpruceWood) {
		bresCone(leaves, chunkPtr, x, y + size * 0.2f, z, size/2, size);
		trunkHeight = size;
	}
	else {
		bresSphere(leaves, chunkPtr, x, y + size * 0.6f, z, size / 4);
	}
	for (uint32_t t = ptr; t < (ptr + trunkHeight); t++) chunks[t] = bark;

}
