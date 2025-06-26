#include <Pi3C.h>
#include <Pi3CloadOptions.h>
#include <Pi3Cavatar.h>

#include <fstream>
#include <sstream>
#include <vector>

// ==========================================================================
// NoahsArk example using the Pi3C Graphics Library
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
	Pi3C pi3c(opts.asString("title"), screenWidth , screenHeight, opts.asBool("fullscreen"));

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
	
	//Setup player keys
	Pi3Cavatar::navkeys &keys = player.keys;
	keys.forward = "KEY_W";
	keys.back = "KEY_S";
	keys.left = "KEY_A";
	keys.right = "KEY_D";
	keys.up = "KEY_R";
	keys.down = "KEY_F";

	//load models ...
	int skybox = pi3c.scene.loadSkybox(opts.asString("skyboxPath"), opts.asString("skybox"), nullptr, opts.asFloat("skyboxScale"));
	int modelref = pi3c.load_model_and_collider(opts.asString("modelPath"), opts.asString("model"), opts.asString("collider"));

	//setup scene ...
	pi3c.scene.setFog(0xffffff, opts.asFloat("fogNear"), opts.asFloat("fogFar"));
	pi3c.scene.setPerspective3D(screenWidth, screenHeight, opts.asFloat("perspective"), opts.asFloat("nearz"), opts.asFloat("farz"));
	pi3c.scene.setSun(0xffffff, vec3f(1000.f, 1000.f, -1000.f)); //transform sun position into scene

	while (pi3c.is_running())
	{
		//process player key presses and collisions ...
		player.doKeys();
		player.updateAndCollide(&pi3c.scene, pi3c.window.getTicks());

		// Centre skybox and other 'infinite' geometry around player...
		//if (skybox >= 0) pi3c.scene.models[skybox].matrix.move(-player.getPosition());

		// process window events ...
		std::vector<uint32_t> eventList = pi3c.get_events();
		for (auto& ev : eventList) 
		{
			switch (ev) 
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
		pi3c.scene.setMatrix(player.getPosition(), vec3f(0,0,0), player.getRotation());
		pi3c.render3D();
		
		//Render 2D
		pi3c.render2D();
		pi3c.showFPS();
		pi3c.swap_buffers();
		
	}

	return 0;
}
