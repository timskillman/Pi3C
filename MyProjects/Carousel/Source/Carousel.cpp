#include "Pi3C.h"
#include "Pi3Cdirectory.h"

int main(int argc, char *argv[])
{
	Pi3C pi3c("SpriteAnimator", 1600, 900, true);

	//Load images from textures directory ...
	vector<string> images = Pi3Cdirectory::readDirectory("Textures");
	for (string image : images) {
		if (image.find(".png") || image.find(".jpg")) {
			pi3c.add_background("Textures/" + image);
		}
	}

	int seconds = 1000; //milliseconds
	int delay = 5 * seconds;
	int switchTime = pi3c.getMilliseconds() + delay;

	pi3c.showBackground(0);

	while (pi3c.is_running())
	{
		pi3c.clear_window();

		if (pi3c.getMilliseconds() > switchTime) {
			pi3c.nextBackground();
			switchTime = pi3c.getMilliseconds() + delay;
		}

		pi3c.render2D();

		pi3c.swap_buffers();
	}

	return 0;
}
