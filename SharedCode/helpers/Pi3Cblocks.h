#pragma once

#include "Pi3Cvector.h"
#include "Pi3Cmodel.h"
#include "Pi3Cresource.h"
#include "Pi3Cscene.h"
#include "FastNoiseLite.h"
#include "Pi3CvertsPtr.h"

class Blocks {
public:
	Blocks() {}
	Blocks(int MapSize, int ChunkWidth = 16, int ChunkDepth = 16, int ChunkHeight = 256);

	void createBlocks(int MapSize, int ChunkWidth = 16, int ChunkDepth = 16, int ChunkHeight = 256);
	void createMapChunk(int x, int z, uint32_t flags);
	void createTexPackUV();
	void newMeshChunk(Pi3Cresource* resource, Pi3Cscene* scene, int cx, int cz, int blocksId, int texRef);
	void createMeshChunk(Pi3Cresource* resource, Pi3Cmesh& mesh, Pi3Cmesh& watermesh, int chunkX, int chunkZ);
	void updateMeshChunk(Pi3Cresource* resource, Pi3Cscene* scene, const vec3f& position);
	void createTrees(int x, int z, int trees);
	void insertBlock(uint8_t blockType, uint32_t chunkPtr, int x, int y, int z);
	void insertBlock(uint8_t blockType, const vec3f& position);

	inline int posiChunk(int c) { return (c + mapSize * 10000) % mapSize; }

	uint32_t calcChunkPtr(int chunkX, int chunkZ) { 
		return posiChunk(chunkX) * chunkSlice + posiChunk(chunkZ) * mapSize * chunkSize;
	}

	int32_t calcVoxelPtr(const uint32_t chunkPtr, const int x, const int z) {
		return chunkPtr + (x + z * chunkPitch) * chunkHeight;
	}

	int32_t calcRelativePositionPtr(const vec3f& position);
	int getGroundHeight(const vec3f& position);
	int getHeightAtPoint(const vec3f& position);

	enum { BLK_CAVES=1, BLK_VEGETATION=2 };
	
private:
	void updatePackedVert(std::vector<float>& verts, uint32_t& vc, const vec3f& position, const vec3f& normal, const vec2f& uv, const uint32_t col);
	void addQuadTopBottom(std::vector<float>& verts, uint32_t& vc, int x, float h, int y, uint8_t mapval, uint8_t faceVal, int tb, int light, int shadowEdge);
	void addQuadLeftRight(std::vector<float>& verts, uint32_t& vc, int x, int h, int y, uint8_t mapval, uint8_t faceVal, int lr, int light, int shadowEdge);
	void addQuadFrontBack(std::vector<float>& verts, uint32_t& vc, int x, int h, int y, uint8_t mapval, uint8_t faceVal, int fb, int light, int shadowEdge);
	void addXshape(std::vector<float>& verts, uint32_t& vc, int x, int y, int z, uint8_t mapVal, int light, int shadowEdge);
	void addTree(uint32_t chunkPtr, int x, int z, int size, int bark, int leaves);

	void fillCircXY(int blockType, uint32_t chunkPtr, int xc, int yc, int zc, int x, int y, int z);
	void fillCircXZ(int blockType, uint32_t chunkPtr, int xc, int yc, int zc, int x, int y, int z);
	void fillCirc(int blockType, uint32_t chunkPtr, int xc, int yc, int zc, int radius);
	void fillDisk(int blockType, uint32_t chunkPtr, int xc, int yc, int zc, int radius);

	void bresSphere(int blockType, uint32_t chunkPtr, int xc, int yc, int zc, int radius);
	void bresCone(int blockType, uint32_t chunkPtr, int xc, int yc, int zc, int radius, int height);

	std::vector<std::vector<vec2f>> texPackUV; //uvs for pixel (cube) sides (top, left, right, front, back, bottom)

	FastNoiseLite noise;

	//Chunk data containing all data within view
	std::vector<uint8_t> chunks;
	int32_t mapSize = 8;
	int32_t chunkMapSize;
	int32_t chunkSize = 0;
	int32_t chunkWidth = 16;
	int32_t chunkDepth = 16;
	int32_t chunkHeight = 256;
	int32_t chunkSlice, chunkPitch, chunkPitchHeight;
	int32_t chunkLeft, chunkRight, chunkBack, chunkFront;
	int32_t chunkLeftBack, chunkRightBack, chunkLeftFront, chunkRightFront;
	int32_t chunkWrapLeft, chunkWrapRight, chunkWrapBack, chunkWrapFront;

	float textureDiv = 1.f / 16.f; //Assumes texture is 16x16 texture map
	Pi3Cmesh blockmesh, watermesh;
};

class ivec
{
public:
	ivec() : x(0), y(0), z(0) { }
	ivec(int x, int y, int z) : x(x), y(y), z(z) { }
	ivec operator+(const ivec& v) { return ivec(this->x + v.x, this->y + v.y, this->z + v.z); }
	ivec operator-(const ivec& v) { return ivec(this->x - v.x, this->y - v.y, this->z - v.z); }
	int x; int y; int z;
};

class xblock {
public:
	float x;
	float y;
	float z;
	uint32_t  b;
	uint8_t  doorTop;

	xblock(float _x, float _y, float _z, uint32_t  _b, bool _doorTop) { x = _x; y = _y; z = _z; b = _b; doorTop = _doorTop; }
};

enum blockTypes { air, solid, seethrough, xshape, water, stick, table, unknown, steps, slab, door, ladder, thinslab, verythin  };

static const uint32_t  paintCols[16] = {
	0xffffff, 0xaaffff, 0xffaaff, 0xaaaaff, 0xffffaa, 0xaaffaa, 0xffaaaa, 0xaaaaaa,
	0x777777, 0x008888, 0x880088, 0x000088, 0x888800, 0x008800, 0x880000, 0x222222
}; //A set of colours for non-block shapes

