#include "Pi3Cscene.h"
#include <algorithm>
#include <cstring>
#include "Pi3CfileOBJ.h"

void Pi3Cscene::render(const float ticks, Pi3Cmatrix &projMatrix, Pi3Cmatrix &modelMatrix, std::vector<Pi3Cmodel> &mods, Pi3Cmaterial *materialOverride, uint32_t flags)
{
	resource->shaders[currentShader].ticks = ticks;
	resource->shaders[currentShader].setProjectionMatrix(projMatrix);
	for (auto &model : mods) {
		Pi3Cmaterial *mat = (model.selected && materialOverride) ? &selectMaterial : materialOverride;
		model.render(resource, resource->shaders[currentShader], &modelMatrix, mat, flags);
	}
}

void Pi3Cscene::render3D(const float ticks, Pi3Cmaterial *materialOverride, uint32_t flags)
{
	render(ticks, projMatrix3D, modelMatrix3D, models, materialOverride, flags);
}

void Pi3Cscene::setup2D()
{
	resource->shaders[currentShader].setProjectionMatrix(projMatrix2D);
}

void Pi3Cscene::render2D(const float ticks)
{
	glClear(GL_DEPTH_BUFFER_BIT);
	render(ticks, projMatrix2D, modelMatrix2D, models2D, nullptr);
}

void Pi3Cscene::renderSkybox(const vec3f &pos, const int32_t skyboxref, const float ticks)
{
	resource->shaders[currentShader].ticks = ticks;
	resource->shaders[currentShader].setProjectionMatrix(projMatrix3D);
	if (skyboxref >= 0 && skyboxes.size() > 0) {
		skyboxes[skyboxref].matrix.move(pos);
		skyboxes[skyboxref].render(resource, resource->shaders[currentShader], &modelMatrix3D);
	}
}

bool Pi3Cscene::collide(const vec3f &pos, const vec3f &dir, const float radius) const
{
	if (dir.isZero()) return false;
	for (const auto &model : models) {
		if (model.collide(resource, nullptr, pos, dir, radius)) return true;
	}
	return false;
}

float Pi3Cscene::collideFloor(const vec3f &pos) const
{
	float ht = 1e8f;  //start from way up high!
	for (const auto &model : models) {
		ht = model.collideFloor(resource, nullptr, pos, ht);
	}
	return ht; //and see where we land :)
}

int32_t Pi3Cscene::loadModelOBJ(const std::string &path, const std::string &modelfile, const vec3f pos, const bool grouped, const int32_t groupId, bool addColliderGrid, 
	const std::function<void(float)> showProgressCB)
{
	if (modelfile == "") return -1;
	std::string newfile = modelfile;
	std::string newpath = (path == "") ? Pi3Cutils::extractPath(newfile) : path;
	newfile = Pi3Cutils::extractName(newfile);

	std::string error;
	size_t meshStart = resource->meshes.size();
	Pi3CfileOBJ::load(newpath, newfile, resource, showProgressCB, false, addColliderGrid, groupId, error);
	size_t meshEnd = resource->meshes.size();

	if (grouped) {
		models.emplace_back();
		Pi3Cmodel &model = models.back();
		model.addMeshes(resource, modelfile, meshStart, meshEnd);
		model.matrix.move(pos);
		model.asCollider = addColliderGrid;
	}
	else
	{
		if (meshEnd - meshStart > 0) {
			for (size_t i = meshStart; i < meshEnd; i++) {
				Pi3Cmodel newModel(resource, i);
				newModel.matrix.Translate(pos);
				newModel.asCollider = addColliderGrid;
				models.push_back(newModel);
			}
		}
	}
	return models.size()-1; //reference returned is only meaningful with grouped flag set
}

int32_t Pi3Cscene::loadSkybox(const std::string &path, const std::string &file, const std::function<void(float)> showProgressCB, const float scale)
{
	int skybox = loadModelOBJ(path, file, vec3f(0,0,0), true, 0, false, showProgressCB); // loadbarCallback);
	models[skybox].matrix.SetScale(scale);
	models[skybox].touchable = false;

	//if (file == "") return -1;
	//Pi3Cmodel model;
	//model.loadOBJfile(resource, path, file, showProgressCB, false);
	//model.matrix.SetScale(scale);
	//skyboxes.push_back(model);
	//return skyboxes.size()-1;
	return skybox;
}

Pi3Cmodel * Pi3Cscene::find(const std::string &name)
{
	//Search through model heirarchy
	//first model with matching name is returned - so use unique names!
	for (auto &m : models) {
		Pi3Cmodel * found = m.find(name);
		if (found) return found;
	}
	return nullptr;
}

