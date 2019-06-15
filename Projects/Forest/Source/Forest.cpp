
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
		float dx = pos[b].z * width; 
		float dy = -pos[b].x *width;
		
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

void changeShip(int shipType)
{

}

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
	
	int landRef = pi3c.load_model(opts.asString("modelPath"), opts.asString("landscape"));
	pi3c.scene.models[landRef].asCollider = true;  //use this model as a collider

	//Scatter trees in round clusters ...
	uint32_t maxTrees = opts.asInt("trees");		//Get max number of trees from options file
	uint32_t maxTreeTypes = 4;

	//Setup our tree array ...
	SDL_Log("Setting up %d trees ...",maxTrees);
	Pi3Cpopulate trees;
	Pi3Cmodel treesModel = trees.Populate(&pi3c.resource, maxTrees, pi3c.scene.models[landRef], "../../Resources/models/maps/redwoods.png", maxTreeTypes, 500.f);
	int treesModelRef = pi3c.add_model_to_scene3D(treesModel);

	//Pi3Cmodel altimeter;
	//altimeter.appendMesh(&pi3c.resource, Pi3Cshapes::rect(vec2f(20, 20), vec2f(1, 1), 0x00ff00),false);
	//int32_t altref = pi3c.scene.append2D(altimeter);

	boltArray bolts;

	SDL_Log("Loading models ...");
	int skybox = pi3c.scene.loadSkybox(opts.asString("skyboxPath"), opts.asString("skybox"), 0, opts.asFloat("skyboxScale")); // loadbarCallback);
	int airport = pi3c.load_model(opts.asString("modelPath"), "airport.obj", vec3f(-500, 0, 0));

	int sship = pi3c.load_model(opts.asString("modelPath"), "sship4.obj"); //sship3 //EagleTransporter // NMSship
	int cockpit = pi3c.load_model(opts.asString("modelPath"), "sshipCockpit2.obj"); //sship //EagleTransporter // NMSship

	int ship = sship;
	Pi3Cbbox3d shipbox = pi3c.model(ship)->bbox;

	pi3c.scene.setFog(opts.asHex("fogColour"), opts.asFloat("fogNear"), opts.asFloat("fogFar"));
	pi3c.scene.setPerspective3D(screenWidth, screenHeight, opts.asFloat("perspective"), opts.asFloat("nearz"), opts.asFloat("farz"));
	
	SDL_Log("Loading music and sounds ...");
	
	Mix_Music *menuMusic = nullptr;
	std::string spath = opts.asString("musicPath") + "/ActionStrike.ogg";
	menuMusic = Mix_LoadMUS(spath.c_str());
	if (Mix_PlayingMusic() == 0) Mix_PlayMusic(menuMusic, -1);
	
	Mix_Chunk *sound_laser = nullptr;
	spath = opts.asString("soundPath") + "/alien_laser_1.ogg";
	sound_laser = Mix_LoadWAV(spath.c_str());
	
	Mix_Chunk *sound_engineRunning = nullptr;
	spath = opts.asString("soundPath") + "/afterburner_1.ogg";
	sound_engineRunning = Mix_LoadWAV(spath.c_str());
	
	Mix_Chunk *sound_cockpit = nullptr;
	spath = opts.asString("soundPath") + "/cockpit_interior_rumble.ogg";
	sound_cockpit = Mix_LoadWAV(spath.c_str());
	
	Mix_PlayChannel(1, sound_engineRunning, -1);
	
	//std::shared_ptr<Pi3Cmusic> music = pi3c.resource.addMusic(opts.asString("musicPath"), "ActionStrike.ogg");
	//std::shared_ptr<Pi3Csound> sound_laser = pi3c.resource.addSound(opts.asString("soundPath"), "alien_laser_1.ogg");
	//std::shared_ptr<Pi3Csound> sound_engineRunning = pi3c.resource.addSound(opts.asString("soundPath"), "afterburner_1.ogg");
	//std::shared_ptr<Pi3Csound> sound_cockpit = pi3c.resource.addSound(opts.asString("soundPath"), "cockpit_interior_rumble.ogg");

	SDL_Log("Setting up sounds ...");
	//music->play(-1);
	//music->volume(120);
	//sound_engineRunning->play(1,-1);
	//sound_engineRunning->volume(60);

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
		if (keystate[SDL_SCANCODE_W])  player.flyspeed += 0.2f; // player.forward();
		if (keystate[SDL_SCANCODE_S]) player.flyspeed -= 0.2f;  //player.back();
		//if (keystate[SDL_SCANCODE_Z] || keystate[SDL_SCANCODE_X]) {
		//	//player.left();
		//	float ldelta = (keystate[SDL_SCANCODE_Z]) ? 0.03f : -0.03f;
		//	player.rotate(vec3f(0, ldelta, 0));
		//	shipRot.y -= ldelta;
		//	shipRot.z += ldelta*0.5f;
		//}
		//if (keystate[SDL_SCANCODE_SLASH] || keystate[SDL_SCANCODE_APOSTROPHE]) {
		//	float ldelta = (keystate[SDL_SCANCODE_SLASH]) ? 0.02f : -0.02f;
		//	player.rotate(vec3f(ldelta, 0, 0));
		//	shipRot.x -= ldelta *0.5f;
		//	//shipRot.y -= ldelta;
		//	//shipRot.z += ldelta * 0.5f;
		//}
		//if (keystate[SDL_SCANCODE_D]) player.right();
		if (keystate[SDL_SCANCODE_R]) player.up();			//only when flying
		if (keystate[SDL_SCANCODE_F]) player.down();		//only when flying
		//if (keystate[SDL_SCANCODE_SPACE]) player.jump();	//only when walking

		if (keystate[SDL_SCANCODE_C]) { ship = cockpit; Mix_PlayChannel(1, sound_cockpit, -1); } //sound_cockpit->play(1, -1); }
		if (keystate[SDL_SCANCODE_V]) { ship = sship; Mix_PlayChannel(1, sound_engineRunning, -1); } //sound_engineRunning->play(1, -1); }
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
			case SDL_MOUSEMOTION:
				//{
				if (pi3c.window.mouse.RightButton) {
					vec3f rotDelta(pi3c.window.mouse.deltaXY.y / 300.f, -pi3c.window.mouse.deltaXY.x / 300.f, 0);
					player.rotate(rotDelta);
					shipRot -= vec3f(rotDelta.x, rotDelta.y, -rotDelta.y);
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (pi3c.window.mouse.LeftButton) {
					Mix_PlayChannel(0, sound_laser, 0);
					//sound_laser->play(0, 0);
				}
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
		if (shipRot.x != 0 && ship == sship) shipRot.x *= 0.7f;			//dampened pitch to 0

		//clear background ...
		pi3c.clear_window();
	
		//Render 3D scene ...
		pi3c.scene.setMatrix(player.getPosition(), vec3f(0, 0, 0), player.getRotation());

		pi3c.scene.setSun(0xffffff, vec3f(5000.f, 5000.f, -5000.f)); //transform sun position into scene
		pi3c.render3D();
		
		//Render 2D
		//pi3c.model(altref)->resizeRect2D(&pi3c.resource, vec2f(20, 20), vec2f(20, player.altitude));
		pi3c.render2D();

		gui.Begin();
		std::string fps = "Altitude:" + std::to_string((int)player.altitude);
		gui.Text(fps, 0xff0000ff);

		pi3c.swap_buffers();
		
	}

	//resource.clearAll();
	SDL_Log("Average FPS = %f ", pi3c.getAverageFPS());

	pi3c.window.destroy();

	return 0;
}
