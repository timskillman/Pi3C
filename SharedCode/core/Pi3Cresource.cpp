#include "Pi3Cresource.h"

void Pi3Cresource::init(const uint32_t stride)
{
	this->stride = stride;
	currentBuffer = -1;
	lastVBO = -1;
	startVBO = 0;
	createDefaultMaterial();
	uploaded = false;

	//Create a default 2D rectangle as a helper for creating a letter sheet (verts will dynamically change)
	Pi3Cmesh lets = Pi3Cshapes::rect(vec2f(0, 0), vec2f(1.f, 1.f));
	lets.dynamic = true;
	letterSheetRef = addMesh(lets, true, true,100000); //create a dynamic buffer for the lettersheet (page size)

	//Create a default 2D rectangle as a helper for creating a letter sheet (verts will dynamically change)
	lettersRef = addMesh(lets, true, true, 10000); //create a dynamic buffer for small temp lettersheet

	//Create a default 2D rectangle as a helper for 2D GUI rendering 
	Pi3Cmesh rect = Pi3Cshapes::rect(vec2f(0, 0), vec2f(1.f, 1.f));
	rectRef = addMesh(rect);
}

void Pi3Cresource::createDefaultMaterial(const std::string &name)
{
	Pi3Cmaterial defmaterial;
	defmaterial.texID = createDefaultTexture(defmaterial.texRef);
	defmaterial.name = name;
	defmaterial.texName = name;
	defmaterial.texWidth = 1;
	defmaterial.texHeight = 1;
	materials.push_back(defmaterial);
}


int32_t Pi3Cresource::createDefaultTexture(int32_t &texRef)
{

	/* Creates a 1 pixel white texture
	Can be recoloured by material colDiffuse */
	std::shared_ptr<Pi3Ctexture> Texture;
	Texture.reset(new Pi3Ctexture());
	Texture->createColour(0xffffffff);
	Texture->upload();

	//keep texture - don't destroy it!
	texRef = textures.size();
	textures.push_back(Texture); 

	return Texture->textureID; //textures.size();
}

std::vector<float> * Pi3Cresource::getLetterVerts()
{
	if (letterSheetRef < 0) return nullptr;
	Pi3Cmesh &mesh = meshes[letterSheetRef];
	return &vertBuffer[mesh.bufRef];
}

void Pi3Cresource::updateLetterVerts(uint32_t vertCount, const uint32_t vertOffset)
{
	Pi3Cmesh *mesh = &meshes[letterSheetRef];
	if (letterSheetRef < 0 ) return; //|| (vertCount / mesh->stride) > vertBuffer[mesh->bufRef].size()
	mesh->vc = vertCount;
	mesh->vertSize = vertCount / mesh->stride;
	mesh->vertOffset = vertOffset;
	if (uploaded) updateGPUverts(mesh->bufRef, mesh->vertOffset*mesh->stride, vertCount, vertBuffer[mesh->bufRef]);
}

int32_t Pi3Cresource::addTexture(const std::shared_ptr<Pi3Ctexture> &Texture, int32_t &texRef)
{
	if (Texture.get() != nullptr) {
		Texture->upload();
		texRef = textures.size();		//Texture reference in resource textures array
		textures.push_back(Texture);	//keep texture - don't destroy it
		return Texture->textureID;		//return GL texture ID
	}

	texRef = -1;
	return -1;
}

int32_t Pi3Cresource::loadTexture(const std::string &path, const std::string &texname, int32_t &texRef) {

	for (size_t i = 0; i < materials.size(); i++) {
		if (texname == materials[i].texName) {
			texRef = materials[i].texRef;
			SDL_Log("Using existing texture '%s'", texname.c_str());
			return materials[i].texID;
		}
	}

	std::shared_ptr<Pi3Ctexture> Texture;
	Texture.reset(new Pi3Ctexture());

	std::string filename = path + texname;
	SDL_Log("Loading texture '%s'", filename.c_str());

	Texture->loadFromFile(filename.c_str());
	if (Texture.get()==nullptr){
		SDL_Log("ERROR! texture '%s' not found!!", filename.c_str());
		return -1;
	}

	return addTexture(Texture, texRef);
}

