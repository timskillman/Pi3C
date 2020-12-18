#pragma once
#include "Pi3Cresource.h"
#include "Pi3Cmodel.h"

class Pi3Cpopulate {
public:

	struct mapper {
		std::vector<Pi3Cmodel> mergeModels;
		const std::string mapImagePath;
	};

	struct shapeArray {

		void init(const uint32_t maxTypes) {
			mesh.reset(new Pi3Cmesh);
			textureStep = 1.f / (float)maxTypes;
		}

		void create(const vec3f& pos, const vec2f& size, const uint32_t type) {
			positions.push_back(pos);
			sizes.push_back(size);
			types.push_back(type);
			mesh->addXshape(pos, size, vec2f((float)(type)* textureStep, 0.f), vec2f(textureStep, 0.99f));
		}

		std::unique_ptr<Pi3Cmesh> mesh;
		std::vector<vec3f> positions;
		std::vector<vec2f> sizes;
		std::vector<uint32_t> types;
		float textureStep = 1.f;		//usually four trees per texture - but needs setting
	};


	Pi3Cmodel Populate(Pi3Cresource *resource, const uint32_t maxShapes, Pi3Cmodel& model, const std::string& texturePath, const uint32_t maxTextures, const float radius, int32_t groupId);

private:
	
	float rnd(const uint32_t size) { return (float)(rand() % size); }

	shapeArray shapes;

};