#include "Pi3C.h"
#include "Pi3Cavatar.h"
#include "BlockMap.h"
#include "Pi3CloadOptions.h"

int main(int argc, char *argv[])
{

	//loadOptions opts("options.txt");
	Pi3Cwindow::options winopts;
	winopts.title = "Blocks"; // opts.asString("title");
	winopts.width = 800; // opts.asInt("screenWidth");
	winopts.height = 600; // opts.asInt("screenHeight");
	winopts.antialiasLevel = 0;
	winopts.fullscreen = true; // opts.asBool("fullscreen");
	winopts.perspective = 300;
	Pi3C pi3c(winopts);

	//Pi3C pi3c("Blocks", 800, 600, true);

	//Setup your scene here ...
	int mapSize = 5;
	int chunkSize = 16;
	int chunkHeight = 128;
	Blocks chunkMap(mapSize, chunkSize, chunkSize, chunkHeight);

	BlockMap blockMap;
	vec3f chunkCorner(-(mapSize * chunkSize) / 2, 0, -(mapSize * chunkSize) / 2);
	blockMap.createMap(&pi3c.resource, &pi3c.scene, &chunkMap, mapSize, chunkCorner);

	Pi3Cavatar player;
	//player.movement = Pi3Cavatar::move_fly;
	player.flyspeed = 0;
	player.walkSpeed = 0.1f;
	player.runSpeed = 0.2f;

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
			vec3f newblock(-ppos.x, -ht / 10, -ppos.z);
			chunkMap.insertBlock(blockType::Diamond, chunkCorner, newblock);
			chunkMap.updateMeshChunk(&pi3c.resource, &pi3c.scene, chunkCorner, newblock);
		}

		if (!jump && oht!=ht && oht != -1e8) {
			if (!tooHigh)
				oht += (oht < ht) ? 1 : -1;
			else {
				ht = oht;
			}
		}
		else {
			if (jump && oht != jumpHeight) {
				oht -= 1;
				if (oht == jumpHeight) {
					jump = false;
				}
			}
			ht = chunkMap.getGroundHeight(chunkCorner, -ppos) * 10;
			//ht = chunkMap.getHeightAtPoint(chunkCorner, -vec3f(ppos.x,ppos.y+1,ppos.z))*10;
			tooHigh = (oht - ht > 19);
			if (oht == -1e8) oht = ht;
		}
		if (tooHigh) ppos = opos;

		player.setPosition(vec3f(ppos.x, (float)oht / 10 - 2, ppos.z));

		//Draw your scene here ...
		pi3c.scene.setSun(0xffffff, vec3f(5000.f, 15000.f, -15000.f)); //transform sun position into scene
		pi3c.scene.setMatrix(player.getPosition(), vec3f(0, 0, 0), player.getRotation());
		pi3c.render3D();
		prot = prot * 0.9f;
		

		pi3c.swap_buffers();
	}

	return 0;
}
