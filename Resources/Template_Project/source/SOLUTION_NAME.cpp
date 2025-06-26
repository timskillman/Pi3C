#include <Pi3C.h>

#include SCREEN_WIDTH 800
#include SCREEN_HEIGHT 600

int main(int argc, char *argv[])
{
	Pi3C pi3c("PROJECT_NAME", SCREEN_WIDTH, SCREEN_HEIGHT);

	//Setup your scene here ...
	
	while (pi3c.is_running())
	{
		pi3c.clear_window();

		//Draw your scene here ...

		pi3c.update_time();
		pi3c.swap_buffers();
	}

	return 0;
}
