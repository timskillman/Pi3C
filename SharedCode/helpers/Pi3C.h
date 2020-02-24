#pragma once

#include "Pi3Cwin.h"
#include "Pi3Cresource.h"
#include "Pi3Cscene.h"
#include "Pi3Cimgui.h"
#include "Pi3Cmodel.h"
#include "Pi3Ccolours.h"
#include "Pi3Cshapes.h"
#include <vector>

// ==========================================================================
// Pi3C Graphics Library - Pi3C wrapper (by Tim Skillman)
// ==========================================================================
//
// The MIT License
//
// Copyright (c) 2018 Tim Skillman
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// Dedicated to the One who has blessed me beyond my dreams ...
// Jesus Christ, King of Kings and Lord of Lords :-)
// =======================================================================


class Pi3C {
public:
	Pi3C();
	Pi3C(const Pi3Cwindow::options& winopts);
	Pi3C(const std::string, const uint32_t width = 800, const uint32_t height = 600, const bool fullscreen = false);

	~Pi3C() 
	{ 
		window.destroy(); 
	}

	void init(const Pi3Cwindow::options& winopts);
	void newWindow(const std::string &title, const uint32_t width, const uint32_t height, const bool fullscreen = false);

	Pi3Cmodel create_model_from_mesh(const Pi3Cmesh &mesh, const uint32_t colour = 0xffffff) { return Pi3Cmodel(&resource, mesh, colour); }
	Pi3Cmodel create_model_from_text(const std::string &text, const uint32_t width, const uint32_t colour = 0x303030);

	uint32_t create_background(const std::string &path, const std::string &file = "");
	int32_t load_model(const std::string &path, const std::string &file, const vec3f &pos = vec3f(0,0,0));
	int32_t load_model_and_collider(const std::string& path, const std::string& model, const std::string& collider, const vec3f& pos = vec3f(0, 0, 0));
	int32_t add_model_to_scene2D(const Pi3Cmodel &model) { return scene.append2D(model,""); }
	int32_t add_model_to_scene3D(Pi3Cmodel &model) { return scene.append3D(model,""); }
	//int32_t add_spriteArray(Pi3CspriteArray &spritearray, const std::string &file = "");

	std::vector<float> * getMeshVerts(const uint32_t meshRef) { return &resource.vertBuffer[resource.meshes[meshRef].bufRef]; }
	uint32_t getMeshVertPtr(const uint32_t meshRef) { return resource.meshes[meshRef].vertOffset*resource.meshes[meshRef].stride; }

	//void update_sprite_position(const uint32_t spritesRef, const uint32_t spriteRef, const float x, const float y);
	//void update_sprite_rotated(const uint32_t spritesRef, const uint32_t spriteRef, const vec2f &pos, const vec2f &size, const float angle);
	//void update_sprite_transform(const uint32_t spritesRef, const uint32_t spriteRef, const vec3f &pos, const vec2f &size, const Pi3Cmatrix *scene_matrix, const vec2f &cent);
	//void update_sprite_billboards(const uint32_t spritesRef, const uint32_t spriteRef, const uint32_t count, const vec3f &lookat);

	int32_t create_cuboid(const vec3f& pos, const vec3f& size, const uint32_t colour, const std::string& texfile = "")
	{
		Pi3Cmodel cuboid(&resource, Pi3Cshapes::cuboid(pos, size), colour);
		return scene.append3D(cuboid, texfile);
	}

	int32_t create_cylinder(const vec3f& pos, const float radius, const float height, const uint32_t colour, const std::string& texfile = "")
	{
		Pi3Cmodel cylinder(&resource, Pi3Cshapes::cylinder(pos, radius, height), colour);
		return scene.append3D(cylinder, texfile);
	}

	int32_t create_cone(const vec3f& pos, const float radius, const float height, const uint32_t colour, const std::string& texfile = "")
	{
		Pi3Cmodel cone(&resource, Pi3Cshapes::cone(pos, radius, height), colour);
		return scene.append3D(cone, texfile);
	}

	int32_t create_tube(const vec3f& pos, const float inner_radius, const float outer_radius, const float height, const uint32_t colour, const std::string& texfile = "")
	{
		Pi3Cmodel tube(&resource, Pi3Cshapes::tube(pos, inner_radius, outer_radius, height), colour);
		return scene.append3D(tube, texfile);
	}

	int32_t create_sphere(const vec3f& pos, const float radius, const uint32_t colour, const std::string& texfile = "")
	{
		Pi3Cmodel sphere(&resource, Pi3Cshapes::sphere(pos, radius), colour);
		return scene.append3D(sphere, texfile);
	}

	int32_t create_extrude(vec3f& pos, std::vector<std::vector<float>>& floatPath, float thickness, uint32_t colour, const std::string& texfile = "") {
		Pi3Cmodel extrude(&resource, Pi3Cshapes::extrude("", pos, floatPath, thickness), colour);
		return scene.append3D(extrude, texfile);
	}

	void render3D() { 
		scene.render3D(window.getTicks()); 
	}

	void render2D() { 
		scene.render2D(window.getTicks());
	}

	bool is_running();
	bool do_events();
	std::vector<uint32_t> get_events();

	void clear_window(uint32_t clearColour = 0xff800000)
	{ 
		if (clearColour != window.clearColour) window.setClearColour(clearColour);
		window.clear(); 
	}

	void resize_window();

	void swap_buffers() { 
		frames++; fps++; 
		window.SwapBuffers(); 
		done_events = false; 
	}

	float getAverageFPS();
	float getCurrentFPS();
	void showFPS();

	uint32_t width() { return winw; }
	uint32_t height() { return winh; }

	std::vector<std::string> get_dropfiles();

	Pi3Cmodel * model(const uint32_t modelRef) { return &scene.models[modelRef]; }
	//Pi3CspriteArray * getSpriteArray(const uint32_t spriteArrayRef) { return (Pi3CspriteArray*)(&resource.meshes[spriteArrayRef]); }

	//Pi3Cwindow::options winopts;
	static Pi3Cwindow window;
	static Pi3Cresource resource;
	Pi3Cscene scene;

	Pi3Cimgui gui;
	std::vector<std::string> guifonts;

	Pi3Cimgui::rectStyle bsMenu;

private:
	std::vector<uint32_t> eventList;
	bool done_events = false;

	bool has_started = false;
	uint32_t frames = 0;
	uint32_t fps = 0;
	uint32_t start_time = 0;
	uint32_t last_time = 0;
	float lastFPS = 0;
	uint32_t winw = 0;
	uint32_t winh = 0;
};
