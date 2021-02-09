#include "Pi3Cmodel.h"
#include "Pi3CfileOBJ.h"
#include "Pi3Cbbox2d.h"
//#include "Pi3Cgizmos.h"

//Pi3Cmodel::Pi3Cmodel(const modelParams mp)
//{
//	init();
//	name = mp.name;
//	matrix.setMoveRotate(mp.position, mp.rotate);
//
//	if (mp.path != "") {
//		if (mp.modelfile != "") {
//			loadOBJfile(mp.resource, mp.path, mp.modelfile, mp.showProgressCB, false);
//		}
//		if (mp.colliderfile != "") {
//			Pi3Cmodel collider;
//			collider.loadOBJfile(mp.resource, mp.path, mp.colliderfile, mp.showProgressCB, true);
//			this->appendCollider(collider);
//		}
//	} 
//	else if (mp.mesh) {
//		create(mp.resource, mp.mesh, mp.diffuseColour, mp.deleteVerts, mp.reserveBuffer, mp.bufferSize);
//	}
//}

Pi3Cmodel::Pi3Cmodel(Pi3Cresource *resource, Pi3Cmesh mesh, int32_t groupId, uint32_t diffuseColour)
{
	init();
	create(resource, &mesh, groupId, diffuseColour);
}

Pi3Cmodel::Pi3Cmodel(Pi3Cresource *resource, const std::string &name, Pi3Cmesh mesh, int32_t groupId, uint32_t diffuseColour)
{
	init(); this->name = name;
	create(resource, &mesh, groupId, diffuseColour);
}

Pi3Cmodel::Pi3Cmodel(Pi3Cresource *resource, const std::string &path, const std::string &modelfile, int32_t groupId, const bool asCollider, std::function<void(float)> showProgressCB)
{
	init();
	loadOBJfile(resource, path, modelfile, groupId, showProgressCB, asCollider);
}

Pi3Cmodel::Pi3Cmodel(Pi3Cresource *resource, const std::string &modelname, const std::string &path, const std::string &modelfile, int32_t groupId, const std::string &collider, std::function<void(float)> showProgressCB)
{
	init();
	loadModelAndCollider(resource, path, modelfile, groupId, collider, showProgressCB);
	name = modelname;
}

Pi3Cmodel::Pi3Cmodel(Pi3Cresource* resource, int meshRef)
{
	this->meshRef = meshRef;
	material = resource->materials[resource->meshes[meshRef].materialRef]; 	//copy material into group model so it can change;
	bbox = resource->meshes[meshRef].bbox;
}

void Pi3Cmodel::loadModelAndCollider(Pi3Cresource *resource, std::string path, std::string modelfile, int32_t groupId, std::string collider, std::function<void(float)> showProgressCB)
{
	if (modelfile != "") loadOBJfile(resource, path, modelfile, groupId, showProgressCB, false);
	if (collider != "") {
		Pi3Cmodel c1(resource, path, collider, groupId, true, showProgressCB);
		this->appendCollider(resource, c1);
	}
}

void Pi3Cmodel::transformVerts(Pi3Cresource* resource, Pi3Cmatrix& matrix)
{
	Pi3Cmesh* mesh = getMesh(resource);
	if (!mesh) return;
	vertsPtr vp = getMeshVerts(resource);
	mesh->transform(*vp.verts, vp.offset, matrix);
	resource->updateMesh(meshRef);
}

void Pi3Cmodel::refreshMesh(Pi3Cresource* resource)
{
	//Update mesh vertices and bounds, then upload to GPU
	vertsPtr vp = getMeshVerts(resource);
	Pi3Cmesh* mesh = getMesh(resource);
	if (!mesh) return;
	mesh->updateBounds(&vp);
	bbox = mesh->bbox;
	resource->updateMesh(meshRef);
}

