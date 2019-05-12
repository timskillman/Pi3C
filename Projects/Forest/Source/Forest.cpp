
#include "Pi3C.h"
#include "Pi3Cshapes.h"
#include "Pi3CfileOBJ.h"
#include "Pi3CloadOptions.h"
#include "Pi3Cavatar.h"

#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

// ==========================================================================
// Forest example using the Skillman Media Raspberry Pi Graphics Library
// ==========================================================================
//
// The MIT License
//
// Copyright (c) 2018 Tim Skillman
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// =======================================================================

class particle {
public:

	particle(vec3f pos) { create(pos); }

	void create(const vec3f &pos) {
		this->pos = pos;
		dx = (float)(rand() % 1600) / 100.f - 8.f;
		size = (float)(rand() % 12) + 4.f;
		e = (float)(rand() % 100) / 100.f + 0.1f;
		gravity = -(float)(rand() % 20) - 10.f;
		dead = false;
		col = (float)(rand() % 4);
		//col = (rand() % 255) | ((rand() % 255) << 8) | ((rand() % 255) << 16) | ((rand() % 255) << 24) | (255<<24);
	}

	void animate(const float d)
	{
		if (dead) return;

		pos += vec3f(dx, -gravity, 0);
		if (pos.y < d) {
			if (std::fabs(gravity) < 1.0) {
				e = 0;
				pos.y = d;
				gravity = 0;
			}
			gravity = -gravity * 0.5f;
			pos.y -= gravity;
		}

		if (gravity == 0) {
			dx = dx * -0.95f;
			dead = (std::fabs(dx) < 0.1f);
		}

		gravity = gravity + e;
	}

	vec3f pos;
	float size = 1.f;
	float dx = 0;
	float e = 0.f;
	float gravity = 0.f;
	bool dead = false;
	uint32_t vertoffset = 0;
	float col = 0;

};