static const uint32_t  paintCols2[16] = {
	0xffffff, 0xffffaa, 0xffaaff, 0xffaaaa, 0xaaffff, 0xaaffaa, 0xaaaaff, 0xaaaaaa,
	0x777777, 0x888800, 0x880088, 0x880000, 0x008888, 0x008800, 0x000088, 0x222222
}; //Another set of colours for blocks!

enum blockFlags { bf_none = 0, bf_nonblock = 1, bf_block = 2, bf_stackable=4, bf_seethrough=8 };

class blockInfo {
public:
	uint8_t  topx;		//texture top/bottom
	uint8_t  topy;
	uint8_t  sidesx;	//texture X side
	uint8_t  sidesy;
	uint8_t  frontx;	//texture Z front
	uint8_t  fronty;
	uint8_t  backx;		//texture Z back
	uint8_t  backy;
	uint32_t colSides;
	uint32_t col;		//Colour (RRGGBB)
	uint8_t  geomType;	//Geometry ref
	uint8_t  flags;		//hard material (i.e. stackable)

	blockInfo(uint8_t  _topx, uint8_t  _topy, uint8_t  _sidesx, uint8_t  _sidesy, uint8_t  _frontx, uint8_t  _fronty, uint8_t  _backx, uint8_t  _backy, uint32_t  _colSides, uint32_t  _col, uint8_t  _geom, uint8_t  _flags)
	{
		topx = _topx; topy = _topy; sidesx = _sidesx; sidesy = _sidesy; frontx = _frontx; fronty = _fronty; backx = _backx; backy = _backy; colSides = _colSides;  col = _col;  geomType = _geom;  flags = _flags;
	}
};