void Pi3Cmodel::create(Pi3Cresource *resource, Pi3Cmesh *mesh, int32_t groupId, uint32_t diffuseColour)
{	
	meshRef = resource->addMesh(mesh, groupId);
	if (mesh->mode==GL_TRIANGLES && mesh->lineIndexes.size()==0) resource->addMeshOutlines(meshRef); //This is really SLOW on big models!
	material = *resource->defaultMaterial();
	material.SetColDiffuse(diffuseColour);
	material.rendermode = mesh->mode;
	bbox = resource->meshes[meshRef].bbox;
}

void Pi3Cmodel::init()
{
	visible = true;
	deleted = false;
	materialRef = -1;
	meshRef = -1;
	touchable = true;
	selected = false;
	lodFrom = 0;
	lodToo = 0;
	choice = -1;
}

void Pi3Cmodel::deleteTexture(Pi3Cresource *resource)
{
	if (material.texRef>=0) resource->textures[material.texRef]->Delete();
}

void Pi3Cmodel::renderBasic(Pi3Cresource *resource, Pi3Cshader * shader, const Pi3Cmatrix *parent_matrix, Pi3Cmaterial *materialOverride)
{
	if (shader == nullptr) shader = &resource->shaders[0];

	if (meshRef >= 0) {
		Pi3Cmaterial *selectedMaterial = (materialOverride) ? materialOverride : &material;
		shader->SetModelMatrix(matrix);
		shader->setMaterial(selectedMaterial);
		resource->renderMesh(meshRef, selectedMaterial->rendermode);
	}
}

void Pi3Cmodel::render(Pi3Cresource *resource, Pi3Cshader &shader, const Pi3Cmatrix *parent_matrix, Pi3Cmaterial *materialOverride, uint32_t flags)
{
	if (!visible || deleted) return;
	
	Pi3Cmatrix newmatrix = (parent_matrix) ? matrix * *parent_matrix : matrix;

	//check if model in LOD range (if not, return) ...
	if (lodToo > 0) {
		const float eyeDistance = newmatrix.position().length();
		if (eyeDistance < lodFrom || eyeDistance >= lodToo) return;
	}

	//render the group (or chosen group model if choice set) ...
	if (choice >= 0 && choice < (int32_t)group.size()) {
		group[choice].render(resource, shader, &newmatrix, materialOverride);
	}
	else {
		if (animated) {
			if (group.size() > 0) {
				group[(int)(frame/2) % group.size()].render(resource, shader, &newmatrix, materialOverride, flags);
				frame++;
			}
		}
		else {
			for (auto& sg : group) sg.render(resource, shader, &newmatrix, materialOverride, flags);
		}
	}

	//finally, render the mesh itself (not usually set if there's a group with the model) ...
	if (meshRef >= 0) {

		Pi3Cmaterial *selectedMaterial = (materialOverride) ? materialOverride : &material;
		bool opaque = (selectedMaterial->alpha == 1.f);
		if (((flags & mdf_alpha) && !opaque) || ((flags & mdf_solid) && opaque)) {
			if (materialOverride) selectedMaterial->SetColDiffuse(material.colDiffuse);
			shader.SetModelMatrix(newmatrix);
			shader.setMaterial(selectedMaterial);
			resource->renderMesh(meshRef, selectedMaterial->rendermode);
		}
	}
}

void Pi3Cmodel::setMesh(Pi3Cresource* resource, Pi3Cmesh mesh, int32_t groupId)
{
	int32_t i = resource->addMesh(&mesh, groupId);
	meshRef = i;
	bbox = resource->meshes[i].bbox;
}

void Pi3Cmodel::appendMesh(Pi3Cresource *resource, Pi3Cmesh mesh, int32_t groupId)
{
	int32_t i = resource->addMesh(&mesh, groupId);
	//Appends first mesh into model itself (if none exists at start)...
	if (group.size() == 0 && meshRef<0) {
		meshRef = i;
		bbox = resource->meshes[i].bbox;
		return;
	}
	//Else append further meshes to group ...
	Pi3Cmodel newGroupMesh;
	newGroupMesh.meshRef = i;
	bbox.update(resource->meshes[i].bbox);
	group.push_back(newGroupMesh);
}