void Pi3Cresource::addMeshVerts(const Pi3Cmesh &mesh, std::vector<float> &newverts, const uint32_t from, const uint32_t size, const uint32_t vertsize, const bool deleteVerts)
{
	memcpy(newverts.data(), &mesh.verts[from], size * sizeof(float));
	Pi3Cmesh newmesh = mesh.clone(from, (deleteVerts) ? 0 : size);
	newmesh.vertSize = vertsize;
	vertBuffer.push_back(newverts);
	currentBuffer++;
	newmesh.bufRef = currentBuffer;
	meshes.push_back(newmesh);
}

int32_t Pi3Cresource::addMesh(Pi3Cmesh &mesh, const bool deleteVerts, const bool dynamicBuffer, const uint32_t vertSize)
{ 
	//SDL_Log("Creating verts buffer %d",currentBuffer);
	if (currentBuffer<0) {
		//No buffers created yet, create one ...
		vertBuffer.emplace_back();
		currentBuffer++;
		mesh.vertOffset = 0;
	} 

	std::vector<float> &mverts = vertBuffer[currentBuffer];
	currentBufferSize = vertSize;
	uint32_t msz = mverts.size();
	uint32_t recordsInBuf = (msz + mesh.vc) / mesh.stride;
	uint32_t records = mesh.vc / mesh.stride;

	if (recordsInBuf > vertSize) {
		mesh.vertOffset = 0;
		if (records > vertSize) {
			uint32_t bufsReq = records / vertSize;
			std::vector<float> newverts;
			uint32_t memsize = vertSize * mesh.stride;
			newverts.resize(memsize);
			for (size_t b = 0; b < bufsReq; b++) {
				addMeshVerts(mesh, newverts, b * memsize, memsize, vertSize, deleteVerts);
			}
			uint32_t remainingRecords = records - bufsReq * vertSize;
			memsize = remainingRecords * mesh.stride;
			newverts.resize(memsize);
			addMeshVerts(mesh, newverts, bufsReq * vertSize * mesh.stride, memsize, remainingRecords, deleteVerts);
		}
		else {
			meshes.emplace_back();
			Pi3Cmesh &newmesh = meshes.back();
			newmesh = mesh.clone(0, (deleteVerts) ? 0 : mesh.vc);
			vertBuffer.push_back(mesh.verts);
			newmesh.vertOffset = 0;
			newmesh.vertSize = records;
			currentBuffer++;
			newmesh.bufRef = currentBuffer;
		}
	}
	else 
	{
		//else copy mesh verts into existing buffer
		meshes.emplace_back();
		Pi3Cmesh &newmesh = meshes.back();
		newmesh = mesh.clone(0, (deleteVerts) ? 0 : mesh.vc);
		mverts.resize((dynamicBuffer) ? vertSize * mesh.stride : msz + mesh.vc);
		if (mesh.verts.size()>0) memcpy(&mverts[msz], &mesh.verts[0], mesh.vc * sizeof(float));
		newmesh.vertOffset = msz / mesh.stride;
		newmesh.vertSize = records;
		newmesh.bufRef = currentBuffer;
	}

	//SDL_Log("Sizes: %d,%d", mesh.verts.size(), vertSize * mesh.stride);
	mesh.verts.resize(0); //free memory since these verts have been transferred

	//If buffer is dynamic, then reserve max number of vertices and create another buffer
	if (dynamicBuffer) {
		vertBuffer.emplace_back();
		currentBuffer++;
	}

	return meshes.size() - 1;
}

void Pi3Cresource::setRenderBuffer(const int bufRef) 
{
	/* RPi can't cope with more than 65k vertices per array.
	 * This function will setup the correct VBO array for the
	 * mesh if the VBO ref is different from the last VBO used */

	//dont bother changing buffer if its the same as last time
	if (lastVBO == bufRef) return; 

	//Select new Vertex buffer ...
	glBindBuffer(GL_ARRAY_BUFFER, VBOid[bufRef]);

	//Reset attributes for newly selected buffer ...
	GLuint ac = 0; GLuint pos = 0; GLuint stride4 = stride * 4;
	glVertexAttribPointer(ac, 3, GL_FLOAT, GL_FALSE, stride4, (void*)pos);  //verts
	glEnableVertexAttribArray(ac++); pos += 12;

	glVertexAttribPointer(ac, 3, GL_FLOAT, GL_FALSE, stride4, (void*)pos);	//normals
	glEnableVertexAttribArray(ac++); pos += 12;

	glVertexAttribPointer(ac, 2, GL_FLOAT, GL_FALSE, stride4, (void*)pos);	//uvs
	glEnableVertexAttribArray(ac++); pos += 8;

	lastVBO = bufRef;
}

