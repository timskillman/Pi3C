#include "Pi3Cresource.h"

void Pi3Cresource::init(const uint32_t stride)
{
	//this->stride = stride;
	currentBuffer = -1;
	lastVBO = -1;
	//startVBO = 0;
	createDefaultMaterial();

	//Create a dummy mesh to reserve GPU memory dynamic creation of letters ...
	Pi3Cmesh lets;
	lets.vc = 1000 * 6 * stride;
	lets.verts.resize(lets.vc); // 10000 letters - reserve MUST be multiple of consistent stride (since glDrawArrays requires a vertex index not a byte index)
	letterSheetRef = addMesh(&lets,-1); //create a dynamic buffer for the lettersheet (page size)

	//Reserve for small text entry fields ...
	lets.vc = 1000 * 6 * stride;
	lets.verts.resize(lets.vc);
	lettersRef = addMesh(&lets,-1); //create a dynamic buffer for small temp lettersheet

	//Create a default 2D rectangle as a helper for 2D GUI rendering 
	Pi3Cmesh rect = createRect(vec3f(0, 0, -10.f), vec2f(1.f, 1.f), 0xffffffff, vec2f(0,0), vec2f(1.f,1.f));
	rectRef = addMesh(&rect,-1); //buffer used for creating models etc..

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
		SDL_Log("couldn't initialize mix, error: %s\n", Mix_GetError());
	}
}

Pi3Cmesh Pi3Cresource::createRect(const vec3f& pos, const vec2f& size, const uint32_t col, const vec2f& uvpos, const vec2f& uvsize)
{
	Pi3Cmesh rect("rect");
	rect.stride = 9;

	vec3f n(0, -1.f, 0);

	rect.addPackedVert(pos + vec3f(size.x, size.y, 0), n, uvpos + uvsize, col);
	rect.addPackedVert(pos + vec3f(0, size.y, 0), n, vec2f(uvpos.x, uvpos.y + uvsize.y), col);
	rect.addPackedVert(pos + vec3f(0, 0, 0), n, uvpos, col);

	rect.addPackedVert(pos + vec3f(size.x, 0, 0), n, vec2f(uvpos.x + uvsize.x, uvpos.y), col);
	rect.addPackedVert(pos + vec3f(size.x, size.y, 0), n, uvpos + uvsize, col);
	rect.addPackedVert(pos + vec3f(0, 0, 0), n, uvpos, col);

	rect.bbox.bboxFromVerts(rect.verts, 0, rect.vc, rect.stride);
	rect.materialRef = 0;
	return rect;
}

void Pi3Cresource::createDefaultMaterial(const std::string &name)
{
	Pi3Cmaterial defmaterial;
	defmaterial.texID = createDefaultTexture(defmaterial.texRef);
	defmaterial.name = name;
	defmaterial.texName = name;
	defmaterial.texWidth = 1;
	defmaterial.texHeight = 1;
	defmaterial.groupID = -1; //system type group
	materials.push_back(defmaterial);
}

int32_t Pi3Cresource::addMaterial(const Pi3Cmaterial& material)
{
	materials.push_back(material);
	return materials.size() - 1;
}

void Pi3Cresource::deleteMaterialTexturesByID(int32_t groupId)
{
	for (auto& m: materials)
	{
		if (m.groupID == groupId) {
			if (m.texRef >= 0 && m.texRef!= materials[0].texRef && textures[m.texRef]!=nullptr) {
				textures[m.texRef]->Delete();
				textures[m.texRef].reset();
				m.texRef = -1;
			}
		}
	}
	//cleanTextures();
}

void Pi3Cresource::cleanTextures()
{
	for (auto& t: textures) {
		if (t->isValid()==false) t.reset();
	}
}

void Pi3Cresource::deleteMaterialsByID(int32_t groupId)
{
	size_t msz = materials.size();
	for (size_t i = msz - 1; i > 0; i--)
	{
		Pi3Cmaterial& m = materials[i];
		if (m.groupID == groupId) {
			materials.erase(materials.begin() + i);
		}
	}
}

int32_t Pi3Cresource::createDefaultTexture(int32_t &texRef)
{

	/* Creates a 1 pixel white texture
	Can be recoloured by material colDiffuse */
	std::shared_ptr<Pi3Ctexture> Texture;
	Texture.reset(new Pi3Ctexture());
	Texture->createColour(0xffffffff);
	Texture->upload(true);

	//keep texture - don't destroy it!
	texRef = textures.size();
	textures.push_back(Texture); 

	return Texture->textureID; //textures.size();
}

