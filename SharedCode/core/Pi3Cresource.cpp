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
	lets.vc = 10000 * 6 * stride;
	lets.verts.resize(lets.vc); // 10000 letters - reserve MUST be multiple of consistent stride (since glDrawArrays requires a vertex index not a byte index)
	letterSheetRef = addMesh(&lets); //create a dynamic buffer for the lettersheet (page size)

	//Reserve for small text entry fields ...
	lets.vc = 1000 * 6 * stride;
	lets.verts.resize(lets.vc);
	lettersRef = addMesh(&lets); //create a dynamic buffer for small temp lettersheet

	//Create a default 2D rectangle as a helper for 2D GUI rendering 
	Pi3Cmesh rect = Pi3Cshapes::rect(vec2f(0, 0), vec2f(1.f, 1.f));
	rectRef = addMesh(&rect); //buffer used for creating models etc..

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
		SDL_Log("couldn't initialize mix, error: %s\n", Mix_GetError());
	}
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

void Pi3Cresource::updateMesh(const uint32_t meshRef, uint32_t vertCount, const uint32_t vertOffset)
{
	if (meshRef < 0) return; //|| (vertCount / mesh->stride) > vertBuffer[mesh->bufRef].size()
	Pi3Cmesh &mesh = meshes[meshRef];
	if (vertCount > 0) {
		mesh.vc = vertCount;
		mesh.vertSize = vertCount / mesh.stride;
		mesh.vertOffset = vertOffset;
	}
	updateGPUverts(mesh.bufRef, mesh.vertOffset*mesh.stride, mesh.vc, vertBuffer[mesh.bufRef]);
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

int32_t Pi3Cresource::addMesh(Pi3Cmesh * mesh, uint32_t maxsize)
{
	//Are we requesting too much?  Then attempt it ...
	uint32_t mvsize = (mesh) ? mesh->verts.size() : 0;
	if (mvsize > maxsize) maxsize = mvsize; // return -1;

	// check if we can squeeze this mesh into a buffer with space ...
	int32_t cbuf = currentBuffer;
	if (cbuf > 0 && mvsize > 0) {
		for (size_t b = 0; b < vertBuffer.size(); b++) {
			uint32_t spareBuffer = vertBuffer[b].size() - vertBufferPtr[b];
			if (spareBuffer > mvsize) {
				cbuf = b; break;
			}
		}
	}

	//No buffers created yet or, request is larger than current buffer?, then create new buffer ...
	bool vertsMoved = false;
	if (cbuf < 0 || (mvsize + vertBufferPtr[cbuf]) > vertBuffer[cbuf].size()) {
		if (cbuf>=0) SDL_Log("cbuf:%d, mvsize:%d + vertBufferPtr:%d, size=%d, maxsize = %d", cbuf, mvsize , vertBufferPtr[cbuf], vertBuffer[cbuf].size(), maxsize);
		mesh->verts.resize(maxsize);
		vertBuffer.push_back(std::move(mesh->verts));
		vertsMoved = true;
		vertBufferPtr.push_back(0);
		currentBuffer++; cbuf = currentBuffer;

		//Create and reserve buffer in GPU ...
		glGenBuffers(1, &VBOid[currentBuffer]);
		SDL_Log("VBO:%d, Uploading floats = %d, mvsize=%d", VBOid[currentBuffer], maxsize, mvsize);
		glBindBuffer(GL_ARRAY_BUFFER, VBOid[currentBuffer]);
		glBufferData(GL_ARRAY_BUFFER, maxsize * sizeof(float), &vertBuffer[currentBuffer].front(), GL_DYNAMIC_DRAW);
	}

	if (mesh) {
		std::vector<float> &mverts = vertBuffer[cbuf];		//Get current buffer
		uint32_t &bufferPtr = vertBufferPtr[cbuf];

		//Upload mesh verts into GPU ...
		if (mvsize > 0) {
			uint32_t vcbytes = mesh->vc * sizeof(float);	
			if (!vertsMoved) {
				memcpy(&mverts[bufferPtr], &mesh->verts[0], vcbytes);
				mesh->verts.resize(0); //delete verts from original mesh as these have been stored and uploaded to GPU
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

Pi3Crect Pi3Cresource::renderText(const int meshRef, Pi3Cfont *font, const std::string &text, const vec3f &pos, const float wrapWidth, const uint32_t colour)
{
	//Text meshref's can only be used once per frame (otherwise using the same meshRef will simply overwrite the previous text)
	Pi3Crect size;
	Pi3Cmesh &mesh = meshes[meshRef];
	uint32_t vertCount = font->textureRects(text, vertBuffer[mesh.bufRef], size, wrapWidth, mesh.stride);	//generate verts from text	
	updateMesh(meshRef, vertCount);											//upload updated verts to GPU

	Pi3Cmatrix matrix;
	matrix.move(pos);
	//matrix.SetScales(vec3f((float)wh.x, (float)wh.y, 1.f));
	shaders[0].SetModelMatrix(matrix);
	shaders[0].setMaterial(&font->fontsheet.sheetMaterial);
	shaders[0].setColDiffuse(colour);

	renderMesh(meshRef, GL_TRIANGLES);
	return size;
}

void Pi3Cresource::renderMesh(const int meshRef, const GLenum rendermode)
{
	Pi3Cmesh &mesh = meshes[meshRef];
	setRenderBuffer(mesh.bufRef, mesh.stride);
	mesh.render(rendermode);
	calls++; //used for debugging
}

int32_t Pi3Cresource::touchMesh(const int meshRef, Pi3Ctouch &touch, const Pi3Cmatrix &tmat) const
{
	const Pi3Cmesh &mesh = meshes[meshRef];
	return mesh.touchPoint(touch, tmat, vertBuffer[mesh.bufRef]);
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
	vertBufferPtr.resize(0);
	vertCount = 0;
	currentBuffer = -1;
	lastVBO = -1;
	//startVBO = 0;
}