void Pi3Cmodel::appendMeshToGroup(Pi3Cresource* resource, Pi3Cmesh mesh, int32_t groupId, uint32_t diffuseColour, bool deleteVerts)
{
	int32_t meshRef = resource->addMesh(&mesh, groupId, 0, false);
	if (mesh.mode == GL_TRIANGLES && mesh.lineIndexes.size() == 0) resource->addMeshOutlines(meshRef); //This is really SLOW on big models!
	Pi3Cmodel newGroupMesh;
	newGroupMesh.material = *resource->defaultMaterial();
	newGroupMesh.material.SetColDiffuse(diffuseColour);
	newGroupMesh.meshRef = meshRef;
	bbox.update(resource->meshes[meshRef].bbox);
	group.push_back(newGroupMesh);
}

//void Pi3Cmodel::updateSelBox(Pi3Cresource *resource)
//{
//	if (meshSel < 0) return;
//	vertsPtr vp = resource->getMeshVerts(meshSel);
//	Pi3Cgizmos::select_box_verts(*vp.verts, vp.ptr, bbox.min, bbox.size(), 0xffffff);
//	resource->updateMesh(meshSel);
//}

void Pi3Cmodel::updateMesh(Pi3Cresource* resource, Pi3Cmesh &umesh)
{
	vertsPtr vp = resource->getMeshVerts(meshRef);

	for (uint32_t i = 0; i < umesh.verts.size(); i++) {
		(*vp.verts)[vp.offset++] = umesh.verts[i];
	}
	umesh.verts.clear(); //discard update mesh vertices as they've been uploaded to the buffer

	Pi3Cmesh& mesh = resource->meshes[meshRef];
	mesh.vertSize = umesh.vc / umesh.stride;
	mesh.vc = umesh.vc;
	mesh.bbox = umesh.bbox;
	mesh.lineIndexes = umesh.lineIndexes;
	resource->updateMesh(meshRef);
}

int32_t Pi3Cmodel::addTexture(Pi3Cresource *resource, const std::string &txfile, bool smooth)
{
	material.texRef = resource->loadTexture("", txfile, smooth);
	if (material.texRef >= 0) {
		material.texID = resource->getTextureID(material.texRef);
		material.texName = txfile;
		resource->materials.push_back(material);
	}
	return material.texID;
}

int32_t Pi3Cmodel::addTexture(Pi3Cresource *resource, const std::shared_ptr<Pi3Ctexture> &texture)
{
	material.texRef = resource->addTexture(texture); //resource->loadTexture("", txfile, material.texRef);
	if (material.texRef >= 0) {
		Pi3Ctexture *tx = texture.get();
		material.texID = resource->getTextureID(material.texRef);
		material.texWidth = tx->GetWidth();
		material.texHeight = tx->GetHeight();
		resource->materials.push_back(material);
	}
	return material.texID;
}

int32_t Pi3Cmodel::assignTexture(Pi3Cresource* resource, int texRef)
{
	material.texRef = texRef; //resource->loadTexture("", txfile, material.texRef);
	if (material.texRef >= 0) {
		Pi3Ctexture* tx = resource->textures[texRef].get();
		material.texID = resource->getTextureID(material.texRef);
		material.texWidth = tx->GetWidth();
		material.texHeight = tx->GetHeight();
	}
	return material.texID;
}

int32_t Pi3Cmodel::addPicture(Pi3Cresource *resource, const std::shared_ptr<Pi3Ctexture> &texture)
{
	material.SetColDiffuse(0xffffff);
	material.alpha = 0.99f;
	material.illum = 1; //non shaded illuminat
	return addTexture(resource, texture);
}