void Pi3Cresource::updateMesh(const uint32_t meshRef, uint32_t vertCount, const uint32_t vertOffset)
{
	if (meshRef < 0) return; //|| (vertCount / mesh->stride) > vertBuffer[mesh->bufRef].size()
	Pi3Cmesh &mesh = meshes[meshRef];
	if (vertCount > 0) {
		mesh.vc = vertCount;
		mesh.vertSize = vertCount / mesh.stride;
		mesh.vertOffset = vertOffset;
	}
	updateGPUverts(mesh.bufRef, mesh.vertOffset*mesh.stride, mesh.vc, vertBuffer[mesh.bufRef].verts);
}

int32_t Pi3Cresource::getTextureID(int32_t texRef)
{
	return textures[texRef]->textureID;
}

int32_t Pi3Cresource::findTextureByName(const std::string& name)
{
	for (size_t i = 0; i < textures.size(); i++) {
		if (name == textures[i]->name) return i;
	}
	return -1;
}

int32_t Pi3Cresource::addTexture(const std::shared_ptr<Pi3Ctexture> &Texture, bool smooth)
{
	if (Texture.get() != nullptr) {
		Texture->upload(smooth);
		if (Texture->name == "") Texture->name = "Pi3Tex" + std::to_string(textures.size());
		textures.push_back(Texture);	//keep texture - don't destroy it
		int texref = textures.size() - 1;
		//Pi3Cmaterial mat = *defaultMaterial();
		//mat.texRef = texref;
		//mat.texName = Texture->name;
		//mat.name = Texture->name;
		//mat.texWidth = Texture->GetWidth();
		//mat.texHeight = Texture->GetHeight();
		//addMaterial(mat);
		return texref;		//return texture reference in resource textures array
	}
	return -1;
}

int32_t Pi3Cresource::loadTexture(const std::string &path, const std::string &file, bool smooth) 
{
	if (file == "") return -1;
	for (size_t i = 0; i < materials.size(); i++) {
		if (file == materials[i].texName) {
			return materials[i].texRef;
		}
	}

	std::shared_ptr<Pi3Ctexture> Texture;
	Texture.reset(new Pi3Ctexture());

	std::string filename = path + file;
	Texture->loadFromFile(filename.c_str());
	if (Texture.get()==nullptr){
		SDL_Log("ERROR! texture '%s' not found!!", filename.c_str());
		return -1;
	}

	return addTexture(Texture, smooth);
}

void Pi3Cresource::addMeshOutlines(uint32_t meshref)
{
	Pi3Cmesh &mesh = meshes[meshref];
	mesh.createTriangleEdges(vertBuffer[mesh.bufRef].verts);
}

int32_t Pi3Cresource::addMesh(Pi3Cmesh * mesh, int32_t groupId, uint32_t maxsize, bool deleteVerts)
{
	if (maxsize == 0) maxsize = 500000;
	//Are we requesting too much?  Then attempt it ...
	uint32_t mvsize = (mesh) ? mesh->verts.size() : 0;
	if (mvsize > maxsize) maxsize = mvsize; // return -1;

	// check if we can squeeze this mesh into a buffer with space ...
	int32_t cbuf = currentBuffer;
	if (cbuf > 0 && mvsize > 0) {
		for (size_t b = 0; b < vertBuffer.size(); b++) {
			uint32_t spareBuffer = vertBuffer[b].free();
			if (spareBuffer > mvsize && groupId==vertBuffer[b].groupId) {
				cbuf = b; break;
			}
		}
	}

	//No buffers created yet or, request is larger than current buffer?, then create new buffer ...
	bool vertsMoved = false;
	if (cbuf < 0 || groupId != vertBuffer[cbuf].groupId || (mvsize + vertBuffer[cbuf].freePtr) > vertBuffer[cbuf].verts.size()) {
		if (cbuf>=0) SDL_Log("cbuf:%d, mvsize:%d + vertBufferPtr:%d, size=%d, maxsize=%d, groupId=%d", cbuf, mvsize , vertBuffer[cbuf].freePtr, vertBuffer[cbuf].verts.size(), maxsize, groupId);
		mesh->verts.resize(maxsize);
		vertBuffer.emplace_back();
		Pi3CvertsBuffer& vb = vertBuffer.back();
		vb.verts = std::move(mesh->verts); // mesh->verts;
		vb.freePtr = 0;
		vb.groupId = groupId;
		//vb.verts.push_back(std::move(mesh->verts));
		vertsMoved = true;
		//vertBufferPtr.push_back(0);
		currentBuffer++; cbuf = currentBuffer;

		//Create and reserve buffer in GPU ...
		glGenBuffers(1, &VBOid[currentBuffer]);
		SDL_Log("VBO:%d, Uploading floats = %d, mvsize=%d, remaining:%d", VBOid[currentBuffer], maxsize, mvsize, VBOid[currentBuffer]-(vertBuffer[currentBuffer].verts.front()+mvsize));
		glBindBuffer(GL_ARRAY_BUFFER, VBOid[currentBuffer]);
		glBufferData(GL_ARRAY_BUFFER, maxsize * sizeof(float), &vertBuffer[currentBuffer].verts.front(), GL_DYNAMIC_DRAW);
		vertBuffer[currentBuffer].bufferRef = VBOid[currentBuffer];
	}

	if (mesh) {
		std::vector<float> &mverts = vertBuffer[cbuf].verts;		//Get current buffer
		uint32_t &bufferPtr = vertBuffer[cbuf].freePtr;

		//Upload mesh verts into GPU ...
		if (mvsize > 0) {
			uint32_t vcbytes = mesh->vc * sizeof(float);	
			if (!vertsMoved) {
				memcpy(&mverts[bufferPtr], &mesh->verts[0], vcbytes);
				SDL_Log("VBO:%d, Uploading floats = %d, mvsize=%d, remaining:%d", VBOid[currentBuffer], maxsize, mvsize, maxsize - (bufferPtr + mvsize));
				if (deleteVerts) mesh->verts.resize(0); //delete verts from original mesh as these have been stored and uploaded to GPU
				glBindBuffer(GL_ARRAY_BUFFER, VBOid[cbuf]);
				glBufferSubData(GL_ARRAY_BUFFER, bufferPtr * sizeof(float), vcbytes, &mverts[bufferPtr]); // This transfer requires vcbytes 
			}
		}
		
		//Push mesh into meshes store ...
		mesh->vertOffset = bufferPtr / mesh->stride;	//vertice offset index - used for rendering glDrawArrays
		mesh->vertSize = mesh->vc / mesh->stride;		//vertice size
		mesh->bufRef = cbuf;
		meshes.push_back(*mesh);

		bufferPtr += mesh->vc;

		return meshes.size() - 1;
	}

	return 0;
}