Pi3Cbbox3d Pi3Cscene::getSelectedBounds()
{
	Pi3Cbbox3d bbox;
	for (auto &model : models) {
		if (model.selected && model.visible && !model.deleted) {
			bbox.update(model.bbox.bboxFromTVerts(&model.matrix));
		}
	}
	return bbox;
}

void Pi3Cscene::addTexture(Pi3Cmodel& model, const std::string &txfile, bool shaded)
{
	if (txfile != "") {
		model.addTexture(resource, txfile);
		model.material.illum = (shaded) ? 2 : 1;		//If shaded, material has max illumination
		model.material.alpha = (shaded) ? 1.f : 0.99f;
	}
}

int32_t Pi3Cscene::append3D(Pi3Cmodel& model, const std::string &txfile)
{
	addTexture(model, txfile);
	models.push_back(model); 
	return models.size() - 1;
}

int32_t Pi3Cscene::append2D(Pi3Cmodel model, const std::string &txfile)
{
	addTexture(model, txfile, false);
	models2D.push_back(model);
	return models2D.size() - 1;
}

void Pi3Cscene::setMatrix(const vec3f &firstpos, const vec3f &thirdpos, const vec3f &rot)
{
	modelMatrix3D.setModelMatrix(firstpos, thirdpos, rot);
}

Pi3Ctouch Pi3Cscene::touch(const vec3f &mousexyz, const bool calcPerspective) // vec3f tp, Pi3Cmodel *&selgroup, Pi3Cmodel *&selmodel, int32_t &meshRef, int32_t &triref, vec3f &intersection)
{
	//calc ray position and direction from x,y screen coords and transform into model world space
	Pi3Ctouch touch;

	if (calcPerspective) {
		touch.calcRay(mousexyz, perspective);
	}
	else {
		touch.calcRay(modelMatrix3D.transformRotateVec(mousexyz), 0);
	}

	//int32_t level = 0;
	float dist = 1e8f;
	for (size_t i = 0; i < models.size(); i++) {
		models[i].touch(resource, &modelMatrix3D, touch, 1);
		if (touch.prevDist < dist) {
			touch.groupRefs[0] = (int32_t)i; 
			dist = touch.prevDist;
		}
	}

	if (touch.touched()) {
		Pi3Cmatrix imtx = modelMatrix3D.inverse();
		touch.intersection = imtx.transformVec(touch.intersection);
	}

	return touch;
}

void Pi3Cscene::touchRect(const Pi3Crect touchRect, const viewInfo& view)
{
	setSceneMatrix(view);
	for (auto& model : models) {
		if (model.touchRect(resource, &modelMatrix3D, projMatrix3D, touchRect)) model.selected = true; else model.selected = false;
	}
}

Pi3Cmodel * Pi3Cscene::getSubModel(const int32_t * groupRefs, const int32_t maxlevel)
{
	switch (maxlevel) {
	case 0: return &models[groupRefs[0]];
	case 1: return &models[groupRefs[0]].group[groupRefs[1]];
	case 2: return &models[groupRefs[0]].group[groupRefs[1]].group[groupRefs[2]];
	case 3: return &models[groupRefs[0]].group[groupRefs[1]].group[groupRefs[2]].group[groupRefs[3]];
	case 4: return &models[groupRefs[0]].group[groupRefs[1]].group[groupRefs[2]].group[groupRefs[3]].group[groupRefs[4]];
	case 5: return &models[groupRefs[0]].group[groupRefs[1]].group[groupRefs[2]].group[groupRefs[3]].group[groupRefs[4]].group[groupRefs[5]];
	case 6: return &models[groupRefs[0]].group[groupRefs[1]].group[groupRefs[2]].group[groupRefs[3]].group[groupRefs[4]].group[groupRefs[5]].group[groupRefs[6]];
	case 7: return &models[groupRefs[0]].group[groupRefs[1]].group[groupRefs[2]].group[groupRefs[3]].group[groupRefs[4]].group[groupRefs[5]].group[groupRefs[6]].group[groupRefs[7]];
	}
	return nullptr;
}

void Pi3Cscene::setSun(const uint32_t col, const vec3f &pos, const int32_t ref)
{
	setFixedLight(col, modelMatrix3D.transformVec(pos), ref);
}

void Pi3Cscene::setFixedLight(const uint32_t col, const vec3f &pos, const int32_t ref)
{
	resource->shaders[(ref < 0) ? currentShader : ref].setSun(pos, col);
}

