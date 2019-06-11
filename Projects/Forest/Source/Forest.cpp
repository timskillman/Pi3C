
#include "Pi3C.h"
#include "Pi3Cshapes.h"
#include "Pi3CfileOBJ.h"
#include "Pi3CloadOptions.h"
#include "Pi3Cavatar.h"
#include "Pi3Cpopulate.h"

#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

// ==========================================================================
// Pi3C Graphics Library Example - Forest Flight (by Tim Skillman)
// ==========================================================================
//
// The MIT License
//
// Copyright (c) 2019 Tim Skillman
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
// Dedicated to the One who has blessed me beyond my dreams ...
// Jesus Christ, King of Kings and Lord of Lords :-)
// =======================================================================

#define maxBolts 100

class boltArray {
public:

	void init()
	{
		for (size_t i = 0; i < maxBolts; i++) {
			//vec3f p1(pos.x + dir.y, pos.y, pos.z - dir.z);
			//vec3f p2(pos.x + dir.y, pos.y, pos.z - dir.z);
			//vec3f p3(pos.x + dir.y, pos.y, pos.z - dir.z);
			//vec3f p4(pos.x + dir.y, pos.y, pos.z - dir.z);
			//Pi3Cshapes::quad(p1, p2, p3, p4);
		}
	}

	void fireBolt(const vec3f &pos, const vec3f &dir) {
		if (count < maxBolts) {
			this->pos[count] = pos;
			this->dir[count] = dir;
			this->life[count++] = 1000;

		}
	}

	void updateBolt(const uint32_t b)
	{
		float dx = pos[b].z * width; float dy = -pos[b].x *width;

	}

	void animate()
	{
		for (uint32_t b = 0; b < count; b++) {
			if (life[b] > 1000) {
				pos[b] += dir[b];
				life[b]--;
				updateBolt(b);
			}
		}
	}

	std::unique_ptr<Pi3Cmesh> boltMesh;
	vec3f pos[maxBolts];
	vec3f dir[maxBolts];
	int life[maxBolts]{ -1 };
	float width = 1.f;
	float length = 10.f;
	int32_t count = -1;
};

//float rnd(const uint32_t size) { return (float)(rand() % size); }

