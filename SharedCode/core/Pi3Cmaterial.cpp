#include "Pi3Cmaterial.h"

//----- Pi3Cmaterial -----------------------------------------------------------------------------------------------------------------

Pi3Cmaterial::Pi3Cmaterial()
{
	init("");
}

void Pi3Cmaterial::init(const std::string &name)
{
	colAmbient = { 0.05f,0.05f,0.05f,1.0f };
	colDiffuse = { 1.0f,1.0f,1.0f,1.0f };
	colSpecular = { 0.1f,0.1f,0.1f,1.0f };
	colEmissive = { 0.0f,0.0f,0.0f,0.0f };
	shininess = 0.2f;
	alpha = 1.0f;
	alphaTestValue = 0.f; //no test
	texName = ""; 
	texRef = -1; 
	cullface = true; 
	illum = 2; 
	texID = 0;
	texBumpName = ""; 
	texBumpID = -1;
	animvec = vec2f(0.f,0.f); animoffset = vec2f(0.f,0.f);
	animsize = vec2f(1.f, 1.f);
	animFrames = 0;
	animFrameCount = 0;
	reflective = 0;
	this->name = name;
	rendermode = GL_TRIANGLES;
	frameticks = 0;
	framespeed = 1.f;
}

void Pi3Cmaterial::SetColAmbient(const uint32_t colour)
{
	colAmbient = vec4f((GLfloat)(colour & 255) / 255.f, (GLfloat)((colour >> 8) & 255) / 255.f,(GLfloat)((colour >> 16) & 255) / 255.f, (GLfloat)((colour >> 24) & 255) / 255.f);
	changed = true;
}

void Pi3Cmaterial::SetColAmbient(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a)
{
	colAmbient = vec4f((GLfloat)r / 255.f, (GLfloat)g / 255.f,(GLfloat)b / 255.f, (GLfloat)a / 255.f);
	changed = true;
}

void Pi3Cmaterial::SetColEmissive(const uint32_t colour)
{
	colEmissive = vec4f((GLfloat)(colour & 255) / 255.f, (GLfloat)((colour >> 8) & 255) / 255.f,(GLfloat)((colour >> 16) & 255) / 255.f, (GLfloat)((colour >> 24) & 255) / 255.f);
	changed = true;
}

void Pi3Cmaterial::SetColEmissive(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a)
{
	colEmissive = vec4f((GLfloat)r / 255.f, (GLfloat)g / 255.f,(GLfloat)b / 255.f, (GLfloat)a / 255.f);
	changed = true;
}

void Pi3Cmaterial::SetColDiffuse(const uint32_t colour)
{
	colDiffuse = vec4f((GLfloat)(colour & 255) / 255.f, (GLfloat)((colour >> 8) & 255) / 255.f,(GLfloat)((colour >> 16) & 255) / 255.f, (GLfloat)((colour >> 24) & 255) / 255.f);
	changed = true;
}

void Pi3Cmaterial::SetColDiffuse(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a)
{
	colDiffuse = vec4f((GLfloat)r / 255.f, (GLfloat)g / 255.f,(GLfloat)b / 255.f, (GLfloat)a / 255.f);
	changed = true;
}

void Pi3Cmaterial::SetColDiffuse(const vec4f &col)
{
	colDiffuse = col;
	changed = true;
}

void Pi3Cmaterial::SetColSpecular(const uint32_t colour)
{
	colSpecular = vec4f((GLfloat)(colour & 255) / 255.f, (GLfloat)((colour >> 8) & 255) / 255.f,(GLfloat)((colour >> 16) & 255) / 255.f, (GLfloat)((colour >> 24) & 255) / 255.f);
	changed = true;
}

void Pi3Cmaterial::SetColSpecular(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a)
{
	colSpecular = vec4f((GLfloat)r / 255.f, (GLfloat)g / 255.f,(GLfloat)b / 255.f, (GLfloat)a / 255.f);
	changed = true;
}

void Pi3Cmaterial::SetAnimationTexture(const float stepx, const float stepy)
{
	animvec = vec2f(stepx, stepy);
	animFrames = 0;
	animoffset = vec2f(0.f, 0.f);
}

void Pi3Cmaterial::SetAnimationSheet(const int divx, const int divy, const int frames)
{
	animvec = vec2f(1.f / divx, 1.f / divy);
	animFrames = frames;
	animoffset = vec2f(0.f, animvec.y);
}

void Pi3Cmaterial::AnimateTexture(const float ticks)
{

	if (animFrames > 0) {
		frameticks += ticks;
		if (frameticks > framespeed) {
			frameticks = 0;
			animoffset.x += animvec.x;
			if (animoffset.x > 0.999f) {
				animoffset = vec2f(0.f, animoffset.y + animvec.y);
				//if (animoffset.y > 0.999f) animoffset.y = animvec.y;
			}
			animFrameCount = (animFrameCount + 1) % animFrames;
			if (animFrameCount == 0) animoffset = vec2f(0.f, animvec.y);
		}
	}
	else {
		animoffset += animvec * ticks;
		if (animoffset.x > 0.999f) animoffset.x = 0.f;
		//animoffset = vec2f(0.f, animoffset.y + animvec.y);
		if (animoffset.y > 0.999f) animoffset.y = 0.f;
		//}
	}
}