void Pi3Cscene::setFog(const uint32_t col, const float start, const float end, const int32_t ref)
{
	resource->shaders[(ref < 0) ? currentShader : ref].setFog(start, end, vec3f((float)(col & 255)/255.f, (float)((col>>8) & 255) / 255.f, (float)((col >> 16) & 255) / 255.f));
}

void Pi3Cscene::setPerspective3D(const int32_t width, const int32_t height, const float pspmul, const float znear, const float zfar)
{
	nearz3D = znear;
	farz3D = zfar;
	perspective = pspmul;
	projMatrix3D.SetPerspective(width, height, perspective, znear, zfar);
}

void Pi3Cscene::setOrthographic3D(const Pi3Crecti& rect, const float zoom, const float znear, const float zfar)
{
	//projMatrix3D.SetOrtho(rect.x, rect.x + rect.width, rect.y + rect.height, rect.y , znear, zfar);
	//projMatrix3D.SetOrtho((int)(((float)rect.width)*zoom), (int)(((float)-rect.width)*zoom), (int)(((float)-rect.height)*zoom), (int)(((float)rect.height)*zoom), znear, zfar);
	float izm = 1.f / zoom;
	projMatrix3D.SetOrtho((float)rect.width * izm, -(float)rect.width * izm, -(float)rect.height * izm, (float)rect.height * izm, znear, zfar, -1.f);
}

void Pi3Cscene::setViewport(const Pi3Crecti &rect)
{
	glViewport(rect.x, rect.y, rect.width, rect.height);
}

void Pi3Cscene::setSceneMatrix(const viewInfo& view)
{
	setViewport(view.viewport);
	switch (view.projection)
	{
	case viewInfo::PERSPECTIVE:
		setMatrix(view.pan, vec3f(0, 0, -view.zoom), view.rot);
		setPerspective3D(view.viewport.width, view.viewport.height, view.pspvalue, view.psp_nearz, view.psp_farz);
		break;
	case viewInfo::ORTHOGRAPHIC:
		setMatrix(view.pos + view.pan, vec3f(0, 0, 0), view.rot);
		setOrthographic3D(view.viewport, view.zoom, view.ortho_nearz, view.ortho_farz);
		break;
	}
}

void Pi3Cscene::renderView(const viewInfo& view, Pi3Cmaterial* outlines)
{
	setViewport(view.viewport);
	switch (view.projection)
	{
	case viewInfo::PERSPECTIVE:
		setMatrix(view.pan, vec3f(0, 0, -view.zoom), view.rot);
		setPerspective3D(view.viewport.width, view.viewport.height, view.pspvalue, view.psp_nearz, view.psp_farz);
		setSun(0xffffff, vec3f(1000.f, 1000.f, 800.f)); //transform sun position into scene
		render3D(view.ticks);
		//render3D(view.ticks, outlines);
		break;
	case viewInfo::ORTHOGRAPHIC:
		setMatrix(view.pos + view.pan, vec3f(0, 0, 0), view.rot);
		setOrthographic3D(view.viewport, view.zoom, view.ortho_nearz, view.ortho_farz);
		setFixedLight(0xffffff, vec3f(1000.f, 1000.f, 1000.f));
		render3D(view.ticks, outlines);
		break;
	}
}

void Pi3Cscene::setViewport2D(const Pi3Crecti &rect, const float znear, const float zfar)
{
	nearz2D = znear;
	farz2D = zfar;
	projMatrix2D.SetOrtho((float)rect.x, (float)(rect.x+rect.width), (float)(rect.y + rect.height), (float)rect.y, znear, zfar);
}

void Pi3Cscene::resize(const Pi3Crecti &rect)
{
	projMatrix3D.SetPerspective(rect.width, rect.height, perspective, nearz3D, farz3D);
	projMatrix2D.SetOrtho((float)rect.x, (float)(rect.x + rect.width), (float)(rect.y + rect.height), (float)rect.y, nearz2D, farz2D);
}

//void Pi3Cscene::deleteSelection()
//{
//	undos.deleteSelections(models);
//	for (auto &model : models) if (model.selected) model.deleted = true;
//}

void Pi3Cscene::flipImage(std::vector<uint8_t> &src, std::vector<uint8_t> &dest, uint32_t w, uint32_t h)
{
	uint32_t span = w * 4;
	uint8_t *pt = &src[0] + 0;
	uint8_t *pb = &dest[0] + (h-1) * span;
	for (uint32_t y = 0; y < h; y++) {
		std::memcpy(pb, pt, span);
		pb -= span;
		pt += span;
	}
}