Pi3Cmodel * Pi3Cmodel::append(Pi3Cresource *resource, Pi3Cmodel model, vec3f offset, vec3f rotation)
{
	if (!(offset.isZero() && rotation.isZero())) {
		Pi3Cmatrix mm;
		mm.setMoveRotate(offset, rotation);
		model.matrix = model.matrix * mm;
		model.rotation = rotation;
	}
	group.push_back(model);
	bbox.update(model.bbox, &model.matrix);
	//updateSelBox(resource);
	return &group.back();
}

Pi3Cmodel * Pi3Cmodel::append(Pi3Cresource *resource, const Pi3Cmodel& model)
{
	if (model.meshRef < 0 && model.group.size() == 0) return nullptr;
	//model.matrix = matrix;
	group.push_back(model);
	bbox.update(model.bbox, &model.matrix);
	//updateSelBox(resource);
	return &group.back();
}

Pi3Cmodel * Pi3Cmodel::appendCollider(Pi3Cresource *resource, Pi3Cmodel &model, vec3f offset, vec3f rotation)
{
	model.matrix.setMoveRotate(offset, rotation);
	colliders.push_back(model);
	bbox.update(model.bbox, &model.matrix);
	//updateSelBox(resource);
	return &colliders.back();
}

Pi3Cmodel * Pi3Cmodel::appendLOD(Pi3Cresource *resource, Pi3Cmodel model, float LODfrom, float LODtoo, vec3f offset, vec3f rotation)
{
	model.lodFrom = LODfrom;
	model.lodToo = LODtoo;
	model.matrix.setMoveRotate(offset, rotation);
	group.push_back(model);
	bbox.update(model.bbox, &model.matrix);
	//updateSelBox(resource);
	return &group.back();
}


void Pi3Cmodel::loadOBJfile(Pi3Cresource* resource, std::string path, std::string modelfile, int32_t groupId, std::function<void(float)> showProgressCB, bool asCollider, bool addColliderGrid)
{
	std::string error;
	size_t meshStart = resource->meshes.size();
	Pi3CfileOBJ::load(path, modelfile, resource, showProgressCB, asCollider, addColliderGrid, groupId, error);
	size_t meshEnd = resource->meshes.size();
	//collider = asCollider;
	addMeshes(resource, modelfile, meshStart, meshEnd);
}

void Pi3Cmodel::addMeshes(Pi3Cresource* resource, const std::string& modelname, const size_t meshStart, const size_t meshEnd)
{
	if (meshEnd-meshStart > 0) {
		name = modelname;
		for (size_t i=meshStart; i<meshEnd; i++) {
			Pi3Cmodel newGroupModel(resource, i);
			group.push_back(newGroupModel);
			bbox.update(newGroupModel.bbox);
		}
	}
}

void Pi3Cmodel::addColliderGrid(Pi3Cresource *resource)
{
	if (meshRef >= 0) {
		Pi3Cmesh& mesh = resource->meshes[meshRef];
		mesh.hasColliderGrid = mesh.createColliderGrid();
	}
	else {
		for (auto &g : group) {
			g.addColliderGrid(resource);
		}
	}
}
/*
bool Pi3Cmodel::collide(const Pi3Cresource *resource, const Pi3Cmatrix &pmat, const vec3f &pos, const vec3f &dir, const float radius) const
{
	Pi3Cmatrix tmat = matrix * pmat;

	for (auto &sg : group) {
		if (sg.collide(resource, tmat, pos, dir, radius)) return true;
	}

	if (colliders.size() == 0) return false;

	for (auto &collider : colliders) {
		if (collider.collideSub(resource, tmat, pos, dir, radius))
			return true;
	}

	return false;
}
*/

