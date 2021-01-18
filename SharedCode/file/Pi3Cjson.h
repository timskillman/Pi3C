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

	int readInt(const Value& doc, const char * key);
	void readInt2(const Value& doc, const char * key, int32_t &v1, int32_t &v2);
	void readInt3(const Value& doc, const char * key, int32_t &v1, int32_t &v2, int32_t &v3);
	bool readBool(const Value& doc, const char * key);
	float readFloat(const Value& doc, const char * key);
	void readFloat2(const Value& doc, const char * key, float& v1, float& v2);
	vec2f readVec2f(const Value& doc, const char * key);
	vec3f readVec3f(const Value& doc, const char * key);
	Pi3Cmatrix readMatrix(const Value& doc, const char * key);
	uint32_t readHex(const Value& doc, const char * key);
	uint32_t readHexDefault(const Value& doc, const char* key, uint32_t default);
	uint32_t readColour(const Value& doc, const char* key, const uint32_t default);
	std::string readString(const Value& doc, const char * key);

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