void Pi3Cscene::saveBufferToPNG(const char * filename, std::vector<uint8_t> &snapShot, const int width, const int height)
{
		std::vector<uint8_t> destimage;
		destimage.resize(snapShot.size());
		flipImage(snapShot, destimage, width, height);

		SDL_Surface *ss = SDL_CreateRGBSurfaceFrom(&destimage[0], width, height, 32, width * 4, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
		SDL_RWops *fo = SDL_RWFromFile(filename, "wb");
		IMG_SavePNG_RW(ss, fo, 0);
		SDL_RWclose(fo);
		SDL_FreeSurface(ss);
}

void Pi3Cscene::checkFBerrors()
{
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		switch (status)
		{
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			SDL_Log("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			SDL_Log("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT");
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED:
			SDL_Log("GL_FRAMEBUFFER_UNSUPPORTED");
			break;
		default:
			SDL_Log("Unable to create FrameBuffer");
			break;
		}
	}
	else {
		SDL_Log("FrameBuffer created");
	}
}

void Pi3Cscene::renderOffscreen(const viewInfo& viewinfo, Pi3Cmaterial* outlines, int width, int height) //, const Pi3Cshader &shader
{
//#ifdef __arm__
	viewInfo newview = viewinfo;
	 
	if (width <= 0 || height <= 0) {
		width = viewinfo.viewport.width;
		height = viewinfo.viewport.height;
	}

	newview.viewport = Pi3Crecti(0, 0, width, height);

	GLuint frame_buf = 0, colour_buf = 0, depth_buf = 0;

	glGenRenderbuffers(1, &colour_buf);
	glBindRenderbuffer(GL_RENDERBUFFER, colour_buf);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8_OES, width, height);

	glGenRenderbuffers(1, &depth_buf);
	glBindRenderbuffer(GL_RENDERBUFFER, depth_buf);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);

	glGenFramebuffers(1, &frame_buf);
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buf);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colour_buf);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_buf);

	//checkFBerrors();

	//RENDER
	glViewport(0, 0, width, height);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);

	//glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clear colour and Z buffer

	renderView(newview, outlines);
	
	std::vector<uint8_t> data(width*height * 4);
	glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);

	saveBufferToPNG("offscreen.png", data, width, height);
	
	//deinit frame buffer:
	glDeleteFramebuffers(1, &frame_buf);
	glDeleteRenderbuffers(1, &depth_buf);
	glDeleteRenderbuffers(1, &colour_buf);

	// Return to onscreen rendering:
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
//#endif

//#ifdef __WINDOWS__
//	//initialize frame buffer
//	GLuint fbo = 0, render_buf = 0, depth_buf = 0;
//	glGenFramebuffers(1, &fbo);
//	//glBindFramebuffer(GL_FRAMEBUFFER, fbo);
//	//glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, render_buf);
//
//	glGenRenderbuffers(1, &render_buf);
//	glBindRenderbuffer(GL_RENDERBUFFER, render_buf);
//	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, width, height);
//
//	glGenRenderbuffers(1, &depth_buf);
//	glBindRenderbuffer(GL_RENDERBUFFER, depth_buf);
//	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
//
//	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
//	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, render_buf);
//	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_buf);
//
//	//render
//	glViewport(0, 0, width, height);
//	glEnable(GL_DEPTH_TEST);
//	glEnable(GL_MULTISAMPLE);
//
//	glClearColor(0, 0, 0, 0);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clear colour and Z buffer
//
//	//RENDER
//	render3D();
//
//	std::vector<std::uint8_t> data(width*height * 4);
//	//glFramebufferReadBufferEXT(GL_COLOR_ATTACHMENT0);
//	glReadBuffer(GL_COLOR_ATTACHMENT0);
//	glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);
//
//	//write png
//	SDL_Surface *ss = SDL_CreateRGBSurfaceFrom(&data[0], width, height, 32, width * 4, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
//
//	//SDL_RWops *fo = SDL_RWFromFile("thumb.png", "wb");
//	//IMG_SavePNG_RW(ss, fo, 0);
//	//SDL_RWclose(fo);
//
//	IMG_SavePNG_RW(ss, f, 0);
//	SDL_FreeSurface(ss);
//	//SDL_RWclose(f);
//
//	//deinit frame buffer:
//	glDeleteFramebuffers(1, &fbo);
//	glDeleteRenderbuffers(1, &render_buf);
//	glDeleteRenderbuffers(1, &depth_buf);
//
//	// Return to onscreen rendering:
//	glBindFramebufferEXT(GL_FRAMEBUFFER, 0);
//
//	//glViewport(vx, vy, vw, vh); //restore viewport to what it was
//	glEnable(GL_DEPTH_TEST);
//#endif
}