void Pi3Cresource::renderText(const int meshRef, Pi3Cfont *font, std::string &text, const float wrapWidth)
{
	//Text meshref's can only be used once per frame (otherwise using the same meshRef will simply overwrite the previous text)
	std::vector<float> *verts = getLetterVerts();				//get ptr to vertices from resource letterVerts
	if (verts == nullptr) return;
	Pi3Crect size;
	uint32_t vertCount = font->textureRects(text, *verts, size, wrapWidth);	//generate verts from text	
	updateLetterVerts(vertCount);								//upload updated verts to GPU
	renderMesh(meshRef, GL_TRIANGLES);
}

void Pi3Cresource::renderMesh(const int meshRef, const GLenum rendermode)
{
	setRenderBuffer(meshes[meshRef].bufRef);
	meshes[meshRef].render(rendermode);
	calls++; //used for debugging
}

int32_t Pi3Cresource::touchMesh(const int meshRef, Pi3Ctouch &touch, const Pi3Cmatrix &tmat) const
{
	const Pi3Cmesh &mesh = meshes[meshRef];
	return mesh.touchPoint(touch, tmat, vertBuffer[mesh.bufRef]);
}

void Pi3Cresource::uploadMeshesToGPU()
{	
	/* upload geometry to the GPU */

	uint32_t vs = vertBuffer.size();
	glGenBuffers(vs, &VBOid[startVBO]); //create the GL buffer
	
	for (uint32_t i= startVBO; i< vs; i++) {
		GLuint vno = vertBuffer[i].size();
		if (vno > 0) {
			SDL_Log("VBO:%d, verts size:%d", VBOid[i], vno);
			glBindBuffer(GL_ARRAY_BUFFER, VBOid[i]); //Vertex buffer
			glBufferData(GL_ARRAY_BUFFER, vno * sizeof(float), &vertBuffer[i].front(), GL_DYNAMIC_DRAW); //GL_STATIC_DRAW
		}
	}
	startVBO = vs;

	//now that geometries are uploaded, we must start with a fresh buffer for new geometry ...
	currentBuffer = vs;
	vertBuffer.emplace_back(); //create new empty buffer
	uploaded = true;
}

void Pi3Cresource::uploadGPUverts(const int bufref, const std::vector<float> &verts)
{
	setRenderBuffer(bufref);
	glBufferData(GL_ARRAY_BUFFER, verts.size()*sizeof(float), verts.data(), GL_DYNAMIC_DRAW);
}

void Pi3Cresource::updateGPUverts(const int bufref, const int vfrom, const int vsize, const std::vector<float> &verts)
{
	setRenderBuffer(bufref);
	glBufferSubData(GL_ARRAY_BUFFER, vfrom * sizeof(float), vsize * sizeof(float), verts.data());
}

int32_t Pi3Cresource::addShader(const std::string &vertfile, const std::string &fragfile)
{
	shaders.emplace_back();
	Pi3Cshader &shader = shaders.back();
	errorStr = shader.create(vertfile.c_str(), fragfile.c_str());
	if (errorStr != "") return -1;

	shader.Use();
	shader.lightPos = vec3f(0, 0, 1000.f);
	shader.setFog(100.f, 5000.f, vec3f(1.f, 1.f, 1.f));
	shader.setupShader();
	return shaders.size() - 1;
}

void Pi3Cresource::addFont(const char * path, const char * fontfile, int ptsize) {
	std::shared_ptr<Pi3Cfont> font;
	font.reset(new Pi3Cfont(path, fontfile, ptsize));
	if (font->font) fonts.insert({ fontfile, font });
}

void Pi3Cresource::addMusic(const char * path, const char * musicfile) {
	std::shared_ptr<Pi3Cmusic> smusic;
	smusic.reset(new Pi3Cmusic(musicfile));
	if (smusic->music) music.insert({ musicfile, smusic });
}

void Pi3Cresource::addSound(const char * path, const char * soundfile) {
	std::shared_ptr<Pi3Csound> sound;
	sound.reset(new Pi3Csound(soundfile));
	if (sound->sound) sounds.insert({ soundfile, sound });
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
	vertCount = 0;
	currentBuffer = -1;
	lastVBO = -1;
	startVBO = 0;
}