static const blockInfo typToTex[256] = {
	//TOP TEXTURES (x,y,block_flag)...
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, blockTypes::air, bf_none),						//Air = 0;
	blockInfo(1, 1, 1, 1, 1, 1, 1, 1, 0xffffff, 0xffffff, blockTypes::solid, bf_stackable | bf_block),		//Bedrock = 1;
	blockInfo(2, 0, 2, 0, 2, 0, 2, 0, 0xffffff, 0xffffff, blockTypes::solid, bf_stackable | bf_block),		//Dirt = 2;
	blockInfo(1, 0, 1, 0, 1, 0, 1, 0, 0xffffff, 0xffffff, blockTypes::solid, bf_stackable | bf_block),		//Granite = 3;
	blockInfo(0, 11, 0, 11, 0, 11, 0, 11, 0xffffff, 0xffffff, blockTypes::solid, bf_stackable | bf_block),	//Sandstone = 4
	blockInfo(0, 1, 0, 1, 0, 1, 0, 1, 0xffffff, 0xffffff, blockTypes::solid, bf_stackable | bf_block),		//Cobblestone = 5
	blockInfo(3, 1, 3, 1, 3, 1, 3, 1, 0xffffff, 0xffffff, blockTypes::solid, bf_stackable | bf_block),		//Gravel = 6
	blockInfo(2, 1, 2, 1, 2, 1, 2, 1, 0xffffff, 0xffffff, blockTypes::solid, bf_stackable | bf_block),		//Sand = 7
	blockInfo(0, 0, 3, 0, 3, 0, 3, 0, 0xffffff, 0x4DCC4D, blockTypes::solid, bf_stackable | bf_block),		//Grass block = 8
	blockInfo(5, 1, 4, 1, 4, 1, 4, 1, 0xffffff, 0xffffff, blockTypes::solid, bf_stackable | bf_block),		//Oak Wood = 9
	blockInfo(5, 1, 5, 7, 5, 7, 5, 7, 0xffffff, 0xffffff, blockTypes::solid, bf_stackable | bf_block),		//Birch Wood = 10
	blockInfo(5, 1, 4, 7, 4, 7, 4, 7, 0xffffff, 0xffffff, blockTypes::solid, bf_stackable | bf_block),		//Spruce Wood = 11
	blockInfo(4, 3, 4, 3, 4, 3, 4, 3, 0x80ffaa, 0x80ffaa, blockTypes::seethrough, bf_stackable | bf_block | bf_seethrough),		//Oak Leaves = 12
	blockInfo(4, 3, 4, 3, 4, 3, 4, 3, 0x80ee80, 0x80ee80, blockTypes::seethrough, bf_stackable | bf_block | bf_seethrough),		//Birch Leaves = 13
	blockInfo(4, 8, 4, 8, 4, 8, 4, 8, 0x90CC80, 0x90CC80, blockTypes::seethrough, bf_stackable | bf_block | bf_seethrough),		//Spruce Leaves = 14
	blockInfo(3, 4, 3, 4, 3, 4, 3, 4, 0xffffff, 0xffffff, blockTypes::seethrough, bf_stackable | bf_block | bf_seethrough),		//Glass = 15
	blockInfo(2, 2, 2, 2, 2, 2, 2, 2, 0xffffff, 0xffffff, blockTypes::solid, bf_stackable | bf_block),		//Coal Ore = 16
	blockInfo(4, 0, 4, 13, 4, 13, 4, 13, 0xffffff, 0xffffff, blockTypes::seethrough, bf_stackable | bf_block),		//Wicker Lamp = 17
	blockInfo(13, 11, 13, 11, 13, 11, 13, 11, 0xE06666, 0xE06666, blockTypes::water, bf_block | bf_seethrough), //Water = 18
	blockInfo(5, 5, 5, 5, 5, 5, 5, 5, 0x4DCC4D, 0x4DCC4D, blockTypes::xshape, bf_nonblock),		//Tall Grass = 19			X
	blockInfo(12, 0, 12, 0, 12, 0, 12, 0, 0xffffff, 0xffffff, blockTypes::xshape, bf_nonblock),	//Red Flower = 20			X
	blockInfo(4, 0, 4, 0, 4, 0, 4, 0, 0xffffff, 0xffffff, blockTypes::solid, bf_stackable | bf_block),		//Planks = 21
	blockInfo(14, 3, 14, 3, 14, 3, 14, 3, 0xffffff, 0xffffff, blockTypes::solid, bf_stackable | bf_block),	//Coal Chunk = 22
	blockInfo(3, 13, 3, 13, 3, 13, 3, 13, 0xffffff, 0xffffff, blockTypes::stick, bf_nonblock),	//Stick = 23				thin box
	blockInfo(13, 0, 13, 0, 13, 0, 13, 0, 0xffffff, 0xffffff, blockTypes::xshape, bf_nonblock),	//Purple Flower = 24		X
	blockInfo(14, 0, 14, 0, 14, 0, 14, 0, 0xffffff, 0xffffff, blockTypes::xshape, bf_nonblock),	//White Flower = 25			X
	blockInfo(6, 3, 6, 3, 6, 3, 6, 3, 0xffffff, 0xffffff, blockTypes::solid, bf_stackable | bf_block),		//Stone Bricks = 26
	blockInfo(11, 2, 15, 2, 15, 2, 15, 2, 0xffffff, 0xffffff, blockTypes::table, bf_stackable | bf_nonblock),	//Crafting Table = 27		table
	blockInfo(7, 3, 7, 3, 7, 3, 7, 3, 0xffffff, 0xffffff, blockTypes::xshape, bf_nonblock),		//Dry Bush = 28				X
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Stone Axe = 29
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Wooden Shovel = 30
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, bf_none),						//Torch = 31				thin box (angled/rotated)
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Wooden Pickaxe = 32
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Wooden Axe = 33
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Stone Shovel = 34
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Stone Pickaxe = 35
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Iron Shovel = 36
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Iron Axe = 37
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Iron Pickaxe = 38
	blockInfo(1, 2, 1, 2, 1, 2, 1, 2, 0xffffff, 0xffffff, blockTypes::solid, bf_stackable | bf_block),		//Iron Ore = 39
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_stackable | bf_block),						//Iron Ingot = 40
	blockInfo(0, 2, 0, 2, 0, 2, 0, 2, 0xffffff, 0xffffff, blockTypes::solid, bf_stackable | bf_block),		//Copper Ore = 41
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_stackable | bf_block),						//Copper Ingot = 42
	blockInfo(3, 11, 3, 11, 3, 11, 3, 11, 0xffffff, 0xffffff, blockTypes::solid, bf_stackable | bf_block),	//Malachite Chunk = 43
	blockInfo(3, 5, 3, 5, 3, 5, 3, 5, 0xffffff, 0xffffff, blockTypes::seethrough, bf_stackable | bf_block),		//Framed Glass = 44
	blockInfo(10, 2, 9, 1, 11, 1, 10, 1, 0xffffff, 0xffffff, blockTypes::solid, bf_stackable | bf_block),		//Chest = 45
	blockInfo(15, 3, 15, 3, 15, 3, 15, 3, 0xffffff, 0xffffff, blockTypes::solid, bf_stackable | bf_block),	//Solid Iron Block = 46
	blockInfo(5, 11, 5, 11, 5, 11, 5, 11, 0xffffff, 0xffffff, blockTypes::solid, bf_stackable | bf_block),	//Solid Copper Block = 47
	blockInfo(0, 1, 0, 1, 0, 1, 0, 1, 0xffffff, 0xffffff, blockTypes::steps, bf_stackable | bf_nonblock),		//Cobblestone Stairs = 48		steps
	blockInfo(4, 0, 4, 0, 4, 0, 4, 0, 0xffffff, 0xffffff, blockTypes::steps, bf_stackable | bf_nonblock),		//Wooden Stairs = 49			steps
	blockInfo(6, 3, 6, 3, 6, 3, 6, 3, 0xffffff, 0xffffff, blockTypes::steps, bf_stackable | bf_nonblock),		//Stone Brick Stairs = 50		steps
	blockInfo(0, 11, 0, 11, 0, 11, 0, 11, 0xffffff, 0xffffff, blockTypes::steps, bf_stackable | bf_nonblock),	//Sandstone Stairs = 51			steps
	blockInfo(0, 11, 0, 11, 0, 11, 0, 11, 0xffffff, 0xffffff, blockTypes::slab, bf_nonblock),		//Sandstone Slab = 52  (2bf_none)		half height box
	blockInfo(0, 1, 0, 1, 0, 1, 0, 1, 0xffffff, 0xffffff, blockTypes::slab, bf_nonblock),		//Cobblestone Slab = 53			half height box
	blockInfo(6, 3, 6, 3, 6, 3, 6, 3, 0xffffff, 0xffffff, blockTypes::slab, bf_nonblock),		//Stone Brick Slab = 54			half height box
	blockInfo(4, 0, 4, 0, 4, 0, 4, 0, 0xffffff, 0xffffff, blockTypes::slab, bf_nonblock),		//Wooden Slab = 55				half height box
	blockInfo(4, 0, 4, 0, 1, 6, 1, 6, 0xffffff, 0xffffff, blockTypes::door, bf_stackable | bf_nonblock),		//Wooden Door = 56				Rotated box
	blockInfo(0, 0, 0, 0, 2, 6, 2, 6, 0xffffff, 0xffffff, blockTypes::door, bf_stackable | bf_nonblock),		//Iron Door = 57				Rotated box
	blockInfo(0, 0, 0, 0, 0, 6, 0, 6, 0xffffff, 0xffffff, blockTypes::door, bf_stackable | bf_nonblock),		//Cell Door = 58				Rotated box
	blockInfo(4, 0, 4, 0, 4, 0, 4, 0, 0xffffff, 0xffffff, blockTypes::ladder, bf_stackable | bf_nonblock),		//Ladder = 59					3x boxes
	blockInfo(3, 6, 3, 6, 3, 6, 3, 6, 0xffffff, 0xffffff, blockTypes::seethrough, bf_stackable | bf_block | bf_seethrough),		//Window = 60
	blockInfo(2, 4, 2, 4, 2, 4, 2, 4, 0xffffff, 0xffffff, blockTypes::thinslab, bf_nonblock),		//Snow = 61						thin slab
	blockInfo(1, 4, 1, 4, 1, 4, 1, 4, 0xffffff, 0xffffff, blockTypes::thinslab, bf_nonblock),		//Ice = 62						thin slab
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0xffffff, 0xffffff, 14, bf_none),		//Christmas Tree = 63			Xmas tree
	blockInfo(12, 2, 12, 2, 14, 2, 12, 2, 0xffffff, 0xffffff, 46, bf_stackable | bf_block),	//Furnace = 64
	blockInfo(12, 2, 12, 2, 14, 2, 12, 2, 0xffffff, 0xffffff, 46, bf_stackable | bf_block),	//Furnace = 65
	blockInfo(5, 0, 5, 0, 5, 0, 5, 0, 0xffffff, 0xffffff, blockTypes::solid, bf_stackable | bf_block),		//Limestone = 66
	blockInfo(6, 0, 6, 0, 6, 0, 6, 0, 0xffffff, 0xffffff, blockTypes::solid, bf_stackable | bf_block),		//Basalt = 67
	blockInfo(7, 0, 7, 0, 7, 0, 7, 0, 0xffffff, 0xffffff, blockTypes::solid, bf_stackable | bf_block),		//Marble = 68
	blockInfo(7, 0, 7, 0, 7, 0, 7, 0, 0xffffff, 0xffffff, blockTypes::steps, bf_stackable | bf_nonblock),		//Marble Stairs = 69				steps
	blockInfo(7, 0, 7, 0, 7, 0, 7, 0, 0xffffff, 0xffffff, blockTypes::slab, bf_nonblock),		//Marble Slab = 70				half height box
	blockInfo(3, 11, 3, 11, 3, 11, 3, 11, 0xffffff, 0xffffff, blockTypes::solid, bf_stackable | bf_block),	//Malachite Block = 71
	blockInfo(8, 0, 8, 0, 8, 0, 8, 0, 0xffffff, 0xffffff, blockTypes::solid, bf_stackable | bf_block),		//Clay = 72
	blockInfo(6, 4, 6, 4, 6, 4, 6, 4, 0xffffff, 0xffffff, blockTypes::solid, bf_stackable | bf_block),		//Brick Wall = 73
	blockInfo(6, 4, 6, 4, 6, 4, 6, 4, 0xffffff, 0xffffff, blockTypes::solid, bf_stackable | bf_block),		//Brick = 74
	blockInfo(6, 4, 6, 4, 6, 4, 6, 4, 0xffffff, 0xffffff, blockTypes::slab, bf_nonblock),		//Brick Slab = 75				half height box
	blockInfo(6, 4, 6, 4, 6, 4, 6, 4, 0xffffff, 0xffffff, blockTypes::steps, bf_stackable | bf_nonblock),		//Brick Stairs = 76				steps
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Raw Bird = 77
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Cooked Bird = 78
	blockInfo(1, 1, 1, 1, 1, 1, 1, 1, 0xffffff, 0xffffff, blockTypes::solid, bf_stackable | bf_block),		//Stone Chunk = 79
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Wooden Spear = 80
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Stone Spear = 81
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Iron Spear = 82
	blockInfo(3, 7, 4, 0, 4, 0, 4, 0, 0xffffff, 0xffffff, 13, bf_nonblock),		//Wooden Trapdoor = 83			thin slab
	blockInfo(2, 7, 0, 0, 0, 0, 0, 0, 0xffffff, 0xffffff, 13, bf_nonblock),		//Cell Trapdoor = 84			thin slab
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Snowball = 85
	blockInfo(4, 0, 4, 0, 4, 0, 4, 0, 0xffffff, 0xffffff, 15, bf_nonblock),		//Spiked Plank = 86				spiked plank
	blockInfo(6, 1, 6, 1, 6, 1, 6, 1, 0xffffff, 0x4DCC4D, 13, bf_nonblock),		//Grass Trap = 87				thin box
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Raw Bacon = 88
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Cooked Bacon = 89
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Bucket = 90
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Water Bucket = 91
	blockInfo(14, 7, 14, 7, 14, 7, 14, 7, 0xffffff, 0xffffff, blockTypes::solid, bf_stackable | bf_block),	//Magma = 92
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Magma Bucket = 93
	blockInfo(15, 2, 15, 2, 15, 2, 15, 2, 0xffffff, 0xffffff, 16, bf_stackable | bf_nonblock),	//Wooden Fence = 94
	blockInfo(6, 0, 6, 0, 6, 0, 6, 0, 0xffffff, 0xffffff, blockTypes::slab, bf_nonblock),		//Basalt Slab = 95				half height box
	blockInfo(6, 0, 6, 0, 6, 0, 6, 0, 0xffffff, 0xffffff, blockTypes::steps, bf_stackable | bf_nonblock),		//Basalt Stairs = 96			steps
	blockInfo(4, 0, 4, 0, 4, 0, 4, 0, 0xffffff, 0xffffff, 17, bf_none),		//Sign = 97						signboard
	blockInfo(4, 0, 4, 0, 4, 0, 4, 0, 0xffffff, 0xffffff, 17, bf_none),		//Sign = 98						signboard
	blockInfo(7, 4, 7, 4, 7, 4, 7, 4, 0xffffff, 0xffffff, blockTypes::xshape, bf_none),		//Large Dry Bush = 99			X
	blockInfo(3, 2, 3, 2, 3, 2, 3, 2, 0xffffff, 0xffffff, blockTypes::solid, bf_stackable | bf_block),		//Saltpeter Ore = 100
	blockInfo(4, 2, 4, 2, 4, 2, 4, 2, 0xffffff, 0xffffff, blockTypes::solid, bf_stackable | bf_block),		//Sulphur Ore = 101
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Saltpeter Chunk = 102
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Sulphur Chunk = 103
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18, bf_none),						//Fire = 104
	blockInfo(13, 3, 13, 3, 13, 3, 13, 3, 0xffffff, 0xffffff, 19, bf_nonblock),	//Small Gunpowder Keg = 105		keg1
	blockInfo(13, 3, 13, 3, 13, 3, 13, 3, 0xffffff, 0xffffff, 20, bf_nonblock),	//Medium Gunpowder Keg = 106	keg2
	blockInfo(13, 3, 13, 3, 13, 3, 13, 3, 0xffffff, 0xffffff, 21, bf_stackable | bf_nonblock),	//Large Gunpowder Keg = 107		keg3
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Match = 108
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Gunpowder = 109
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Milk Bucket = 110
	blockInfo(6, 11, 6, 11, 6, 11, 6, 11, 0xffffff, 0xffffff, blockTypes::solid, bf_stackable | bf_block),	//Diamond = 111
	blockInfo(0, 3, 0, 3, 0, 3, 0, 3, 0xffffff, 0xffffff, blockTypes::solid, bf_stackable | bf_block),		//Diamond Ore = 112
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Diamond Edge Shovel= 113
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Diamond Edge Axe= 114
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Diamond Tip Pickaxe= 115
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Diamond Tip Spear = 116
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Compass = 117
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Egg = 118
	blockInfo(8, 4, 8, 4, 8, 4, 8, 4, 0xffffff, 0xffffff, blockTypes::xshape, bf_none),		//Sapling = 119					X
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Thermometer = 120
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Hygrometer = 121
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Wooden Machete = 122
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Stone Machete = 123
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Iron Machete = 124
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Diamond Edge Machete = 125
	blockInfo(6, 11, 6, 11, 6, 11, 6, 11, 0xffffff, 0xffffff, blockTypes::solid, bf_stackable | bf_block),	//Solid Diamond Block = 126
	blockInfo(5, 6, 4, 6, 4, 6, 4, 6, 0xffffff, 0xffffff, 22, bf_none),		//Cactus = 127					tall box
	blockInfo(7, 1, 7, 1, 7, 1, 7, 1, 0xffffff, 0xffffff, blockTypes::solid, bf_stackable | bf_block),		//Painted Planks = 128			
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Paint Bucket = 129
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//White Pigment = 130
	blockInfo(6, 6, 6, 6, 6, 6, 6, 6, 0xffffff, 0xffffff, 23, bf_nonblock),		//Pumpkin = 131					pumpkin
	blockInfo(7, 6, 7, 6, 7, 6, 7, 6, 0xffffff, 0xffffff, 23, bf_nonblock),		//Jack O'Lantern = 132			pumpkin
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0xffffff, 0xffffff, 24, bf_nonblock),		//Electric Wire = 133
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0xffffff, 0xffffff, 24, bf_nonblock),		//Electric Wire = 134
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0xffffff, 0xffffff, 24, bf_nonblock),		//Electric Wire = 135
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0xffffff, 0xffffff, blockTypes::slab, bf_none),		//Granite slab = 136
	blockInfo(14, 4, 14, 4, 5, 8, 14, 4, 0xffffff, 0xffffff, 25, bf_nonblock),	//Logic AND Gate = 137
	blockInfo(7, 7, 6, 7, 6, 7, 6, 7, 0xffffff, 0xffffff, blockTypes::solid, bf_stackable | bf_block),		//Battery = 138
	blockInfo(15, 4, 15, 4, 8, 6, 15, 4, 0xffffff, 0xffffff, blockTypes::thinslab, bf_nonblock),	//Lightbulb = 139
	blockInfo(14, 4, 14, 4, 6, 9, 14, 4, 0xffffff, 0xffffff, 27, bf_none),	//Logic NOT Gate = 140
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0xffffff, 0xffffff, 28, bf_none),		//Switch = 141
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0xffffff, 0xffffff, 29, bf_none),		//Button = 142
	blockInfo(14, 4, 14, 4, 5, 9, 14, 4, 0xffffff, 0xffffff, 30, bf_none),	//Logic OR Gate = 143
	blockInfo(4, 0, 4, 0, 4, 0, 4, 0, 0xffffff, 0xffffff, blockTypes::thinslab, bf_none),		//Pressure Plate = 144
	blockInfo(14, 4, 14, 4, 6, 10, 14, 4, 0xffffff, 0xffffff, 32, bf_none),	//Delay Gate = 145
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0xffffff, 0xffffff, 33, bf_none),		//SR Latch = 146
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0xffffff, 0xffffff, 34, bf_none),		//Electric Detonator = 147
	blockInfo(1, 3, 1, 3, 1, 3, 1, 3, 0xffffff, 0xffffff, blockTypes::solid, bf_stackable | bf_block),		//Germanium Ore = 148
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Germanium Crystal = 149
	blockInfo(14, 3, 14, 3, 14, 3, 14, 3, 0xffffff, 0xffffff, blockTypes::solid, bf_stackable | bf_block),	//Solid Coal Block = 150
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0xffffff, 0xffffff, 35, bf_none),		//Photodiode = 151
	blockInfo(14, 4, 14, 4, 8, 7, 14, 4, 0xffffff, 0xffffff, blockTypes::thinslab, bf_none),	//LED = 152
	blockInfo(4, 0, 4, 0, 8, 10, 8, 10, 0xffffff, 0xffffff, 48, bf_none),		//Wire Through Planks = 153
	blockInfo(0, 0, 0, 0, 8, 11, 8, 11, 0xffffff, 0xffffff, 48, bf_none),		//Wire Through Stone = 154
	blockInfo(14, 4, 14, 4, 8, 7, 8, 7, 0xffffff, 0xffffff, 48, bf_none),		//Wire Through Semiconductor = 155
	blockInfo(14, 4, 14, 4, 8, 12, 14, 4, 0xffffff, 0xffffff, 30, bf_none),	//Logic XOR Gate = 156
	blockInfo(14, 4, 14, 4, 7, 12, 14, 4, 0xffffff, 0xffffff, 12, bf_none),	//Random Generator = 157
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Saddle = 158
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Leather = 159
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Whistle = 160
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Raw Fish = 161
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Cooked Fish = 162
	blockInfo(1, 0, 1, 0, 1, 0, 1, 0, 0xffffff, 0xffffff, 45, bf_stackable),		//Granite Fence = 163
	blockInfo(6, 4, 6, 4, 6, 4, 6, 4, 0xffffff, 0xffffff, 45, bf_stackable),		//Brick Fence = 164
	blockInfo(6, 0, 6, 0, 6, 0, 6, 0, 0xffffff, 0xffffff, 45, bf_stackable),		//Basalt Fence = 165
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0xffffff, 0xffffff, 37, bf_stackable),		//Wooden Fence Gate = 166			fence
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0xffffff, 0xffffff, 38, bf_none),		//Magnet = 167						magnet
	blockInfo(5, 2, 2, 0, 2, 0, 2, 0, 0xffffff, 0xffffff, blockTypes::solid, bf_none),		//Soil = 168						thin slab
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Wooden Rake = 169
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Stone Rake = 170
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Iron Rake = 171
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Diamond Spikes Rake = 172
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						// = 173
	blockInfo(14, 5, 14, 5, 14, 5, 14, 5, 0xffffff, 0xffffff, blockTypes::xshape, bf_none),	//Rye = 174							X	
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Flour = 175
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Dough = 176
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Bread = 177
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Boat = 178
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0xffffff, 0xffffff, 39, bf_none),		//Motion Detector = 179
	blockInfo(14, 4, 14, 4, 9, 8, 14, 4, 0xffffff, 0xffffff, blockTypes::thinslab, bf_none),	//Digital To Analog Converter = 180
	blockInfo(14, 4, 14, 4, 9, 9, 14, 4, 0xffffff, 0xffffff, blockTypes::thinslab, bf_none),	//Analog To Digital Converter = 181
	blockInfo(14, 4, 14, 4, 9, 10, 14, 4, 0xffffff, 0xffffff, blockTypes::thinslab, bf_none),	//4-LED = 182
	blockInfo(14, 4, 14, 4, 9, 11, 14, 4, 0xffffff, 0xffffff, 40, bf_none),	//Sound Generator = 183
	blockInfo(14, 4, 14, 4, 9, 13, 14, 4, 0xffffff, 0xffffff, 41, bf_none),	//4-Bit Counter = 184
	blockInfo(14, 4, 14, 4, 9, 12, 14, 4, 0xffffff, 0xffffff, 41, bf_none),	//7-Segment Display = 185
	blockInfo(14, 4, 14, 4, 8, 14, 14, 4, 0xffffff, 0xffffff, blockTypes::thinslab, bf_none),	//Memory Bank = 186
	blockInfo(14, 4, 14, 4, 9, 14, 14, 4, 0xffffff, 0xffffff, blockTypes::thinslab, bf_none),	//Real Time Clock = 187
	blockInfo(14, 4, 14, 4, 10, 7, 14, 4, 0xffffff, 0xffffff, blockTypes::thinslab, bf_none),	//Truth Table Circuit = 188
	blockInfo(9, 2, 9, 2, 9, 2, 9, 2, 0xffffff, 0xffffff, 41, bf_none),		//Gravestone = 189					Gravestone
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Treasure Generator = 190
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Bow = 191
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Arrow = 192
	blockInfo(10, 3, 10, 3, 10, 3, 10, 3, 0xffffff, 0xffffff, 42, bf_stackable),	//Iron Fence = 193					thin panel (rotated)
	blockInfo(10, 3, 10, 3, 10, 3, 10, 3, 0xffffff, 0xffffff, 42, bf_stackable),	//Iron Fence Gate = 194				thin panel (rotated)
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0xffffff, 0xffffff, 43, bf_none),		//Rod = 195
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Feather = 196
	blockInfo(10, 0, 10, 0, 10, 0, 10, 0, 0xffffff, 0x4DCC4D, 44, bf_none),	//Ivy = 197							thin panel (rotated)
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//String = 198
	blockInfo(8, 0, 8, 0, 8, 0, 8, 0, 0xffffff, 0xffffff, 43, bf_none),		//Target = 199
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Crossbow = 200
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//Bomb = 201
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0xffffff, 0xffffff, 45, bf_stackable),		//Stone Fence = 202					thin box (rotated)
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//null = 203
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//null = 204
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//null = 205
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//null = 206
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//null = 207	
	blockInfo(6, 2, 6, 2, 6, 2, 6, 2, 0xffffff, 0xffffff, blockTypes::thinslab, bf_none),		//carpet = 208						thin box
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//null = 209
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0xffffff, 0xffffff, 17, bf_none),		//Granite sign = 210
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//null = 211
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//null = 212
	blockInfo(15, 3, 15, 3, 15, 3, 15, 3, 0xffffff, 0xffffff, 47, 3),	//Iron ladder = 213
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//null = 214
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//null = 215
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//null = 216
	blockInfo(1, 0, 1, 0, 1, 0, 1, 0, 0xffffff, 0xffffff, blockTypes::steps, bf_none),		//Granite steps = 217					steps (rotated, cornerstone)
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//null = 218
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//null = 219
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//null = 220
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//null = 221
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//null = 222
	blockInfo(6, 4, 6, 4, 8, 9, 8, 9, 0xffffff, 0xffffff, 48, bf_stackable),		//wire brick = 223
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//null = 224
	blockInfo(4, 8, 4, 8, 4, 8, 4, 8, 0x80CC80, 0x80CC80, blockTypes::seethrough, bf_stackable | bf_block | bf_seethrough),		//Spruce leaves = 225
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//null = 226
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//null = 227
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//null = 228
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//null = 229
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//null = 230
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//null = 231
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//null = 232
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//null = 233
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//null = 234
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//null = 235
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//null = 236
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//null = 237
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//null = 238
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//null = 239
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//null = 240
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//null = 241
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//null = 242
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//null = 243
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//null = 244
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//null = 245
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//null = 246
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//null = 247
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//null = 248
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//null = 249
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//null = 250
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//null = 251
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//null = 252
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//null = 253
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none),						//null = 254
	blockInfo(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, bf_none)						//null = 255
};