bool Pi3Cmodel::collide(const Pi3Cresource *resource, const Pi3Cmatrix *parent_matrix, const vec3f &pos, const vec3f &dir, const float radius) const
{
	if (!visible || deleted) return false;

	Pi3Cmatrix newmatrix = (parent_matrix) ? matrix * *parent_matrix : matrix;

	//check if model in LOD range (if not, return) ...
	if (lodToo > 0) {
		const float eyeDistance = newmatrix.position().length();
		if (eyeDistance < lodFrom || eyeDistance >= lodToo) return false;
	}

	//render the group (or chosen group model if choice set) ...
	if (choice >= 0 && choice < (int32_t)group.size()) {
		if (group[choice].collide(resource, &newmatrix, pos, dir, radius)) return true;
	}
	else {
		if (colliders.size() > 0) {
			for (auto& collider : colliders) {
				if (collider.collideSub(resource, newmatrix, pos, dir, radius))
					return true;
			}
		}
		else {
			for (auto& sg : group) {
				//if (sg.colliders.size() > 0) {
				//	for (auto& collider : sg.colliders) {
				//		if (collider.collideSub(resource, newmatrix, pos, dir, radius))
				//			return true;
				//	}
				//}
				//else
					if (sg.visible && !sg.deleted && sg.collide(resource, &newmatrix, pos, dir, radius)) return true;
			}
		}
	}

	return false;
}

bool Pi3Cmodel::collideSub(const Pi3Cresource *resource, const Pi3Cmatrix &pmat, const vec3f &pos, const vec3f &dir, const float radius) const
{
	Pi3Cmatrix tmat = matrix * pmat;

	for (auto &sg:group) {
		if (sg.collideSub(resource, tmat, pos, dir, radius)) return true;
	}

	if (meshRef >= 0) {
		return Pi3Ccollision::collideVector(resource, meshRef, tmat, false, pos, dir);
	}
		
	return false;
}

float Pi3Cmodel::collideFloor(const Pi3Cresource *resource, const Pi3Cmatrix *parent_matrix, const vec3f &pos, float &prevHeight, const bool onehit) const
{
	float ht = prevHeight;

	Pi3Cmatrix newmatrix = (parent_matrix) ? matrix * *parent_matrix : matrix;

	//check if model in LOD range (if not, return) ...
	if (lodToo > 0) {
		const float eyeDistance = newmatrix.position().length();
		if (eyeDistance < lodFrom || eyeDistance >= lodToo) return ht;
	}

	//render the group (or chosen group model if choice set) ...
	if (choice >= 0 && choice < (int32_t)group.size()) {
		if (group[choice].collideFloor(resource, &newmatrix, pos, ht)) return ht;
	}
	else {
		if (colliders.size() > 0) {
			for (auto& collider : colliders) {
				ht = collider.collideFloorSub(resource, newmatrix, pos, prevHeight, onehit);
			}
		}
		else if (asCollider) {
			ht = collideFloorSub(resource, newmatrix, pos, prevHeight, onehit);
		}
		else {
			for (auto& sg : group) {
				//if (sg.colliders.size() > 0) {
				//	for (auto &collider : sg.colliders) {
				//		ht = collider.collideFloorSub(resource, newmatrix, pos, prevHeight, onehit);
				//	}
				//}
				//else
				//{
				//	if (asCollider) {
				//		ht = collideFloorSub(resource, newmatrix, pos, prevHeight, onehit);
				//	}
				//	else
				if (sg.visible && !sg.deleted) ht = sg.collideFloor(resource, &newmatrix, pos, ht);
			}
		}
	}

	//Pi3Cmatrix tmat = matrix * pmat;

	//for (auto &sg : group) {
	//	ht = (sg.collideFloor(resource, tmat, pos, ht));
	//}

	//if (colliders.size() == 0) return ht;

	////if (!bbox.pointInsideXZtx(pos, tmat)) return ht;

	//for (auto &collider : colliders) {
	//	ht = (collider.collideFloorSub(resource, tmat, pos, prevHeight));
	//}

	return ht;
}

