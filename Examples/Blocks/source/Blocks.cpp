#include "Pi3C.h"
#include "Pi3Cavatar.h"
#include "BlockMap.h"
#include "Pi3CloadOptions.h"

int main(int argc, char *argv[])
{

	//loadOptions opts("options.txt");
	Pi3Cwindow::options winopts;
	winopts.title = "Blocks"; // opts.asString("title");
	winopts.width = 1920; // opts.asInt("screenWidth");
	winopts.height = 1080; // opts.asInt("screenHeight");
	winopts.antialiasLevel = 0;
	winopts.fullscreen = false; // opts.asBool("fullscreen");
	winopts.perspective = 500;
	Pi3C pi3c(winopts);

	//Pi3C pi3c("Blocks", 800, 600, true);

	//Setup your scene here ...
	int mapSize = 7;
	int hmap = mapSize / 2;
	int chunkSize = 16;
	int chunkHeight = 128;

	BlockMap blockMap(&pi3c.resource, mapSize, chunkSize, chunkSize, chunkHeight);
	blockMap.createMap(-hmap, hmap + 1, -hmap, hmap + 1);
	blockMap.createMapMeshes(&pi3c.resource, &pi3c.scene, -hmap, hmap + 1, -hmap, hmap + 1);

	Pi3Cavatar player;
	//player.movement = Pi3Cavatar::move_fly;
	player.flyspeed = 0;
	player.walkSpeed = 0.5f;
	player.runSpeed = 1.0f;
	player.fallSpeed = 0.5f;

	pi3c.scene.setPerspective3D(800,600,300,0.01f,5000.f);
	int oht = -1e8;
	int ht = chunkHeight/2-5;
	bool tooHigh = false;
	int climbHeight = 19;
	bool jump = false;
	int jumpHeight = 10;

	player.setPosition(vec3f(0, (float)-ht, 0));
	vec3f prot(0, 0, 0);
	vec3f ppos(0, 0, 0);
	vec3f opos(0, 0, 0);
	int mx,my;
	int flip = 0;
	SDL_WarpMouseInWindow(pi3c.window.handle(), 100, 100);
	SDL_ShowCursor(SDL_DISABLE);

	//vec3f chunkCorner(-(mapSize * chunkSize) / 2, 0, -(mapSize * chunkSize) / 2);

	ppos = player.getPosition();
	vec3f offset = -ppos; // -chunkCorner;
	int ocx = (int)(-ppos.x / chunkSize);
	int ocz = (int)(-ppos.z / chunkSize);
	int fallspeed = 5;

	while (pi3c.is_running())
	{
		pi3c.clear_window(Pi3Ccolours::SkyBlue);
		
		player.doKeys();
		
		SDL_GetMouseState(&mx, &my);
		vec3f rotDelta(((float)my - 100.f) / 500.f, -((float)mx - 100.f) / 500.f, 0);
		flip=(flip+1)%2;
		if (flip==0) SDL_WarpMouseInWindow(pi3c.window.handle(), 100, 100);
		
		if (oht != -1e8) prot += rotDelta;
		player.rotate(prot);
		player.updateAndCollide(&pi3c.scene, pi3c.window.getTicks());

		if (!tooHigh) opos = ppos;
		ppos = player.getPosition();

		const Uint8* keystate = SDL_GetKeyboardState(NULL);
		if (!jump && keystate[SDL_SCANCODE_SPACE] && oht==ht) {
			jump = true; 
			jumpHeight = ht - 10;
		}

		if (keystate[SDL_SCANCODE_RETURN]) {
			blockMap.insertBlock(&pi3c.resource, &pi3c.scene, blockType::Diamond, vec3f(-ppos.x, -ht / 10, -ppos.z));
		}

		if (!jump && oht!=ht && oht != -1e8) {
			if (!tooHigh)
				oht += (oht < ht) ? fallspeed : -fallspeed;
			else {
				ht = oht;
			}
		}
		else {
			if (jump && oht != jumpHeight) {
				oht -= fallspeed;
				if (oht == jumpHeight) {
					jump = false;
				}
			}
			ht = blockMap.getGroundHeight(-ppos) * 10;
			//ht = chunkMap.getHeightAtPoint(chunkCorner, -vec3f(ppos.x,ppos.y+1,ppos.z))*10;
			tooHigh = false; // (oht - ht > 19);
			if (oht == -1e8) oht = ht;
		}
		if (tooHigh) ppos = opos;

		player.setPosition(vec3f(ppos.x, (float)oht / 10 - 2, ppos.z));

		//Draw your scene here ...
		pi3c.scene.setSun(0xffffff, vec3f(5000.f, 15000.f, -15000.f)); //transform sun position into scene
		pi3c.scene.setMatrix(player.getPosition(), vec3f(0, 0, 0), player.getRotation());
		pi3c.render3D();
		prot = prot * 0.9f;
		
		pi3c.render2D();
		//pi3c.showFPS();

		pi3c.gui.Begin();
		const std::string fps = "FPS:" + std::to_string((int)pi3c.getCurrentFPS());
		pi3c.gui.Text(fps, 0xffffffff);

		vec3f offset = -ppos; // -chunkCorner;
		int xx = (int)(offset.x + chunkSize * 10000);
		int zz = (int)(offset.z + chunkSize * 10000);
		int cx = xx / chunkSize - 10000;
		int cz = zz / chunkSize - 10000;
		int x = xx % chunkSize;
		int z = zz % chunkSize;

		const std::string cxz = "CX:" + std::to_string(cx) + ", CZ:"+ std::to_string(cz) + ", X:"+ std::to_string(x) + ", Z:" + std::to_string(z);
		pi3c.gui.Text(cxz, 0xffffffff);

		pi3c.gui.End();

		if (ocx != cx && x==7) {
			//create new chunks on X axis ...
			if (ocx < cx) {
				blockMap.createMap(cx + hmap, cx + hmap+1, cz - hmap, cz + hmap+1);
				blockMap.updateMapMeshes(&pi3c.resource, &pi3c.scene, cx + (hmap-1), cx + hmap, cz - (hmap-1), cz + hmap);
			}
			else {
				blockMap.createMap(cx - hmap, cx - hmap -1, cz - hmap, cz + hmap+1);
				blockMap.updateMapMeshes(&pi3c.resource, &pi3c.scene, cx - (hmap-1), cx - (hmap-2), cz - (hmap-1), cz + hmap);
			}
			ocx = cx;
		}

		//if (ocz != cz && z==7) {
		//	//create new chunks on Z axis ...
		//	if (ocz < cz) {
		//		blockMap.createMap(cx - 3, cx + 4, cz + 3, cz + 4);
		//		blockMap.updateMapMeshes(&pi3c.resource, &pi3c.scene, cx - 3, cx + 4, cz + 2, cz + 3);
		//	}
		//	else {
		//		blockMap.createMap(cx - 3, cx + 4, cz - 4, cz - 3);
		//		blockMap.updateMapMeshes(&pi3c.resource, &pi3c.scene, cx - 2, cx + 3, cz - 3, cz - 2);
		//	}
		//	ocz = cz;
		//}

		pi3c.swap_buffers();
	}

	SDL_Log("Average FPS:%f", pi3c.getAverageFPS());
	SDL_ShowCursor(SDL_SYSTEM_CURSOR_ARROW);
	pi3c.window.destroy();
	
	return 0;
}