namespace blockType
{
	const uint16_t Air = 0;
	const uint16_t Bedrock = 1;
	const uint16_t Dirt = 2;
	const uint16_t Granite = 3;
	const uint16_t Sandstone = 4;
	const uint16_t Cobblestone = 5;
	const uint16_t Gravel = 6;
	const uint16_t Sand = 7;
	const uint16_t Grass = 8;
	const uint16_t OakWood = 9;
	const uint16_t BirchWood = 10;
	const uint16_t SpruceWood = 11;
	const uint16_t OakLeaves = 12;
	const uint16_t BirchLeaves = 13;
	const uint16_t SpruceLeaves = 14;
	const uint16_t Glass = 15;
	const uint16_t CoalOre = 16;
	const uint16_t WickerLamp = 17;
	const uint16_t Water = 18;
	const uint16_t TallGrass = 19;
	const uint16_t RedFlower = 20;
	const uint16_t Planks = 21;
	const uint16_t CoalChunk = 22;
	const uint16_t Stick = 23;
	const uint16_t PurpleFlower = 24;
	const uint16_t WhiteFlower = 25;
	const uint16_t StoneBricks = 26;
	const uint16_t CraftingTable = 27;
	const uint16_t DryBush = 28;
	const uint16_t StoneAxe = 29;
	const uint16_t WoodenShovel = 30;
	const uint16_t Torch = 31;
	const uint16_t WoodenPickaxe = 32;
	const uint16_t WoodenAxe = 33;
	const uint16_t StoneShovel = 34;
	const uint16_t StonePickaxe = 35;
	const uint16_t IronShovel = 36;
	const uint16_t IronAxe = 37;
	const uint16_t IronPickaxe = 38;
	const uint16_t IronOre = 39;
	const uint16_t IronIngot = 40;
	const uint16_t CopperOre = 41;
	const uint16_t CopperIngot = 42;
	const uint16_t MalachiteChunk = 43;
	const uint16_t FramedGlass = 44;
	const uint16_t Chest = 45;
	const uint16_t SolidIronBlock = 46;
	const uint16_t SolidCopperBlock = 47;
	const uint16_t StoneStairs = 48;
	const uint16_t WoodenStairs = 49;
	const uint16_t StoneBrickStairs = 50;
	const uint16_t SandstoneStairs = 51;
	const uint16_t SandstoneSlab = 52;
	const uint16_t StoneSlab = 53;
	const uint16_t StoneBrickSlab = 54;
	const uint16_t WoodenSlab = 55;
	const uint16_t WoodenDoor = 56;
	const uint16_t IronDoor = 57;
	const uint16_t CellDoor = 58;
	const uint16_t Ladder = 59;
	const uint16_t Window = 60;
	const uint16_t Snow = 61;
	const uint16_t Ice = 62;
	const uint16_t ChristmasTree = 63;
	const uint16_t Furnace = 64;
	const uint16_t Furnace2 = 65;
	const uint16_t Limestone = 66;
	const uint16_t Basalt = 67;
	const uint16_t Marble = 68;
	const uint16_t MarbleStairs = 69;
	const uint16_t MarbleSlab = 70;
	const uint16_t MalachiteBlock = 71;
	const uint16_t Clay = 72;
	const uint16_t BrickWall = 73;
	const uint16_t Brick = 74;
	const uint16_t BrickSlab = 75;
	const uint16_t BrickStairs = 76;
	const uint16_t RawBird = 77;
	const uint16_t CookedBird = 78;
	const uint16_t StoneChunk = 79;
	const uint16_t WoodenSpear = 80;
	const uint16_t StoneSpear = 81;
	const uint16_t IronSpear = 82;
	const uint16_t WoodenTrapdoor = 83;
	const uint16_t CellTrapdoor = 84;
	const uint16_t Snowball = 85;
	const uint16_t SpikedPlank = 86;
	const uint16_t GrassTrap = 87;
	const uint16_t RawBacon = 88;
	const uint16_t CookedBacon = 89;
	const uint16_t Bucket = 90;
	const uint16_t WaterBucket = 91;
	const uint16_t Magma = 92;
	const uint16_t MagmaBucket = 93;
	const uint16_t WoodenFence = 94;
	const uint16_t BasaltSlab = 95;
	const uint16_t BasaltStairs = 96;
	const uint16_t Sign = 97;
	const uint16_t Sign2 = 98;
	const uint16_t LargeDryBush = 99;
	const uint16_t SaltpeterOre = 100;
	const uint16_t SulphurOre = 101;
	const uint16_t SaltpeterChunk = 102;
	const uint16_t SulphurChunk = 103;
	const uint16_t Fire = 104;
	const uint16_t SmallGunpowderKeg = 105;
	const uint16_t MediumGunpowderKeg = 106;
	const uint16_t LargeGunpowderKeg = 107;
	const uint16_t Match = 108;
	const uint16_t Gunpowder = 109;
	const uint16_t MilkBucket = 110;
	const uint16_t Diamond = 111;
	const uint16_t DiamondOre = 112;
	const uint16_t DiamondEdgeShovel = 113;
	const uint16_t DiamondEdgeAxe = 114;
	const uint16_t DiamondTipPickaxe = 115;
	const uint16_t DiamondTipSpear = 116;
	const uint16_t Compass = 117;
	const uint16_t Egg = 118;
	const uint16_t Sapling = 119;
	const uint16_t Thermometer = 120;
	const uint16_t Hygrometer = 121;
	const uint16_t WoodenMachete = 122;
	const uint16_t StoneMachete = 123;
	const uint16_t IronMachete = 124;
	const uint16_t DiamondEdgeMachete = 125;
	const uint16_t SolidDiamondBlock = 126;
	const uint16_t Cactus = 127;
	const uint16_t PaintedPlanks = 128;
	const uint16_t PaintBucket = 129;
	const uint16_t WhitePigment = 130;
	const uint16_t Pumpkin = 131;
	const uint16_t JackOLantern = 132;
	const uint16_t ElectricWire = 133;
	const uint16_t ElectricWire2 = 134;
	const uint16_t ElectricWire3 = 135;
	const uint16_t ElectricWire4 = 136;
	const uint16_t LogicANDGate = 137;
	const uint16_t Battery = 138;
	const uint16_t Lightbulb = 139;
	const uint16_t LogicNOTGate = 140;
	const uint16_t Switch = 141;
	const uint16_t Button = 142;
	const uint16_t LogicORGate = 143;
	const uint16_t PressurePlate = 144;
	const uint16_t DelayGate = 145;
	const uint16_t SRLatch = 146;
	const uint16_t ElectricDetonator = 147;
	const uint16_t GermaniumOre = 148;
	const uint16_t GermaniumCrystal = 149;
	const uint16_t SolidCoalBlock = 150;
	const uint16_t Photodiode = 151;
	const uint16_t LED = 152;
	const uint16_t WireThroughPlanks = 153;
	const uint16_t WireThroughStone = 154;
	const uint16_t WireThroughSemiconductor = 155;
	const uint16_t LogicXORGate = 156;
	const uint16_t RandomGenerator = 157;
	const uint16_t Saddle = 158;
	const uint16_t Leather = 159;
	const uint16_t Whistle = 160;
	const uint16_t RawFish = 161;
	const uint16_t CookedFish = 162;
	const uint16_t PaintedGranite = 163;
	const uint16_t PaintedBricks = 164;
	const uint16_t PaintedBasalt = 165;
	const uint16_t WoodenFenceGate = 166;
	const uint16_t Magnet = 167;
	const uint16_t Soil = 168;
	const uint16_t WoodenRake = 169;
	const uint16_t StoneRake = 170;
	const uint16_t IronRake = 171;
	const uint16_t DiamondSpikesRake = 172;
	const uint16_t null = 173;
	const uint16_t Rye = 174;
	const uint16_t Flour = 175;
	const uint16_t Dough = 176;
	const uint16_t Bread = 177;
	const uint16_t Boat = 178;
	const uint16_t MotionDetector = 179;
	const uint16_t DigitalToAnalogConverter = 180;
	const uint16_t AnalogToDigitalConverter = 181;
	const uint16_t FourLED = 182;
	const uint16_t SoundGenerator = 183;
	const uint16_t FourBitCounter = 184;
	const uint16_t SevenSegmentDisplay = 185;
	const uint16_t MemoryBank = 186;
	const uint16_t RealTimeClock = 187;
	const uint16_t TruthTableCircuit = 188;
	const uint16_t Gravestone = 189;
	const uint16_t TreasureGenerator = 190;
	const uint16_t Bow = 191;
	const uint16_t Arrow = 192;
	const uint16_t IronFence = 193;
	const uint16_t IronFenceGate = 194;
	const uint16_t Rod = 195;
	const uint16_t Feather = 196;
	const uint16_t Ivy = 197;
	const uint16_t String = 198;
	const uint16_t Target = 199;
	const uint16_t Crossbow = 200;
	const uint16_t Bomb = 201;
	//const uint16_t StoneFence = 202;
	//const uint16_t StoneFence = 203;
	//const uint16_t StoneFence = 204;
	//const uint16_t StoneFence = 205;
	//const uint16_t StoneFence = 206;
	//const uint16_t StoneFence = 207;
	//const uint16_t StoneFence = 208;
	//const uint16_t StoneFence = 209;
	//const uint16_t StoneFence = 210;
	//const uint16_t StoneFence = 211;
	//const uint16_t StoneFence = 212;
	//const uint16_t StoneFence = 213;
	//const uint16_t StoneFence = 214;
	//const uint16_t StoneFence = 215;
	//const uint16_t StoneFence = 216;
	const uint16_t StoneStairs2 = 217;
}
