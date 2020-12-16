#include "Pi3Cshader.h"
#include <fstream>
#include <sstream>

#define RGB_VEC4(col) vec4f((float)(col & 255) / 255.f, (float)((col >> 8) & 255) / 255.f, (float)((col >> 16) & 255) / 255.f, (float)((col >> 24) & 255) / 255.f)

std::string loadShader(const std::string &file)
{
	std::ifstream strfile(file.c_str(), std::ios::binary);
	std::ostringstream os;
	if (strfile) os << strfile.rdbuf(); else return "";
	return os.str();
}

std::string getAttributes(const std::string &vertShaderSource)
{
	std::string attributes;
	int32_t esv = vertShaderSource.find("#version 3");
	std::string attkey = (esv >= 0) ? "in " : "attribute";
	int32_t i = vertShaderSource.find(attkey);
	while (i > 0) {
		int j = vertShaderSource.find(";", i + 1);
		std::string attrib = vertShaderSource.substr(i, j - i);
		int k = attrib.rfind(" ", j);
		if (k >= 0) attributes += attrib.substr(k + 1, attrib.size()) + " ";
		i = vertShaderSource.find(attkey, j);
	}
	return attributes;
}

std::string Pi3Cshader::create(const char *vertShaderFile, const char *fragShaderFile)
{
	std::string error = "";
	std::string attribStr = "";
	std::string vertShaderSource = loadShader(vertShaderFile);
	vertshader = loadShaderStr(GL_VERTEX_SHADER, vertShaderSource, error);
	if (error == "") {
		attribStr = getAttributes(vertShaderSource);	//Automatically extract 'attributes' from the vertex shader
		std::string fragShaderSource = loadShader(fragShaderFile);
		fragshader = loadShaderStr(GL_FRAGMENT_SHADER, fragShaderSource, error);
	}
	if (error == "")
		CreateShaderProgram(vertshader, fragshader, attribStr, error);

	glUseProgram(program); //shader must be used before getting attribute refs in setup
	//setupShader();

	return error;
}

bool Pi3Cshader::CreateShaderProgram(const GLuint &vertexShader, const GLuint &fragmentShader, const std::string &attribStr, std::string &error)
{
	program = glCreateProgram();
	if (program == 0) {
		error = "Could not create shader program";
		return false;
	}

	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	std::istringstream ss(attribStr);
	std::string s; uint32_t a = 0;
	while (ss >> s) glBindAttribLocation(program, a++, s.c_str());
	stride = DEFAULT_STRIDE;

	glLinkProgram(program); //linking

	GLint linked;				//error checking
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (!linked) {
		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
		std::string log; log.resize(infoLogLength);
		glGetProgramInfoLog(program, infoLogLength, NULL, &log[0]);
		error = "Shader linker failure: "+log;
		//SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Shader Error!", "Shader linker failure", handle);

		//SDL_Log("Shader linker failure: %s\n", log.c_str());
		glDeleteProgram(program);
		return false;
	}
	else
	{
		//SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Shader Compiled!", "Shader linked sucessfully!", handle);
		SDL_Log("Shader linked sucessfully!");
	}

	glDetachShader(program, vertexShader);
	glDetachShader(program, fragmentShader);

	SDL_Log("Shader program %d\n", program);

	SDL_Log("glError: %d\n", glGetError());

	return true;
}


//GLuint Pi3Cshader::loadShaderFile2(const char *strShaderFile, std::string &error)
//{
//	std::ifstream sfile(strShaderFile, std::ios::in | std::ios::binary);
//	std::string shaderSource;
//	sfile.seekg(0, std::ios::end);
//	shaderSource.resize(sfile.tellg());
//	sfile.seekg(0, std::ios::beg);
//	sfile.read(&shaderSource[0], shaderSource.size());
//	sfile.close();
//
//	return loadShaderStr(ShaderType, shaderSource, error);
//}

GLuint Pi3Cshader::loadShaderStr(GLenum ShaderType, const std::string &shaderSource, std::string &error)
{
	GLuint shader = glCreateShader(ShaderType);
	const char *ss = shaderSource.c_str();
	glShaderSource(shader, 1, &ss, NULL);

	glCompileShader(shader);

	GLint compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		std::string log; log.resize(infoLogLength);
		glGetShaderInfoLog(shader, infoLogLength, NULL, &log[0]);
		error = "Compile failure in shader: "+log;
		glDeleteShader(shader);
		return -1;
	}
	else
	{
		SDL_Log("Shader compiled sucessfully!");
	}
	return shader;
}

GLint Pi3Cshader::activateTexture(GLuint texID, const char * uni, GLuint texActive)
{
	GLint texh = GetUniformLocation(uni);
	if (texh >= 0) {
		glActiveTexture(GL_TEXTURE0 + texActive);
		glBindTexture(GL_TEXTURE_2D, texID);
		//glUniform1i(texh, texActive);
	}
	return texh;
}

