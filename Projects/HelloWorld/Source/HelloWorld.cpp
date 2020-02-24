#include "Pi3C.h"

int main(int argc, char *argv[])
{
	Pi3C pi3c;
	pi3c.newWindow("Pi3C", 800, 600);

	// Your setup stuff goes here ...

	while (pi3c.is_running())
	{
		pi3c.clear_window(Pi3Ccolours::SkyBlue);

		// Your rendering stuff goes here ...

		pi3c.swap_buffers();
	}

	return 0;
}
