#include <Pi3C.h>

#define SCREEN_WIDTH 800	// The screen width
#define SCREEN_HEIGHT 600	// The screen height

int main(int argc, char *argv[])
{
	Pi3C pi3c("My Project", SCREEN_WIDTH, SCREEN_HEIGHT); // Set up Pi3C using constructor

	// Setup your scene here...
	vec3f cubePosition = vec3f(0, 0, 0); // Position of the cube
	vec3f camPosition = vec3f(0, 0, -2); // Position of the camera

	int32_t cube_id = pi3c.create_cuboid(cubePosition, vec3f(1.0f, 1.0f, 1.0f), Pi3Ccolours::Red); // Create a red 3D cube
	Pi3Cmodel* cube = &pi3c.scene.models.at(cube_id); // Get a reference to the cube's model from its ID

	pi3c.scene.setPerspective3D(SCREEN_WIDTH, SCREEN_HEIGHT, 800, 1, 100);	// Setup 3D perspective
	pi3c.scene.setFixedLight(Pi3Ccolours::White, vec3f(15.f, 15.f, 15.f));	// Transform sun position into scene
	pi3c.scene.setMatrix(camPosition, vec3f(), vec3f()); 					// Set the camera position without translating or rotating the scene

	float angle = 0; // Set the rotation angle for the cube to 0
	
	while (pi3c.is_running())
	{
		pi3c.clear_window(Pi3Ccolours::SkyBlue); // Clear the screen with a sky blue color

		// Set cube rotation and animate the angle
		cube->matrix.setRotate(vec3f(angle, angle, 0)); // Set the new rotation of the cube using the angle
		angle += 1.0f * pi3c.deltaTime; // Animate the rotation angle for the cube smoothly using deltaTime

		// Render the scene
		pi3c.render3D();
		pi3c.render2D();

		pi3c.showFPS(); // Show the FPS

		pi3c.swap_buffers(); // Swap back buffer with front buffer to show result
	}

	return 0;
}