GLint Pi3Cshader::setTexture(const char * uni, GLint texLoc)
{
	GLint texh = GetUniformLocation(uni);
	if (texh >= 0) glUniform1i(texh, texLoc);
	return texh;
}

void Pi3Cshader::setFog(float minDist, float maxDist, vec3f colour) 
{
	Set3f(fogColourRef, colour);
	Setf(fogRangeRef, 1.f / (maxDist- minDist));
	Setf(fogMaxRef, maxDist);
	fogMaxDist = maxDist;
	fogMinDist = minDist;
}

void Pi3Cshader::setSun(vec3f sundir, uint32_t suncol)
{
	Set3f(lightPosRef, sundir);
	Set4f(lightColRef, RGB_VEC4(suncol));
}

void Pi3Cshader::setupShader()
{
	//Set this AFTER shader.Use
	
	// Basic rendering handles
	illuminationModelRef = GetUniformLocation("u_illuminationModel");
	fogColourRef = GetSet3f("u_fogColour", fogColour);
	fogRangeRef = GetSetf("u_fogRange", 1.f / (fogMaxDist - fogMinDist));
	fogMaxRef = GetSetf("u_fogMaxDist", fogMaxDist);
	specularRef = GetUniformLocation("u_specularColour");
	diffuseRef = GetUniformLocation("u_diffuseColour");
	ambientRef = GetUniformLocation("u_ambientColour");
	emissiveRef = GetUniformLocation("u_emissiveColour");
	texAnimRef = GetUniformLocation("u_animoffset");
	lightPosRef = GetSet3f("u_LightPos", lightPos);
	lightColRef = GetUniformLocation("u_lightColour");
	textureRef = GetUniformLocation("u_Texture");
	reflectRef = GetUniformLocation("u_reflective");

	// Basic matrices ..
	perspectiveMatrixRef = GetUniformLocation("u_ProjMatrix");
	modelMatrixRef = GetUniformLocation("u_ModelMatrix");
	
	// Setup texture location
	if (textureRef >= 0) glUniform1i(textureRef, 0);

	Pi3Cmatrix defModelMatrix;
	SetModelMatrix(defModelMatrix); //give it a default model matrix
	setSun(vec3f(1000.f, 8000.f, 1000.f), 0xffffff);
}

void Pi3Cshader::setColDiffuse(uint32_t colour)
{
	Set4f(diffuseRef, RGB_VEC4(colour));
}

void Pi3Cshader::setColAmbient(uint32_t colour)
{
	Set4f(ambientRef, RGB_VEC4(colour));
}

void Pi3Cshader::setColEmissive(uint32_t colour)
{
	Set4f(emissiveRef, RGB_VEC4(colour));
}

void Pi3Cshader::setMaterial(Pi3Cmaterial *mat)
{
	//Animate texture
	//===============
	//Uses a sprite sheet scanning along the X axis first
	//and steps down the Y axis at the end of each X scan
	//Maximum sheet size is 2048x2048 pixels on the RPi

	Set2f(texAnimRef, mat->animoffset);
	mat->AnimateTexture(ticks);

	if (mat == lastMaterial && !mat->changed) return;
	lastMaterial = mat;
	mat->changed = false;

	//if (lastMat == mat.name) {
	//	return;
	//}

	//Set Specular colour
	if (mat->illum == 1) {
		Setf(fogRangeRef, 1.f); //no fog
	}
	else {
		Setf(fogRangeRef, 1.f / (fogMaxDist - fogMinDist));
	}
	Setf(fogMaxRef, fogMaxDist);

	Set4f(specularRef, mat->colSpecular);
	Set4f(ambientRef, mat->colAmbient);
	Set4f(emissiveRef, mat->colEmissive); //vec4f(1.f, 0.4f, 0.0f, 1.f)); //, 
	Seti(illuminationModelRef, mat->illum);
	Seti(reflectRef, mat->reflective);
		
	//Set diffuse colour
	mat->colDiffuse.w = mat->alpha;
	if (mat->alpha == 1.0f) glDisable(GL_BLEND); else  glEnable(GL_BLEND);
	Set4f(diffuseRef, mat->colDiffuse);
		
	if (mat->cullface) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
	if (mat->texRef>=0) {
		//if (lastTex!=0) glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mat->texID);

		//lastTex = 0;
	}
	else {
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	//if (mat.texBumpRef>=0) {
		//if (lastTex!=1) 
		//glActiveTexture(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE_2D, material.texBumpID);
		//lastTex = 1;
		//glActiveTexture(GL_TEXTURE0);
	//}
	//lastMat = mat.name;
}