float Pi3Cmodel::collideFloorSub(const Pi3Cresource *resource, const Pi3Cmatrix &pmat, const vec3f &pos, float &prevHeight, const bool onehit) const
{
	//
	if (group.size() > 0) {
		Pi3Cmatrix tmat = matrix * pmat;
		for (auto &sg : group) {
			float ht = sg.collideFloorSub(resource, tmat, pos, prevHeight, onehit);
			if (ht < 1e8f) return ht;
		}
	}
 
	if (meshRef >= 0) 
		return Pi3Ccollision::collideFloor(resource, meshRef, pmat, -pos, prevHeight, onehit);
		
	return 1e8f;
}

void Pi3Cmodel::touch(const Pi3Cresource *resource, const Pi3Cmatrix *parent_matrix, Pi3Ctouch &touch, const int32_t level)
{
	//reflects how render works ...

	if (!touchable || !visible || deleted) return;

	Pi3Cmatrix tmat = (parent_matrix) ? matrix * *parent_matrix : matrix;

	if (lodToo > 0) {
		float eyeDistance = tmat.position().length();
		if (eyeDistance < lodFrom || eyeDistance >= lodToo) return;
	}

	//touch the group (or chosen group model if choice set) ...
	float dist = touch.prevDist;
	if (choice >= 0 && choice < (int32_t)group.size()) {
		group[choice].touch(resource, &tmat, touch, level + 1);
		if (touch.prevDist < dist) { touch.groupRefs[level] = choice; dist = touch.prevDist; }
	}
	else {
		for (size_t i = 0; i < group.size(); i++) {
			group[i].touch(resource, &tmat, touch, level + 1);
			if (touch.prevDist < dist) {
				touch.groupRefs[level] = (int32_t)i;
				dist = touch.prevDist;
				//SDL_Log("Level = %d, item = %d, %f", level, i, dist);
			}
		}
	}

	if (meshRef >= 0) {
		const int32_t triRef = resource->touchMesh(meshRef, touch, tmat);
		if (triRef >= 0) {
			touch.triRef = triRef;			//keep triangle ref
			touch.meshRef = meshRef;		//keep mesh ref
			touch.selmodel = this;			//keep ptr to this model
			//touch.selgroup = parentModel;	//keep ptr to parent model
			touch.touching = true;
			touch.maxlevel = level-1;
		}
	}
}

bool Pi3Cmodel::touchRect(Pi3Cresource* resource, const Pi3Cmatrix* parent_matrix, const Pi3Cmatrix& projMatrix, const Pi3Crect& rect)
{
	if (!touchable || !visible || deleted) return false;

	Pi3Cmatrix tmat = (parent_matrix) ? matrix * *parent_matrix : matrix;

	if (lodToo > 0) {
		float eyeDistance = tmat.position().length();
		if (eyeDistance < lodFrom || eyeDistance >= lodToo) return false;
	}

	if (meshRef >= 0) {
		Pi3Cmesh& mesh = resource->meshes[meshRef];

		std::vector<uint32_t> pointIndexes;
		std::vector<uint32_t> edgeIndexes;
		std::vector<uint32_t> faceIndexes;
		Pi3Cbbox2d bbox;
		bbox.update(vec2f(rect.x, rect.y));
		bbox.update(vec2f(rect.x + rect.width, rect.y + rect.height));
		Pi3Cmatrix pmat =  tmat * projMatrix;
		mesh.insideRect(bbox, pointIndexes, edgeIndexes, faceIndexes, false, pmat, *(resource->getMeshBuffer(meshRef)));
		return pointIndexes.size() > 0;
	}

	if (choice >= 0 && choice < (int32_t)group.size()) {
		if (group[choice].touchRect(resource, &tmat, projMatrix, rect)) return true;
	}
	else {
		for (size_t i = 0; i < group.size(); i++) {
			if (group[i].touchRect(resource, &tmat, projMatrix, rect)) return true;
		}
	}

	return false;
}

Pi3Cmodel* Pi3Cmodel::find(const std::string &name)
{
	if (name == this->name) return this;
	for (auto &m : group) {
		Pi3Cmodel* found = m.find(name);
		if (found != nullptr) return found;
	}
	return nullptr;
}