void Pi3Cresource::deleteVertsBuffer(int32_t groupId)
{
	//Delete all meshes that use a vertex buffer with specific groupId
	size_t msz = meshes.size();
	for (size_t i = msz - 1; i > 0; i--)
	{
		auto& vb = vertBuffer[meshes[i].bufRef];
		if (vb.groupId == groupId) {
			meshes.erase(meshes.begin() + i);
		}
	}

	//Now delete all vertBuffer data with specific groupId
	for (size_t b = 0; b < vertBuffer.size(); b++) {
		auto& vb = vertBuffer[b];
		if (vb.groupId == groupId) {
			glDeleteBuffers(1, &VBOid[b]);
			vb.verts.resize(0);
			vb.groupId = -2; //mark for deletion
			VBOid[b] = 0;
		}
	}

	//Delete vertBuffers marked for deletion
	msz = vertBuffer.size();
	for (size_t i = msz - 1; i > 0; i--)
	{
		auto& vb = vertBuffer[i];
		if (vb.groupId==-2) {
			vertBuffer.erase(vertBuffer.begin() + i);
			currentBuffer--;
		}
	}
}

void Pi3Cresource::setRenderBuffer(const int bufRef, const uint32_t stride) 
{
	// dont bother changing buffer if its the same as last time
	if (lastVBO == bufRef) return; 

	// Select new Vertex buffer ...
	glBindBuffer(GL_ARRAY_BUFFER, VBOid[bufRef]);

	// Reset attributes for newly selected buffer
	// (this should really be done by the current shader!)
	GLuint attribCount = 0; 
	GLuint pos = 0; 
	GLuint strideBytes = stride * sizeof(float);
	glVertexAttribPointer(attribCount, 3, GL_FLOAT, GL_FALSE, strideBytes, (void*)pos);  //verts
	glEnableVertexAttribArray(attribCount++); pos += 3 * sizeof(float);

	glVertexAttribPointer(attribCount, 3, GL_FLOAT, GL_FALSE, strideBytes, (void*)pos);	//normals
	glEnableVertexAttribArray(attribCount++); pos += 3 * sizeof(float);

	glVertexAttribPointer(attribCount, 2, GL_FLOAT, GL_FALSE, strideBytes, (void*)pos);	//uvs
	glEnableVertexAttribArray(attribCount++); pos += 2 * sizeof(float);

	glVertexAttribPointer(attribCount, 1, GL_FLOAT, GL_FALSE, strideBytes, (void*)pos);	//colour
	glEnableVertexAttribArray(attribCount++); pos += sizeof(float);

	lastVBO = bufRef;
}

Pi3Cpointi Pi3Cresource::getStringSize(const std::string& text, Pi3Cfont* font)
{
	int w, h;
	font->getStringSize(text, w, h);
	return Pi3Cpointi(w, h); 
}

