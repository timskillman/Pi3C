
#include "Pi3C.h"
#include "Pi3Cshapes.h"
#include "Pi3CfileOBJ.h"
#include "Pi3CloadOptions.h"
#include "Pi3Cavatar.h"
#include "Pi3Cpopulate.h"
#include "Pi3CperlinNoise.h"

#include "bullets.h"
#include "aliens.h"

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



int main(int argc, char *argv[])
{
	loadOptions opts("options.txt");
	Pi3Cwindow::options winopts;
	winopts.title = opts.asString("title");
	winopts.width = opts.asInt("screenWidth");
	winopts.height = opts.asInt("screenHeight");
	winopts.fullscreen = opts.asBool("fullscreen");
	//winopts.majorVsn = 3;
	//winopts.minorVsn = 0;
	//winopts.antialiasLevel = opts.asInt("antiAlias");
	Pi3C pi3c(winopts);
	int screenWidth = winopts.width;
	int screenHeight = winopts.height;

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
	Pi3Cpopulate trees;
	Pi3Cmodel treesModel = trees.Populate(&pi3c.resource, maxTrees, pi3c.scene.models[landRef], "../../Resources/models/maps/redwoods.png", maxTreeTypes, 500.f);
	int treesModelRef = pi3c.add_model_to_scene3D(treesModel);

	//Pi3Cmodel altimeter;
	//altimeter.appendMesh(&pi3c.resource, Pi3Cshapes::rect(vec2f(20, 20), vec2f(1, 1), 0x00ff00),false);
	//int32_t altref = pi3c.scene.append2D(altimeter);

	Bullets bullets;
	bullets.init(&pi3c.resource, &pi3c.scene, 100, 300);

	int aship = pi3c.load_model(opts.asString("modelPath"), "saucer.obj");

	Aliens aliens;
	aliens.init(&pi3c.resource, &pi3c.scene, landRef, aship, 40);

	int skybox = pi3c.scene.loadSkybox(opts.asString("skyboxPath"), opts.asString("skybox"), 0, opts.asFloat("skyboxScale")); // loadbarCallback);
	int airport = pi3c.load_model(opts.asString("modelPath"), "airport.obj", vec3f(-500, 0, 0));

	int sship = pi3c.load_model(opts.asString("modelPath"), "sship4.obj"); //sship3 //EagleTransporter // NMSship
	int cockpit = pi3c.load_model(opts.asString("modelPath"), "sshipCockpit2.obj"); //sship //EagleTransporter // NMSship

	int ship = sship;
	Pi3Cbbox3d shipbox = pi3c.model(ship)->bbox;

	pi3c.scene.setFog(opts.asHex("fogColour"), opts.asFloat("fogNear"), opts.asFloat("fogFar"));
	pi3c.scene.setPerspective3D(screenWidth, screenHeight, opts.asFloat("perspective"), opts.asFloat("nearz"), opts.asFloat("farz"));
	
	Mix_Music* menuMusic = nullptr;
	std::string spath = opts.asString("musicPath") + "/ActionStrike.ogg";
	menuMusic = Mix_LoadMUS(spath.c_str());
	
	Mix_Chunk* sound_laser = nullptr;
	spath = opts.asString("soundPath") + "/alien_laser_1.ogg";
	sound_laser = Mix_LoadWAV(spath.c_str());

	Mix_Chunk* sound_engineRunning = nullptr;
	spath = opts.asString("soundPath") + "/afterburner_1.ogg";
	sound_engineRunning = Mix_LoadWAV(spath.c_str());

	Mix_Chunk* sound_cockpit = nullptr;
	spath = opts.asString("soundPath") + "/cockpit_interior_rumble.ogg";
	sound_cockpit = Mix_LoadWAV(spath.c_str());

	Mix_PlayMusic(menuMusic, -1);
	Mix_PlayChannel(1, sound_engineRunning, -1);

	//std::shared_ptr<Pi3Cmusic> music = pi3c.resource.addMusic(opts.asString("musicPath"), "ActionStrike.ogg");
	//std::shared_ptr<Pi3Csound> sound_laser = pi3c.resource.addSound(opts.asString("soundPath"), "alien_laser_1.ogg");
	//std::shared_ptr<Pi3Csound> sound_engineRunning = pi3c.resource.addSound(opts.asString("soundPath"), "afterburner_1.ogg");
	//std::shared_ptr<Pi3Csound> sound_cockpit = pi3c.resource.addSound(opts.asString("soundPath"), "cockpit_interior_rumble.ogg");

	//music->play(-1);
	//music->volume(120);
	//sound_engineRunning->play(1,-1);
	//sound_engineRunning->volume(60);

	vec3f shipRot;
	float rollDelta = 0;
	float takeoff = 0;
	float maxspeed = 30.f;
	player.flyspeed = 0;

	vec3f prot{ 0,0,0 };

	int mx = 100, my = 100;
	Pi3Cimgui &gui = pi3c.gui;	//expose gui object from pi3c

	if (winopts.fullscreen) SDL_WarpMouseInWindow(pi3c.window.handle(), 100, 100);

	while (pi3c.is_running())
	{
		//process key presses ...

		//player.moveKeys(SDL_SCANCODE_W, SDL_SCANCODE_S, SDL_SCANCODE_A, SDL_SCANCODE_D, SDL_SCANCODE_R, SDL_SCANCODE_F);
		const uint8_t *keystate = pi3c.window.getKeys();
		player.run(keystate[SDL_SCANCODE_LSHIFT] || keystate[SDL_SCANCODE_RSHIFT]);
		if (keystate[SDL_SCANCODE_W]) player.flyspeed += 0.2f; // player.forward();
		if (keystate[SDL_SCANCODE_S]) player.flyspeed -= 0.2f;  //player.back();
		if (keystate[SDL_SCANCODE_R]) player.up();			//only when flying
		if (keystate[SDL_SCANCODE_F]) player.down();		//only when flying
		if (keystate[SDL_SCANCODE_C]) { ship = cockpit; Mix_PlayChannel(1, sound_cockpit, -1); shipRot.x = 0; }
		if (keystate[SDL_SCANCODE_V]) { ship = sship; Mix_PlayChannel(1, sound_engineRunning, -1); }
		//if (keystate[SDL_SCANCODE_RETURN]) bolts.fireBolt(-player.getPosition(), player.getRotation());

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

		float ts = 1.f / (float)(pi3c.window.getTicks());
		//if (pi3c.window.mouse.RightButton) {
			//vec3f rotDelta(-pi3c.window.mouse.deltaXY.y / 300.f, pi3c.window.mouse.deltaXY.x / 300.f, 0);
		vec3f rotDelta(-(float)(100 - my) / (50000.f*ts), (float)(100 - mx) / (50000.f*ts), 0);
		prot += rotDelta;
		player.rotate(prot);
		shipRot -= vec3f(prot.x, prot.y, -prot.y);
		//}

		// process window events ...
		Pi3Cmatrix rmat; rmat.rotate(-player.getRotation());
		vec3f offset = (ship == cockpit) ? vec3f(0, 0, 0) : -rmat.transformRotateVec(vec3f(0, shipbox.height()*1.5f, shipbox.depth()*1.5f));
		bool moving = false;

		while (pi3c.do_events()) {
			switch (pi3c.window.ev.type)
			{
			case SDL_MOUSEMOTION:
				SDL_GetMouseState(&mx, &my);
				moving = true;

				//if (pi3c.window.mouse.RightButton) {
				//	vec3f rotDelta(pi3c.window.mouse.deltaXY.y / 300.f, -pi3c.window.mouse.deltaXY.x / 300.f, 0);
				//	player.rotate(rotDelta);
				//	shipRot -= vec3f(rotDelta.x, rotDelta.y, -rotDelta.y);
				//}
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (pi3c.window.mouse.LeftButton) {
					Mix_PlayChannel(0, sound_laser, 0);
					bullets.fire(&pi3c.scene, -player.getPosition()+ offset, player.getRotation(), player.flyspeed);
					//sound_laser->play(0, 0);
				}
			case SDL_MOUSEWHEEL:
				break;
			case SDL_WINDOWEVENT:
				break;
			}
		}
		if (winopts.fullscreen) SDL_WarpMouseInWindow(pi3c.window.handle(), 100, 100);

		shipRot.z *= 0.9f;			//dampened roll to 0
		prot = prot * 0.96f;
		pi3c.model(ship)->move(-player.getPosition()+offset);
		pi3c.model(ship)->matrix.setRotate(shipRot);
		//if (shipRot.z != 0) shipRot.z *= 0.95f;			//dampened roll to 0
		//if (shipRot.x != 0 && ship == sship) shipRot.x *= 0.7f;			//dampened pitch to 0

		//clear background ...
		pi3c.clear_window();
	
		//Render 3D scene ...
		pi3c.scene.setMatrix(player.getPosition(), vec3f(0, 0, 0), player.getRotation());
		pi3c.scene.setSun(0xffffff, vec3f(15000.f, 15000.f, -15000.f)); //transform sun position into scene
		pi3c.render3D();
		
		//Render 2D
		bullets.animate(&pi3c.scene, pi3c.window.getTicks());
		aliens.animate(&pi3c.resource, &pi3c.scene, landRef, pi3c.window.getTicks());
		aliens.hit(&pi3c.resource, &pi3c.scene, bullets);

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
