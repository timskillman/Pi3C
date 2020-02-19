#include "Pi3C.h"


int main(int argc, char *argv[])
{

	Pi3C pi3c("My Pi3C program", 800, 600);

	while (pi3c.is_running())
	{
		pi3c.do_events();
		pi3c.clear_window();

		//Draw your objects here ...

		pi3c.render3D();

		pi3c.swap_buffers();
	}

	pi3c.window.destroy();
	return 0;
}