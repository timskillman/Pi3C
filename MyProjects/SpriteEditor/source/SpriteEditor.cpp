#include "Pi3C.h"

int main(int argc, char *argv[])
{
	Pi3C pi3c("SpriteEditor", 800, 600);

	//Setup your scene here ...
	
	while (pi3c.is_running())
	{
		pi3c.clear_window();

		//Draw your scene here ...

		pi3c.swap_buffers();
	}

	return 0;
}
