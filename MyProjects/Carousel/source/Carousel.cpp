#include "Pi3C.h"

int main(int argc, char *argv[])
{
	Pi3C pi3c("Carousel", 800, 600);

	//Setup your scene here ...
	
	int slide1 = pi3c.add_background("assets/textures/slide1.jpg");
	
	pi3c.showBackground(slide1);
	
	while (pi3c.is_running())
	{
		pi3c.clear_window();

		//Draw your scene here ...
		
		pi3c.render2D();
		
		pi3c.swap_buffers();
	}

	return 0;
}