void Pi3Cmodel::createRect2D(Pi3Cresource *resource, const vec2f &pos, const vec2f &size, const uint32_t colour, const int32_t texRef)
{
	if (resource->rectRef >= 0) {
		meshRef = resource->rectRef;
		matrix.moveXY(pos);
		matrix.SetScales(vec3f(size.x, size.y, 1.f));
		material = *resource->defaultMaterial();  //The default material;
		material.SetColDiffuse(colour);
		material.illum = 1;					//non shaded illumination
		if (texRef>=0) 
			material.texID = assignTexture(resource, texRef);
	}
}

void Pi3Cmodel::textModel(Pi3Cresource *resource, Pi3Cfont *font, const std::string &text, const float wrapWidth, const Pi3Cfont::textReaderType textType)
{
	meshRef = resource->letterSheetRef;
	Pi3Cmesh& mesh = resource->meshes[meshRef];
	std::vector<float> *verts = resource->getMeshBuffer(meshRef);				//get ptr to vertices from resource letterVerts
	if (verts == nullptr) return;
	Pi3Crect size;
	uint32_t vertCount = font->textureRects(text, *verts, size, wrapWidth, mesh.stride, textType);	//generate verts from text	
	resource->updateMesh(meshRef, vertCount);								//upload updated verts to GPU
	bbox.update(vec3f(size.x, size.y-size.height, 0));
	bbox.update(vec3f(size.width, 0, 0));
	//updateSelBox(resource);
	matrix.move(vec3f(0, -size.height, 0));
	material = font->fontsheet.sheetMaterial;
}

#define updateCoordsXYZ(x,y,z)											\
	(*vp.verts)[vp.offset] = x; (*vp.verts)[vp.offset+1] = y; (*vp.verts)[vp.offset+2] = z; vp.offset+=stride;	\

void Pi3Cmodel::updateSpriteBillboard(Pi3Cresource *resource, const std::vector<vec3f> &pos, const std::vector<vec2f> &size, const vec3f &lookat)
{
	//Transform pos into screen space ...
	Pi3Cmesh &mesh = resource->meshes[meshRef];
	uint32_t stride = mesh.stride;
	vertsPtr vp = resource->getMeshVerts(meshRef);

	for (size_t i = 0; i < pos.size(); i++) {
		float hw = size[i].x * 0.5f;
		//float hh = size.y * 0.5f;

		//const vec3f &pt = pos[i];
		float x = pos[i].x, y = pos[i].y, z = pos[i].z, sy = size[i].y;
		float lx = lookat.x - x;
		float lz = lookat.z - z;
		float d = hw / sqrtf(lx*lx + lz * lz);
		float ax = lz * d, az = -lx * d;
		
		updateCoordsXYZ(x - ax, y, z - az); // , uv.x, uv.y);
		updateCoordsXYZ(x + ax, y + sy, z + az); // , uv.x + us.x, uv.y);
		updateCoordsXYZ(x + ax, y, z + az); // , uv.x + us.x, uv.y + us.y);
		updateCoordsXYZ(x - ax, y, z - az); // , uv.x, uv.y);
		updateCoordsXYZ(x - ax, y + sy, z - az); // , uv.x, uv.y + us.y);
		updateCoordsXYZ(x + ax, y + sy, z + az); // , uv.x + us.x, uv.y + us.y);

	}
	resource->updateMesh(meshRef);
}

void Pi3Cmodel::updateRect2D(Pi3Cresource* resource, const vec2f& pos, const vec2f& size)
{
	updateQuad(resource, vec3f(pos.x, pos.y, -20.f), vec3f(pos.x + size.x, pos.y, -20.f), vec3f(pos.x + size.x, pos.y + size.y, -20.f), vec3f(pos.x, pos.y + size.y, -20.f));
}