int main(int argc, char *argv[])
{
	loadOptions opts("options.txt");
	int screenWidth = opts.asInt("screenWidth");
	int screenHeight = opts.asInt("screenHeight");
	Pi3C pi3c(opts.asString("title"), screenWidth , screenHeight, opts.asBool("fullscreen"));

	Pi3CGL::showGLinfo();
	
	Pi3Cavatar player;
	Pi3Cavatar::avatarParams avparams;
	avparams.movement = Pi3Cavatar::move_fly;
	avparams.position = opts.asVec3f("startpos");
	//avparams.rotation = opts.asVec3f("rotate");
	avparams.size = { 1.f, opts.asFloat("avatarHeight"), 1.f };
	avparams.walkSpeed = opts.asFloat("avatarWalkSpeed");
	avparams.runSpeed = opts.asFloat("avatarRunSpeed");
	avparams.fallSpeed = opts.asFloat("avatarFallSpeed");
	player.init(avparams);
	
	int landRef = pi3c.load_model("../../Resources/models", opts.asString("landscape"));
	pi3c.scene.models[landRef].asCollider = true;  //use this model as a collider

	//Scatter trees in round clusters ...
	uint32_t maxTrees = opts.asInt("trees");		//Get max number of trees from options file
	uint32_t maxTreeTypes = 4;

	//Setup our tree array ...
	Pi3Cpopulate trees;
	Pi3Cmodel treesModel = trees.Populate(&pi3c.resource, maxTrees, pi3c.scene.models[landRef], "../../Resources/models/maps/redwoods.png", maxTreeTypes, 500.f);
	int treesModelRef = pi3c.add_model_to_scene3D(treesModel);

	boltArray bolts;

	int skybox = pi3c.scene.loadModelOBJ(opts.asString("skyboxPath"), opts.asString("skybox"), 0); // loadbarCallback);
	pi3c.scene.models[skybox].matrix.SetScale(opts.asFloat("skyboxScale"));
	pi3c.scene.models[skybox].touchable = false;

	
	int airport = pi3c.load_model("../../Resources/models", "airport.obj", vec3f(-500,0,0));

	int sship = pi3c.load_model("../../Resources/models", "EagleTransporter.obj"); //sship3 //EagleTransporter // NMSship
	int cockpit = pi3c.load_model("../../Resources/models", "sshipCockpit2.obj"); //sship //EagleTransporter // NMSship

	int ship = sship;
	Pi3Cbbox3d shipbox = pi3c.model(ship)->bbox;

	pi3c.scene.setFog(opts.asHex("fogColour"), opts.asFloat("fogNear"), opts.asFloat("fogFar"));
	pi3c.scene.setPerspective3D(screenWidth, screenHeight, opts.asFloat("perspective"), opts.asFloat("nearz"), opts.asFloat("farz"));
	

	Mix_Music *menuMusic = nullptr;
	if (opts.asString("sound") != "") {
		if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
			SDL_Log("couldn't initialize mix, error: %s\n", Mix_GetError());
		}
		SDL_Log("Loading sound");
		std::string spath = opts.asString("soundPath") + "/" + opts.asString("sound");
		menuMusic = Mix_LoadMUS(spath.c_str());
		if (Mix_PlayingMusic() == 0) Mix_PlayMusic(menuMusic, -1);
	}

	//for (size_t m = 0; m < opts.models.size(); m++) {
	//	int32_t modelRef = pi3c.scene.loadModelOBJ(opts.modelPath, opts.models[m], nullptr); // loadbarCallback);
	//	if (opts.colliders[m] != "") {
	//		Pi3Cmodel collider(&pi3c.resource, opts.modelPath, opts.colliders[m]); // , loadbarCallback, true);
	//		pi3c.scene.models[modelRef].appendCollider(collider);
	//	}
	//	//if (modelRef >= 0) pi3c.scene.models[modelRef].matrix.setMoveRotate(opts.positions[m],opts.rotations[m]);
	//}

	//pi3c.resource.uploadMeshesToGPU();

	vec3f shipRot;
	float rollDelta = 0;
	float takeoff = 0;
	float maxspeed = 30.f;
	player.flyspeed = 0;

	Pi3Cimgui &gui = pi3c.gui;	//expose gui object from pi3c

	while (pi3c.is_running())
	{
		//process key presses ...

		//player.moveKeys(SDL_SCANCODE_W, SDL_SCANCODE_S, SDL_SCANCODE_A, SDL_SCANCODE_D, SDL_SCANCODE_R, SDL_SCANCODE_F);
		const uint8_t *keystate = pi3c.window.getKeys();
		player.run(keystate[SDL_SCANCODE_LSHIFT] || keystate[SDL_SCANCODE_RSHIFT]);
		if (keystate[SDL_SCANCODE_W])  player.flyspeed += 0.1f; // player.forward();
		if (keystate[SDL_SCANCODE_S]) player.flyspeed -= 0.1f;  //player.back();
		if (keystate[SDL_SCANCODE_Z] || keystate[SDL_SCANCODE_X]) {
			//player.left();
			float ldelta = (keystate[SDL_SCANCODE_Z]) ? 0.03f : -0.03f;
			player.rotate(vec3f(0, ldelta, 0));
			shipRot.y -= ldelta;
			shipRot.z += ldelta*0.5f;
		}
		if (keystate[SDL_SCANCODE_SLASH] || keystate[SDL_SCANCODE_APOSTROPHE]) {
			float ldelta = (keystate[SDL_SCANCODE_SLASH]) ? 0.02f : -0.02f;
			player.rotate(vec3f(ldelta, 0, 0));
			shipRot.x -= ldelta *0.5f;
			//shipRot.y -= ldelta;
			//shipRot.z += ldelta * 0.5f;
		}
		//if (keystate[SDL_SCANCODE_D]) player.right();
		if (keystate[SDL_SCANCODE_R]) player.up();			//only when flying
		if (keystate[SDL_SCANCODE_F]) player.down();		//only when flying
		if (keystate[SDL_SCANCODE_SPACE]) player.jump();	//only when walking
		if (keystate[SDL_SCANCODE_ESCAPE]) pi3c.window.setquit(true);
		if (keystate[SDL_SCANCODE_C]) ship = cockpit; 
		if (keystate[SDL_SCANCODE_V]) ship = sship;
		//if (keystate[SDL_SCANCODE_SPACE]) player.flyspeed += 0.1f; // takeoff = 0.1f;
		//if (keystate[SDL_SCANCODE_L]) player.flyspeed -= 0.1f; //takeoff = -0.1f;
		if (keystate[SDL_SCANCODE_RETURN]) bolts.fireBolt(-player.getPosition(), player.getRotation());

		if (takeoff != 0) {
			player.flyspeed += takeoff;
			if (player.flyspeed > maxspeed) takeoff = 0;
			if (player.flyspeed < 0) {
				takeoff = 0; player.flyspeed = 0;
			}
		}

		pi3c.model(sship)->visible = (ship==sship);
		pi3c.model(cockpit)->visible = (ship==cockpit);
		player.updateAndCollide(&pi3c.scene, pi3c.window.getTicks());

		// Centre skybox and other 'infinite' geometry around player...
		if (skybox >= 0) pi3c.model(skybox)->move(-player.getPosition());

		// process window events ...

		while (pi3c.do_events()) {
			switch (pi3c.window.ev.type)
			{
			case SDL_TEXTINPUT:
				//SDL_Log("Key = %d", pi3c.window.ev.text.text);
				break;
			case SDL_MOUSEMOTION:
				//{
				if (pi3c.window.mouse.RightButton) {
					vec3f rotDelta(pi3c.window.mouse.deltaXY.y / 300.f, -pi3c.window.mouse.deltaXY.x / 300.f, 0);
					player.rotate(rotDelta);
					shipRot.y -= rotDelta.y;
					shipRot.z += rotDelta.y; // *0.5f;
					shipRot.x -= rotDelta.x; // *0.5f;
				}
				break;
			case SDL_MOUSEWHEEL:
				break;
			case SDL_WINDOWEVENT:
				break;
			}
		}

		Pi3Cmatrix rmat; rmat.rotate(-player.getRotation());

		vec3f offset = (ship == cockpit) ? vec3f(0, 0, 0) : -rmat.transformRotateVec(vec3f(0, shipbox.height()*1.5f, shipbox.depth()*1.5f));
		pi3c.model(ship)->move(-player.getPosition()+offset);
		pi3c.model(ship)->matrix.setRotate(shipRot);
		if (shipRot.z != 0) shipRot.z *= 0.95f;			//dampened roll to 0
		//if (shipRot.x != 0) shipRot.x *= 0.95f;			//dampened pitch to 0

		//clear background ...
		pi3c.clear_window();
	
		//Render 3D scene ...
		pi3c.scene.setMatrix(player.getPosition(), vec3f(0, 0, 0), player.getRotation());

		//treesModel.updateSpriteBillboard(&pi3c.resource, trees.positions, trees.sizes, -player.getPosition());

		pi3c.scene.setSun(0xffffff, vec3f(5000.f, 5000.f, -5000.f)); //transform sun position into scene
		pi3c.render3D();
		
		//Render 2D
		pi3c.render2D();
		gui.Begin();
		std::string fps = "FPS:" + std::to_string((int)pi3c.getCurrentFPS());
		gui.Text(fps, 0xff0000ff);

		pi3c.swap_buffers();
		
	}

	//resource.clearAll();
	SDL_Log("Average FPS = %f ", pi3c.getAverageFPS());

	pi3c.window.destroy();

	return 0;
}
