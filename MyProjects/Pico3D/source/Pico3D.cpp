#include "Pi3C.h"

int main(int argc, char* argv[])
{
	Pi3C pi3c("Pico3D", 800, 600);

	//Setup your scene here ...

	int slide1 = pi3c.add_background("assets/textures/slide1.jpg");

	float pico_width = 51.f;
	float pico_depth = 21.f;

	int pico_model = pi3c.create_cuboid(vec3f(0, 0, 0), vec3f(pico_width, pico_depth, 1), 0xff40a040);  //Board
	pi3c.add_to_model_group(pico_model, pi3c.create_cuboid(vec3f(0, 0, 1), vec3f(7, 7, 1), 0xff202020));  //RP2040
	pi3c.add_to_model_group(pico_model, pi3c.create_cuboid(vec3f(pico_width / 2.f - 1, 0, 1.5), vec3f(5, 8, 2), 0xfff0f0f0));  //Micro USB
	pi3c.add_to_model_group(pico_model, pi3c.create_cuboid(vec3f(14, 4, 1.25), vec3f(4, 3, 1.5), 0xffa0f0f0)); //BOOT button
	pi3c.add_to_model_group(pico_model, pi3c.create_cuboid(vec3f(7.5, 4.2, 0.7f), vec3f(3, 2.4, .6f), 0xff202020));
	pi3c.add_to_model_group(pico_model, pi3c.create_cuboid(vec3f(-9, 2.2, 0.7f), vec3f(2.4, 3.5, .6f), 0xfff0f0f0));  //Crystal
	pi3c.add_to_model_group(pico_model, pi3c.create_cuboid(vec3f(15, -3.2, 0.7f), vec3f(2.8, 2.4, .6f), 0xff202020));

	float pico_tab_width = 1.5f;
	float pico_tab_length = 2.f;

	float pc20 = pico_width / 20;
	float pcwh = pico_width / 2;
	float pcdh = pico_depth / 2;

	for (int i = 0; i < 20; i++)
	{
		pi3c.add_to_model_group(pico_model, pi3c.create_cuboid(vec3f(pc20 * i - pcwh + pc20 / 2, -pcdh + pico_tab_length / 2, 0.6f), vec3f(pico_tab_width, pico_tab_length, 0.2f), 0xff20a0c0));
		pi3c.add_to_model_group(pico_model, pi3c.create_cuboid(vec3f(pc20 * i - pcwh + pc20 / 2, pcdh - pico_tab_length / 2, 0.6f), vec3f(pico_tab_width, pico_tab_length, 0.2f), 0xff20a0c0));
	}

	pi3c.scene.setFixedLight(Pi3Ccolours::White, vec3f(150.f, 150.f, 150.f)); //transform sun position into scene

	//pi3c.showBackground(slide1);

	float roty = 0;

	while (pi3c.is_running())
	{
		pi3c.clear_window();

		//Draw your scene here ...

		pi3c.scene.setMatrix(vec3f(0, 0, 0), vec3f(0, 0, -100.f), vec3f(roty, roty, 0));
		roty += 0.01f;

		pi3c.render2D();
		pi3c.render3D();



		pi3c.swap_buffers();
	}

	return 0;
}