void Pi3Cmodel::updateQuad(Pi3Cresource* resource, const vec3f& p1, const vec3f& p2, const vec3f& p3, const vec3f& p4)
{
	Pi3Cmesh& mesh = resource->meshes[meshRef];
	vertsPtr vp = resource->getMeshVerts(meshRef);
	updateSpriteVerts(vp, mesh.stride, p1, p2, p3, p4);
	resource->updateMesh(meshRef);
}

void Pi3Cmodel::updateLineQuad(Pi3Cresource *resource, const std::vector<vec3f> &pos, const std::vector<vec3f> &dir, const vec2f &size)
{
	//Transform pos into screen space ...
	Pi3Cmesh &mesh = resource->meshes[meshRef];
	uint32_t stride = mesh.stride;
	vertsPtr vp = resource->getMeshVerts(meshRef);

	for (size_t i = 0; i < pos.size(); i++) {
		//float hw = size[i].x * 0.5f;
		//float hh = size.y * 0.5f;

		vec3f p1 = pos[i];
		vec3f p2 = pos[i] + dir[i] * size.y;
		vec3f wa = vec3f(dir[i].z*size.x, 0, -dir[i].x*size.x);
		vec3f a = p1 + wa;
		vec3f b = p1 - wa;
		vec3f c = p2 - wa;
		vec3f d = p2 + wa;

		updateCoordsXYZ(a.x, a.y, a.z); // , uv.x, uv.y);
		updateCoordsXYZ(b.x, b.y, b.z); // , uv.x + us.x, uv.y);
		updateCoordsXYZ(d.x, d.y, d.z); // , uv.x + us.x, uv.y + us.y);
		updateCoordsXYZ(b.x, b.y, b.z); // , uv.x, uv.y);
		updateCoordsXYZ(c.x, c.y, c.z); // , uv.x, uv.y + us.y);
		updateCoordsXYZ(d.x, d.y, d.z); // , uv.x + us.x, uv.y + us.y);

	}
	resource->updateMesh(meshRef);
}

#define updateCoordsXY(x,y)							\
		(*vp.verts)[vp.offset] = x; (*vp.verts)[vp.offset + 1] = y; vp.offset+=stride; 	\


void Pi3Cmodel::updateSpriteVerts(vertsPtr &vp, const uint32_t stride, const vec3f& p1, const vec3f& p2, const vec3f& p3, const vec3f& p4)
{
	updateCoordsXYZ(p1.x, p1.y, p1.z);
	updateCoordsXYZ(p2.x, p2.y, p2.z);
	updateCoordsXYZ(p3.x, p3.y, p3.z);
	updateCoordsXYZ(p1.x, p1.y, p1.z);
	updateCoordsXYZ(p3.x, p3.y, p3.z);
	updateCoordsXYZ(p4.x, p4.y, p4.z);
}

void Pi3Cmodel::updateSpriteCoordsRotated(Pi3Cresource *resource, const std::vector<vec3f> &pos, const std::vector<vec2f> &size, const std::vector<float> &angle)
{

	Pi3Cmesh &mesh = resource->meshes[meshRef];
	uint32_t stride = mesh.stride;
	vertsPtr vp = resource->getMeshVerts(meshRef);

	for (size_t i = 0; i < pos.size(); i++) {

		float hw = size[i].x;// *0.5f;
		float hh = size[i].y;// *0.5f;
		float x = pos[i].x, y = pos[i].y;

		float ca = cos(angle[i]);
		float sa = sin(angle[i]);
		float ax = hw * ca - hh * sa;
		float ay = hw * sa + hh * ca;

		updateCoordsXY(x - ax, y + ay);
		updateCoordsXY(x - ay, y - ax);
		updateCoordsXY(x + ax, y - ay);
		updateCoordsXY(x - ax, y + ay);
		updateCoordsXY(x + ax, y - ay);
		updateCoordsXY(x + ay, y + ax);
	}
	resource->updateMesh(meshRef);
}
