#pragma once
#include "rapidjson.h"
#include "prettywriter.h"
#include "stringbuffer.h"
#include "document.h"
#include "Pi3Cvector.h"
#include "Pi3Cmatrix.h"
#include <istreamwrapper.h>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace rapidjson;

class Pi3Cjson {
public:

	Pi3Cjson();
	Pi3Cjson(const char * file);

	vec3f readVec3f(const Value& doc, const char * key);
	Pi3Cmatrix readMatrix(const Value& doc, const char * key);

	void writeString(const char * key, const char * str);
	void writeVec3f(const char * key, vec3f& v);
	void writeMatrix(const char * key, Pi3Cmatrix& matrix);

	void Key(const char * key) { json_writer.Key(key); }
	void StartObject() { json_writer.StartObject(); }
	void EndObject() { json_writer.EndObject(); }
	void StartArray() { json_writer.StartArray(); }
	void EndArray() { json_writer.EndArray(); }
	bool saveFile(const char * file);

	Document doc;

private:

	double formatDbl(float v);

	StringBuffer json_string_buffer;
	PrettyWriter<StringBuffer> json_writer;
	
	
};