Pi3Crect Pi3Cresource::renderText(const int meshRef, Pi3Cfont *font, const std::string &text, const vec3f &pos, const float wrapWidth, const uint32_t colour)
{
	//Text meshref's can only be used once per frame (otherwise using the same meshRef will simply overwrite the previous text)
	Pi3Crect size;
	Pi3Cmesh &mesh = meshes[meshRef];
	uint32_t vertCount = font->textureRects(text, vertBuffer[mesh.bufRef].verts, size, wrapWidth, mesh.stride, Pi3Cfont::RD_NONE);	//generate verts from text	
	updateMesh(meshRef, vertCount);											//upload updated verts to GPU

	Pi3Cmatrix matrix;
	matrix.move(pos);
	//matrix.SetScales(vec3f((float)wh.x, (float)wh.y, 1.f));
	shaders[0].SetModelMatrix(matrix);
	shaders[0].setMaterial(&font->fontsheet.sheetMaterial);
	shaders[0].setColAmbient(colour);
	//shaders[0].setColDiffuse(0xffffffff);

	renderMesh(meshRef, GL_TRIANGLES);
	return size;
}

void Pi3Cresource::renderMesh(const int meshRef, const GLenum rendermode)
{
	Pi3Cmesh &mesh = meshes[meshRef];
	if (mesh.vc == 0) return;

	setRenderBuffer(mesh.bufRef, mesh.stride);

	if (rendermode == GL_TRIANGLES || mesh.mode== GL_LINE_STRIP) mesh.render(rendermode);

	if (mesh.lineIndexes.size()>0 && (rendermode & GL_LINES))
		mesh.renderIndexed(GL_LINES, mesh.lineIndexes.size(), &mesh.lineIndexes[0]);

	calls++; //used for debugging
}

int32_t Pi3Cresource::touchMesh(const int meshRef, Pi3Ctouch &touch, const Pi3Cmatrix &tmat) const
{
	const Pi3Cmesh &mesh = meshes[meshRef];
	return mesh.touchPoint(touch, tmat, vertBuffer[mesh.bufRef].verts);
}

void Pi3Cresource::updateGPUverts(const int bufref, const int vfrom, const int vsize, const std::vector<float> &verts)
{
	//setRenderBuffer(bufref);
	glBindBuffer(GL_ARRAY_BUFFER, VBOid[bufref]);
	glBufferSubData(GL_ARRAY_BUFFER, vfrom * sizeof(float), vsize * sizeof(float), &verts[vfrom]);
}

int32_t Pi3Cresource::addShader(const std::string &vertfile, const std::string &fragfile)
{
	shaders.emplace_back();
	Pi3Cshader &shader = shaders.back();
	errorStr = shader.create(vertfile.c_str(), fragfile.c_str());
	if (errorStr != "") return -1;
	return shaders.size() - 1;
}

void Pi3Cresource::useBasicShader(const uint32_t shaderRef)
{
	Pi3Cshader &shader = shaders[shaderRef];
	shader.Use();
	shader.lightPos = vec3f(0, 0, 1000.f);
	shader.setFog(100.f, 5000.f, vec3f(1.f, 1.f, 1.f));
	shader.setupShader();
}

void Pi3Cresource::addFont(const char * path, const char * fontfile, int ptsize) {
	std::shared_ptr<Pi3Cfont> font;
	font.reset(new Pi3Cfont(path, fontfile, ptsize));
	if (font->font) fonts.insert({ fontfile, font });
}

std::shared_ptr<Pi3Cmusic> Pi3Cresource::addMusic(const std::string& path, const std::string& musicfile) {
	std::string musicpath = (path.size() > 0 && *(path.end() - 1) != '/') ? path + "/" + musicfile : musicfile;
	SDL_Log("Loading music %s", musicpath.c_str());
	std::shared_ptr<Pi3Cmusic> smusic;
	smusic.reset(new Pi3Cmusic(musicpath.c_str()));
	if (smusic->music) music.insert({ musicfile, smusic });
	return smusic;
}

std::shared_ptr<Pi3Csound> Pi3Cresource::addSound(const std::string& path, const std::string& soundfile) {
	std::string soundpath = (path.size() > 0 && *(path.end() - 1) != '/') ? path + "/" + soundfile : soundfile;
	std::shared_ptr<Pi3Csound> sound;
	sound.reset(new Pi3Csound(soundpath.c_str()));
	if (sound->sound) sounds.insert({ soundfile, sound });
	return sound;
}

void Pi3Cresource::clearAll()
{
	for (int32_t i = 0; i < currentBuffer; i++) {
		if (VBOid[i] > 0) glDeleteBuffers(1, &VBOid[i]);
	}

	meshes.clear();
	materials.clear();
	textures.clear();
	shaders.clear();
	vertBuffer.resize(0);
	//vertBufferPtr.resize(0);
	vertCount = 0;
	currentBuffer = -1;
	lastVBO = -1;
	//startVBO = 0;
}