int main(int argc, char *argv[])
{
	loadOptions opts("options.txt");
	int screenWidth = opts.asInt("screenWidth");
	int screenHeight = opts.asInt("screenHeight");
	Pi3C pi3c(opts.asString("title"), screenWidth , screenHeight);

	Pi3CGL::showGLinfo();


	//Setup UI editor and attach functionality ...
	//SMUIform editform("editor", &resource2D, &window, &shader);	//create a UI form and attach window (so it can draw itself)
	//SMeditor editor(&editform, &scene, "editor.form", "ui/uimage_sm.png");		//create our editor class and attach the editor form and scene (so we can glue our UI and Scene elements together)

	// Get the loading bar widget from the Editor so we can use it while loading models ...
	//auto *loadingBar = static_cast<SMUIcontrols::loadingbar*>(editform.findControl("loadingBar"));
	//auto loadbarCallback = loadingBar->getCallback(&window, &shader);

	// Load models from modelFiles and upload vertex buffer object to GPU ..

	

	Pi3Cavatar player;
	Pi3Cavatar::avatarParams avparams;
	//avparams.movement = Pi3Cavatar::move_walk;
	//avparams.position = opts.asVec3f("startpos");
	//avparams.rotation = opts.asVec3f("rotate");
	//avparams.size = { 1.f, opts.asFloat("avatarHeight"), 1.f };
	//avparams.walkSpeed = opts.asFloat("avatarWalkSpeed");
	//avparams.runSpeed = opts.asFloat("avatarRunSpeed");
	//avparams.fallSpeed = opts.asFloat("avatarFallSpeed");
	player.init(avparams);

	//Pi3Cmodel brush = pi3c.create_model_from_mesh(Pi3Cshapes::sphere(vec3f(0, 0, -10.f), 2.0f, 0.f, 10, 20), 0xff00ffff);
	//brush.touchable = false;
	//int brushref = pi3c.add_model_to_scene3D(brush);

	std::vector<particle> flakes;
	Pi3Cmesh rectMesh;
	for (uint32_t r = 0; r < 5000; r++) {
		particle flake(vec3f((float)(rand() % 500 - 250), 0, (float)(rand() % 500 - 250)));
		flake.vertoffset = rectMesh.vc;
		flakes.push_back(flake);
		rectMesh.addRectangle(rectMesh.verts, flake.pos, vec3f(flake.size, flake.size, 0), vec2f((float)(rand() % 4)*0.25f, 0.f), vec2f(0.25f, 1.f));
	}
	rectMesh.vertSize = rectMesh.vc / rectMesh.stride;
	rectMesh.bbox.bboxFromVerts(rectMesh.verts, 0, rectMesh.vc, rectMesh.stride);
	rectMesh.materialRef = 0;
	int rectsMesh = pi3c.resource.addMesh(rectMesh, true, true);
	Pi3Cmodel rectangles;
	rectangles.meshRef = rectsMesh;
	rectangles.addTexture(&pi3c.resource, "../../Resources/models/maps/tree.png");
	rectangles.material.illum = 1;
	rectangles.material.alpha = 1.f;
	int rectsRef = pi3c.add_model_to_scene2D(rectangles);

	int skybox = pi3c.scene.loadModelOBJ(opts.asString("skyboxPath"), opts.asString("skybox"), 0); // loadbarCallback);
	pi3c.scene.models[skybox].matrix.SetScale(opts.asFloat("skyboxScale"));
	pi3c.scene.models[skybox].touchable = false;

	//int ship = pi3c.load_model("../../Resources/models/CargoBay", "CargoHoldBaked3.obj");

	pi3c.scene.setFog(0xffffff, opts.asFloat("fogNear"), opts.asFloat("fogFar"));
	pi3c.scene.setPerspective3D(screenWidth, screenHeight, opts.asFloat("perspective"), opts.asFloat("nearz"), opts.asFloat("farz"));

	//if (opts.logo != "") {
	//	SDL_Log("Loading logo");
	//	SMmodel logoModel(&resource, SMshapes::rect(vec2f(20, 20), vec2f(150, 80)), 0xffffffff);
	//	//logoModel.material.SetAnimationSheet(16, 13, 200);
	//	scene.append2D(logoModel, opts.modelPath+"/"+opts.logo);
	//}

	//Mix_Music *menuMusic = nullptr;
	//if (opts.sound != "") {
	//	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
	//		SDL_Log("couldn't initialize mix, error: %s\n", Mix_GetError());
	//	}
	//	SDL_Log("Loading sound");
	//	std::string spath = opts.soundPath + "/" + opts.sound;
	//	menuMusic = Mix_LoadMUS(spath.c_str());
	//	if (Mix_PlayingMusic() == 0) Mix_PlayMusic(menuMusic, -1);
	//}

	//for (size_t m = 0; m < opts.models.size(); m++) {
	//	int32_t modelRef = pi3c.scene.loadModelOBJ(opts.modelPath, opts.models[m], nullptr); // loadbarCallback);
	//	if (opts.colliders[m] != "") {
	//		Pi3Cmodel collider(&pi3c.resource, opts.modelPath, opts.colliders[m]); // , loadbarCallback, true);
	//		pi3c.scene.models[modelRef].appendCollider(collider);
	//	}
	//	//if (modelRef >= 0) pi3c.scene.models[modelRef].matrix.setMoveRotate(opts.positions[m],opts.rotations[m]);
	//}

	//pi3c.resource.uploadMeshesToGPU();

	// Setup matrices for shader ..

	while (pi3c.is_running())
	{
		//process key presses ...

		player.moveKeys(SDL_SCANCODE_W, SDL_SCANCODE_S, SDL_SCANCODE_A, SDL_SCANCODE_D, SDL_SCANCODE_R, SDL_SCANCODE_F);
		const uint8_t *keystate = pi3c.window.getKeys();
		player.run(keystate[SDL_SCANCODE_LSHIFT] || keystate[SDL_SCANCODE_RSHIFT]);
		if (keystate[SDL_SCANCODE_W]) player.forward();
		if (keystate[SDL_SCANCODE_S]) player.back();
		if (keystate[SDL_SCANCODE_A]) player.left();
		if (keystate[SDL_SCANCODE_D]) player.right();
		if (keystate[SDL_SCANCODE_R]) player.up();			//only when flying
		if (keystate[SDL_SCANCODE_F]) player.down();		//only when flying
		if (keystate[SDL_SCANCODE_SPACE]) player.jump();	//only when walking
		if (keystate[SDL_SCANCODE_ESCAPE]) pi3c.window.setquit(true);
		player.updateAndCollide(&pi3c.scene, pi3c.window.getTicks());

		// Centre skybox and other 'infinite' geometry around player...
		//if (skybox >= 0) pi3c.scene.models[skybox].matrix.move(-player.getPosition());

		// process window events ...

		while (pi3c.do_events()) {
			switch (pi3c.window.ev.type)
			{
			case SDL_TEXTINPUT:
				SDL_Log("Key = %d", pi3c.window.ev.text.text);
				break;
			case SDL_MOUSEMOTION:
				if (pi3c.window.mouse.RightButton) {
					player.rotate(vec3f(pi3c.window.mouse.deltaXY.y / 300.f, -pi3c.window.mouse.deltaXY.x / 300.f, 0));
				}
				break;
			case SDL_MOUSEWHEEL:
				//modmtx.Translate(vec3f(0,0,-((float)window.mousewheel)*1.f));
				//shader.SetMatrix4f(modmtxh, modmtx);
				break;
			case SDL_WINDOWEVENT:
				break;
			case SDL_DROPFILE:
				std::string file = pi3c.get_dropfiles()[0];
				if (file.substr(file.size() - 4, 4) == ".obj") {
					int32_t newmodel = pi3c.load_model("", file);
					if (newmodel >= 0) {
						pi3c.model(newmodel)->move(-player.getPosition());
						pi3c.resource.uploadMeshesToGPU();
					}
				}
				break;
			}
		}

		//clear background ...
		pi3c.clear_window();
	
		//Render 3D scene ...
		Pi3Cmesh &rm = pi3c.resource.meshes[rectsMesh];
		std::vector<float> &verts = pi3c.resource.vertBuffer[rm.bufRef];
		rm.vc = rm.vertOffset*rm.stride;
		Pi3Cmatrix *mm = pi3c.scene.getModelMatrix();
		vec2f cent(pi3c.window.getWidth() / 2, pi3c.window.getHeight() / 2);

		for (uint32_t r = 0; r < flakes.size(); r++) {
			particle &flake = flakes[r];
			rm.updateRectTransformCoords(verts, flake.pos, vec3f(flake.size, flake.size, 0), mm, cent); //, vec2f(flake.col*0.25f, 0), vec2f(0.25f, 1.f)
		}

		/*
		std::vector<std::pair<float, int32_t>> zflakes;

		for (uint32_t r = 0; r < flakes.size(); r++) {
			particle &flake = flakes[r];
			float z = mm->transformZ(flake.pos); if (z < 0) continue;
			std::pair<float, int32_t> zflake{ z,r };
			zflakes.push_back(zflake);

			//flake.animate(0);
			//if (flake.dead)
			//	flake.create(vec3f((float)(pi3c.window.getWidth() / 2), (float)(rand() % 500), (float)(rand() % 500))); //(float)(rand() % 500)
		}

		std::sort(zflakes.begin(), zflakes.end(), [=](std::pair<float, int32_t>& a, std::pair<float, int32_t>& b) { return a.first > b.first; });

		for (uint32_t r = 0; r < zflakes.size(); r++) {
			particle &flake = flakes[zflakes[r].second];
			rm.updateRectTransformCoords(verts, flake.pos, vec3f(flake.size, flake.size, 0), vec2f(flake.col*0.25f, 0), vec2f(0.25f, 1.f), mm,vec2f(pi3c.window.getWidth()/2, pi3c.window.getHeight()/2));
		}
		*/

		pi3c.resource.updateMesh(rectsMesh); //upload all the altered vertices for this mesh

		pi3c.scene.setSun(0xffffff, vec3f(1000.f, 1000.f, -1000.f)); //transform sun position into scene
		pi3c.scene.setMatrix(player.getPosition(), vec3f(0,0,0), player.getRotation());
		//glDisable(GL_DEPTH_TEST);
		pi3c.render3D();
		//glEnable(GL_DEPTH_TEST);
		
		//Render 2D
		pi3c.render2D();

		pi3c.swap_buffers();
		
	}

	//resource.clearAll();
	SDL_Log("Average FPS = %f ", pi3c.getAverageFPS());

	pi3c.window.destroy();

	return 0;
}
