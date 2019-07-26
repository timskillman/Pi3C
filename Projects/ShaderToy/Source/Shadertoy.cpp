#include "Pi3Cwin.h"
#include "Pi3CloadOptions.h"
#include "Pi3Cresource.h"

// ==========================================================================
// Pi3C Graphics Library Example - ShaderToy implementation
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
// =======================================================================

void useShader(uint32_t shaderRef, 
	Pi3Cresource* resource, std::vector<int32_t>& shaderRefs,
	int& res, int& iframe, int& itime, int& imouse,
	uint32_t width, uint32_t height)
{
	if (shaderRefs[shaderRef] < 0) return;
	Pi3Cshader& shader = resource->shaders[shaderRefs[shaderRef]];
	shader.Use();

	int texch0 = shader.GetUniformLocation("iChannel0");
	int texch1 = shader.GetUniformLocation("iChannel1");
	int texch2 = shader.GetUniformLocation("iChannel2");
	int texch3 = shader.GetUniformLocation("iChannel3");

	if (texch0 >= 0) glUniform1i(texch0, 0);
	if (texch1 >= 0) glUniform1i(texch1, 1);
	if (texch2 >= 0) glUniform1i(texch2, 2);
	if (texch3 >= 0) glUniform1i(texch3, 3);

	iframe = shader.GetSetf("iFrame", 0);
	itime = shader.GetSetf("iTime", (float)SDL_GetTicks() / 1000.f);
	imouse = shader.GetSet3f("iMouse", vec3f(0, 0, 0));  //x,y,button
	res = shader.GetSet2f("iResolution", vec2f(width, height));
}

int main(int argc, char *argv[])
{
	loadOptions opts("options.txt");

    // Create and initialize a window
	Pi3Cwindow::options winopts;
	winopts.title = opts.asString("title");
	winopts.fullscreen = opts.asBool("fullscreen");
	winopts.width = opts.asInt("screenWidth");
	winopts.height = opts.asInt("screenHeight");
	winopts.clearColour = 0;
	winopts.majorVsn = 3;
	winopts.minorVsn = 0;
	Pi3Cwindow window(winopts);

	int width = winopts.width;
	int height = winopts.height;
	int frame = 0;

	Pi3CGL::showGLinfo();

	// Setup Editor and Resources ...
	Pi3Cresource resource;
	resource.init();
	
	std::vector<std::string> shaderFiles = opts.asStringArray("shader");
	std::vector<int32_t> shaderRefs;
	
	for (size_t s = 0; s < shaderFiles.size(); s++) {
		shaderRefs.push_back(resource.addShader("Shaders/vs1.glsl", shaderFiles[s].c_str()));
	}

	//Setup shader textures ...
	Pi3Ctexture tex1(opts.asString("tex1").c_str());
	Pi3Ctexture tex2(opts.asString("tex2").c_str());
	Pi3Ctexture tex3(opts.asString("tex3").c_str());
	Pi3Ctexture tex4(opts.asString("tex4").c_str());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex1.textureID);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex2.textureID);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, tex3.textureID);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, tex4.textureID);

	
	// Setup shader ..

	int res, iframe, itime, imouse;
	uint32_t shaderSelect = 0;
	useShader(shaderSelect, &resource, shaderRefs, res, iframe, itime, imouse, width, height);
	window.setCaption("Shader: " + shaderFiles[shaderSelect]);

	Pi3Cshader& shader = resource.shaders[shaderRefs[shaderSelect]];
	//shader.Use();

	//int texch0 = shader.GetUniformLocation("iChannel0");
	//int texch1 = shader.GetUniformLocation("iChannel1");
	//int texch2 = shader.GetUniformLocation("iChannel2");
	//int texch3 = shader.GetUniformLocation("iChannel3");

	//if (texch0 >= 0) glUniform1i(texch0, 0);
	//if (texch1 >= 0) glUniform1i(texch1, 1);
	//if (texch2 >= 0) glUniform1i(texch2, 2);
	//if (texch3 >= 0) glUniform1i(texch3, 3);

	//int res = shader.GetSet2f("iResolution", vec2f(width, height));
	//int itime = shader.GetSetf("iTime", (float)SDL_GetTicks() / 1000.f);
	//int imouse = shader.GetSet3f("iMouse", vec3f(0, 0, 0));  //x,y,button


	Pi3Cmesh rectmesh = resource.createRect(vec3f(-1.f, -1.f, 0), vec2f(2.f, 2.f), 0xffffffff, vec2f(0, 0), vec2f(1.f, 1.f));
	int32_t rectmeshref = resource.addMesh(&rectmesh);

	uint32_t frames = 0;
	uint32_t timer = SDL_GetTicks();

	while (!window.hasquit())
	{
		while (window.event()) {
			switch (window.ev.type)
			{
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEMOTION:
				if (imouse>=0) 
					shader.Set3f(imouse, vec3f(window.mouse.x, window.mouse.y, (window.mouse.anyButton()) ? 1.f:0));
				break;
			case SDL_WINDOWEVENT:
				switch (window.ev.window.event) {
				case SDL_WINDOWEVENT_RESIZED:
					if (res>=0) shader.Set2f(res, vec2f(window.getWidth(), window.getHeight()));
					break;
				}
				break;
			case SDL_KEYUP:
				if (window.getKeyUp()==SDL_SCANCODE_SPACE) {
					shaderSelect = (shaderSelect + 1) % shaderRefs.size();
					useShader(shaderSelect, &resource, shaderRefs, res, iframe, itime, imouse, width, height);
					if (res >= 0) shader.Set2f(res, vec2f(window.getWidth(), window.getHeight()));
					frame = 0;
					window.setCaption("Shader: " + shaderFiles[shaderSelect]);
				}
				break;
			}
		}

		window.clear();

		if (itime >= 0) shader.Setf(itime, (float)SDL_GetTicks() / 1000.f);
		if (iframe >= 0) shader.Setf(iframe, (float)frame);
		resource.renderMesh(rectmeshref, GL_TRIANGLES);

		frames++;
		frame++;
		window.SwapBuffers();	
	}

	uint32_t ticks = SDL_GetTicks() - timer;
	float avtime = (float)frames / ((float)ticks / 1000.f);
	SDL_Log("Average FPS(%d, %d) = %f ", ticks, frames, avtime);

	window.destroy();
	return 0;
}

