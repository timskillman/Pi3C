
#include "pico_display.hpp"

namespace pimoroni {

	PicoDisplay::PicoDisplay(uint16_t* buf)
		: PicoGraphics(WIDTH, HEIGHT, buf) {
		__fb = buf;
	}

	PicoDisplay::PicoDisplay(uint16_t* buf, int width, int height)
		: PicoGraphics(width, height, buf) {
		__fb = buf;
	}

	void PicoDisplay::init(Pi3C& pi3c) {
		//pi3c = Pi3C("PicoScreen", 800, 600);
		this->pi3c = pi3c;
		Pi3Ctexture tex = Pi3Ctexture(WIDTH, HEIGHT, 4);
		texref = pi3c.resource.addTexture(tex);
		rectRef = pi3c.create_rect2D(vec2f(0, 0), vec2f(480, 224), 0xffffffff, "assets/textures/pico_display.png");
		dispRef = pi3c.create_rect2D(vec2f(107, 45), vec2f(WIDTH, HEIGHT), 0xffffffff);
		pi3c.scene.models2D[dispRef].assignTexture(&pi3c.resource, texref);
	}

	void PicoDisplay::update(Pi3C& pi3c) {
		//pi3c.do_events();
		pi3c.clear_window();

		Pi3Cmodel& rectModel = pi3c.scene.models2D[rectRef];
		Pi3Cmodel& dispModel = pi3c.scene.models2D[dispRef];
		rectModel.move(vec3f((pi3c.width() - rectModel.bbox.width()) / 2.f, (pi3c.height() - rectModel.bbox.height()) / 2.f, -10));
		dispModel.move(vec3f((pi3c.width() - rectModel.bbox.width()) / 2.f, (pi3c.height() - rectModel.bbox.height()) / 2.f, -5));

		Pi3Ctexture* texture = pi3c.resource.getTexture(texref);
		uint8_t* pixels = texture->GetTexels();

		int32_t size = WIDTH * HEIGHT;
		for (int32_t i = 0; i < size; i++)
		{
			int32_t p = i << 2;
			uint16_t a = frame_buffer[i];
			pixels[p] = (a & 0xF800) >> 8;
			pixels[p + 1] = (a & 0x07E0) >> 3;
			pixels[p + 2] = (a & 0x001F) << 3;
		}
		texture->update();

		pi3c.render2D();

		pi3c.swap_buffers();
	}

	void PicoDisplay::set_backlight(uint8_t brightness) {

	}

	void PicoDisplay::set_led(uint8_t r, uint8_t g, uint8_t b) {

	}

	bool PicoDisplay::is_pressed(uint8_t button) {
		if (button == A && pi3c.keyPress('a')) return true;
		if (button == B && pi3c.keyPress('z')) return true;
		if (button == Y && pi3c.keyPress('/')) return true;
		if (button == X && pi3c.keyPress('\'')) return true;
		return false;
	}

	void PicoDisplay::flip() {

	}

}
