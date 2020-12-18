#pragma once

#include "Pi3Cwin.h"
#include "Pi3Cresource.h"
#include "Pi3Cscene.h"
#include "Pi3Cimgui.h"
#include "Pi3Cmodel.h"
#include "Pi3Ccolours.h"
#include "Pi3Cshapes.h"
#include <vector>
#include <cctype>

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

using namespace std;

class Pi3C {
public:

	//Constructors
	Pi3C();
	Pi3C(const Pi3Cwindow::options& winopts);
	Pi3C(const std::string, const uint32_t width = 800, const uint32_t height = 600, const bool fullscreen = false);

	//Destructors
	~Pi3C();

	//Initializers
	void init(const Pi3Cwindow::options& winopts);
	void newWindow(const std::string &title, const uint32_t width, const uint32_t height, const bool fullscreen = false);

	//Background display functions
	std::vector<int> backgrounds;
	int currentBackground = -1;

	uint32_t add_background(const std::string &path, const std::string &file = "");
	void showBackground(const int backgroundRef);
	int getBackgroundRef() { return currentBackground; }
	int getBackgroundCount() { return backgrounds.size(); }
	void nextBackground();

	//Pi3CspriteArray * getSpriteArray(const uint32_t spriteArrayRef) { return (Pi3CspriteArray*)(&resource.meshes[spriteArrayRef]); }
	//int32_t add_spriteArray(Pi3CspriteArray &spritearray, const std::string &file = "");
	//void update_sprite_position(const uint32_t spritesRef, const uint32_t spriteRef, const float x, const float y);
	//void update_sprite_rotated(const uint32_t spritesRef, const uint32_t spriteRef, const vec2f &pos, const vec2f &size, const float angle);
	//void update_sprite_transform(const uint32_t spritesRef, const uint32_t spriteRef, const vec3f &pos, const vec2f &size, const Pi3Cmatrix *scene_matrix, const vec2f &cent);
	//void update_sprite_billboards(const uint32_t spritesRef, const uint32_t spriteRef, const uint32_t count, const vec3f &lookat);

	//Model functions
	int32_t load_model(const std::string &path, const std::string &file, const vec3f &pos = vec3f(0, 0, 0), const bool addColliderGrid = false);
	int32_t load_model_and_collider(const std::string& path, const std::string& model, const std::string& collider, const vec3f& pos = vec3f(0, 0, 0));
	int32_t add_model_to_scene2D(const Pi3Cmodel &model) { return scene.append2D(model, ""); }
	int32_t add_model_to_scene3D(Pi3Cmodel &model) { return scene.append3D(model, ""); }
	std::vector<float> * getMeshVerts(const uint32_t meshRef) { return &resource.vertBuffer[resource.meshes[meshRef].bufRef].verts; }
	Pi3Cmodel* model3D(const uint32_t modelRef) { return &scene.models[modelRef]; }
	Pi3Cmodel* model2D(const uint32_t modelRef) { return &scene.models2D[modelRef]; }
	uint32_t getMeshVertPtr(const uint32_t meshRef) { return resource.meshes[meshRef].vertOffset*resource.meshes[meshRef].stride; }

	Pi3Cmodel create_model_from_mesh(const Pi3Cmesh &mesh, const uint32_t colour = 0xffffff) { return Pi3Cmodel(&resource, mesh, colour); }
	Pi3Cmodel create_model_from_text(const std::string &text, const uint32_t width, const uint32_t colour = 0x303030);
	int32_t create_rect2D(const vec2f& pos, const vec2f& size, const uint32_t colour, const std::string& texfile = "");
	int32_t create_cuboid(const vec3f& pos, const vec3f& size, const uint32_t colour, const std::string& texfile = "");
	int32_t create_cylinder(const vec3f& pos, const float radius, const float height, const uint32_t colour, const std::string& texfile = "");
	int32_t create_cone(const vec3f& pos, const float radius, const float height, const uint32_t colour, const std::string& texfile = "");
	int32_t create_tube(const vec3f& pos, const float inner_radius, const float outer_radius, const float height, const uint32_t colour, const std::string& texfile = "");
	int32_t create_tcone(const vec3f& pos, const float top_radius, const float bottom_radius, const float height, const uint32_t colour, const std::string& texfile = "");
	int32_t create_sphere(const vec3f& pos, const float radius, const uint32_t colour, const std::string& texfile = "");
	int32_t create_torus(const vec3f& pos, const float radius, const float ring_radius, const uint32_t colour, const std::string& texfile = "");
	int32_t create_extrude(vec3f& pos, std::vector<std::vector<float>>& paths, float thickness, uint32_t colour, const std::string& texfile = "");
	int32_t create_extrude(vec3f& pos, std::vector<std::vector<vec2f>>& paths, float thickness, uint32_t colour, const std::string& texfile = "");
	int32_t create_lathe(const vec3f &pos, std::vector<vec2f> &path, const int edges, uint32_t colour, const std::string& texfile = "");
	int32_t create_spring(const vec3f& pos, const float radius, const float length, const int coils, const float coilradius, const int ringrots, const uint32_t colour, const std::string& texfile = "");
	int32_t create_disk(const vec3f& pos, const float inner_radius, const float outer_radius, const uint32_t colour, const std::string& texfile);
	int32_t create_plane(const vec3f& pos, const vec2f& size, const uint32_t xdivs, const uint32_t ydivs, int direction, const uint32_t colour, const std::string& texfile);
	int32_t create_elevationMap(const vec3f &pos, const vec3f &size, Pi3Ctexture &tex, const uint32_t xdivs, const uint32_t ydivs, const uint32_t colour, const std::string& texfile);

	void addColliderGridToModel(uint32_t modelRef);
	float getFloorHeight(const uint32_t modelRef, const vec3f pos);

	//Render functions
	void render3D() { scene.render3D(window.getTicks()); }
	void render2D() { scene.render2D(window.getTicks()); }

	//Event handlers
	std::vector<uint32_t> get_events() { return eventList; }
	bool is_running(bool doEvents = true);
	void quit();
	bool do_events();
	bool keyPress(char key);
	inline uint32_t getMilliseconds() { return SDL_GetTicks(); }
	std::vector<std::string> get_dropfiles();

	//Report functions
	float getAverageFPS();
	float getCurrentFPS();
	void showFPS();

	//Window functions
	uint32_t width() { return winw; }
	uint32_t height() { return winh; }
	void clear_window(uint32_t clearColour = 0x000000);
	void resize_window();
	void swap_buffers();

	//Static resources, window and scene 
	static Pi3Cwindow window;
	static Pi3Cresource resource;
	Pi3Cscene scene;

	//GUI variables
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
