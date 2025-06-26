#include <Pi3C.h>

int main(int argc, char *argv[])
{
	Pi3C pi3c("Pi3C", 800, 600);

	// Your setup stuff goes here ...
	pi3c.create_cuboid(vec3f(0, 0, 0), vec3f(30, 30, 30), Pi3Ccolours::Green);
	pi3c.scene.setFixedLight(Pi3Ccolours::White, vec3f(150.f, 150.f, 150.f)); //transform sun position into scene

	float rotValue = 0;
	vec3f firstPersonOffset = vec3f(0, 0, 0);
	vec3f thirdPersonOffset = vec3f(0, 0, -100);

	while (pi3c.is_running())
	{
		pi3c.clear_window(Pi3Ccolours::SkyBlue);

		//Set scene matrix and animate
		pi3c.scene.setMatrix(firstPersonOffset, thirdPersonOffset, vec3f(rotValue, rotValue, 0));
		rotValue += 0.03f;

		//Render scene
		pi3c.render3D();

		pi3c.swap_buffers();
	}

	return 0;
}
