
#include "Pi3C.h"
#include "Pi3Cshapes.h"
#include "Pi3CfileOBJ.h"
#include "Pi3CloadOptions.h"
#include "Pi3Cavatar.h"

#include <fstream>
#include <sstream>
#include <vector>

// ==========================================================================
// SceneViewer example using the Pi3C Graphics Library
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


int main(int argc, char *argv[])
{
	loadOptions opts("options.txt");
	int screenWidth = opts.asInt("screenWidth");
	int screenHeight = opts.asInt("screenHeight");
	Pi3C pi3c(opts.asString("title"), screenWidth , screenHeight);

	Pi3CGL::showGLinfo();

	Pi3Cavatar player;
	Pi3Cavatar::avatarParams avparams;
	avparams.movement = Pi3Cavatar::move_walk;
	avparams.position = opts.asVec3f("startpos");
	avparams.rotation = opts.asVec3f("rotate");
	avparams.size = { 1.f, opts.asFloat("avatarHeight"), 1.f };
	avparams.walkSpeed = opts.asFloat("avatarWalkSpeed");
	avparams.runSpeed = opts.asFloat("avatarRunSpeed");
	avparams.fallSpeed = opts.asFloat("avatarFallSpeed");
	player.init(avparams);
	
	int skybox = pi3c.scene.loadModelOBJ(opts.asString("skyboxPath"), opts.asString("skybox"), 0); // loadbarCallback);
	pi3c.scene.models[skybox].matrix.SetScale(opts.asFloat("skyboxScale"));
	pi3c.scene.models[skybox].touchable = false;

	int ship = pi3c.load_model_and_collider("../../Resources/models/CargoBay", "CargoHoldBaked3.obj", "CargoHold_CollisionBig.obj");
	pi3c.scene.setFog(0xffffff, opts.asFloat("fogNear"), opts.asFloat("fogFar"));
	pi3c.scene.setPerspective3D(screenWidth, screenHeight, opts.asFloat("perspective"), opts.asFloat("nearz"), opts.asFloat("farz"));


	while (pi3c.is_running())
	{
		//process key presses ...

		player.moveKeys(SDL_SCANCODE_W, SDL_SCANCODE_S, SDL_SCANCODE_A, SDL_SCANCODE_D, SDL_SCANCODE_R, SDL_SCANCODE_F);
		const uint8_t *keystate = pi3c.window.getKeys();
		player.run(keystate[SDL_SCANCODE_LSHIFT] || keystate[SDL_SCANCODE_RSHIFT]);
		//if (keystate[SDL_SCANCODE_W]) player.forward();
		//if (keystate[SDL_SCANCODE_S]) player.back();
		//if (keystate[SDL_SCANCODE_A]) player.left();
		//if (keystate[SDL_SCANCODE_D]) player.right();
		//if (keystate[SDL_SCANCODE_R]) player.up();			//only when flying
		//if (keystate[SDL_SCANCODE_F]) player.down();		//only when flying
		if (keystate[SDL_SCANCODE_SPACE]) player.jump();	//only when walking
		player.updateAndCollide(&pi3c.scene, pi3c.window.getTicks());

		// Centre skybox and other 'infinite' geometry around player...
		//if (skybox >= 0) pi3c.scene.models[skybox].matrix.move(-player.getPosition());

		// process window events ...
		while (pi3c.do_events()) {
			switch (pi3c.window.ev.type)
			{
			case SDL_MOUSEMOTION:
				if (pi3c.window.mouse.RightButton) {
					player.rotate(vec3f(pi3c.window.mouse.deltaXY.y / 300.f, -pi3c.window.mouse.deltaXY.x / 300.f, 0));
				}
				break;
			}
		}

		//clear background ...
		pi3c.clear_window();
	
		//Render 3D scene ...
		pi3c.scene.setSun(0xffffff, vec3f(1000.f, 1000.f, -1000.f)); //transform sun position into scene
		pi3c.scene.setMatrix(player.getPosition(), vec3f(0,0,0), player.getRotation());
		pi3c.render3D();
		
		//Render 2D
		pi3c.render2D();
		pi3c.showFPS();
		pi3c.swap_buffers();
		
	}

	//resource.clearAll();
	SDL_Log("Average FPS = %f ", pi3c.getAverageFPS());

	pi3c.window.destroy();

	return 0;
}
