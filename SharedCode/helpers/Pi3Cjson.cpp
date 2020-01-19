#include "Pi3Cjson.h"

Pi3Cjson::Pi3Cjson() 
{
	json_string_buffer.Clear();
	json_writer.Reset(json_string_buffer);
	json_writer.SetFormatOptions(kFormatSingleLineArray);
}

Pi3Cjson::Pi3Cjson(const char * file)
{
	std::ifstream ifs(file);
	IStreamWrapper isw(ifs);
	doc.ParseStream(isw);
}

int Pi3Cjson::readInt(const Value& doc, const char * key)
{
	if (!doc.HasMember(key)) return 0;
	return doc[key].GetInt();
}

void Pi3Cjson::readInt2(const Value& doc, const char * key, int32_t &v1, int32_t &v2)
{
	if (!doc.HasMember(key)) return;
	const Value& a = doc[key];
	if (a.Size() != 2) return;
	v1 = a[0].GetInt();
	v2 = a[1].GetInt();
}
void Pi3Cjson::readInt3(const Value& doc, const char * key, int32_t &v1, int32_t &v2, int32_t &v3)
{
	if (!doc.HasMember(key)) return;
	const Value& a = doc[key];
	if (a.Size() != 3) return;
	v1 = a[0].GetInt();
	v2 = a[1].GetInt();
	v3 = a[2].GetInt();
}

bool Pi3Cjson::readBool(const Value& doc, const char * key)
{
	if (!doc.HasMember(key)) return false;
	return doc[key].GetBool();
}

uint32_t Pi3Cjson::readHex(const Value& doc, const char * key)
{
	if (!doc.HasMember(key)) return 0;
	std::string v = doc[key].GetString();
	return std::stoi(v, 0, 16);
}

float Pi3Cjson::readFloat(const Value& doc, const char * key)
{
	if (!doc.HasMember(key)) return 0;
	return doc[key].GetFloat();
}

void Pi3Cjson::readFloat2(const Value& doc, const char * key, float& v1, float& v2)
{
	if (!doc.HasMember(key)) return;
	const Value& a = doc[key];
	if (a.Size() != 2) return;
	v1 = a[0].GetFloat();
	v2 = a[1].GetFloat();
}

vec3f Pi3Cjson::readVec3f(const Value& doc, const char * key)
{
	if (!doc.HasMember(key)) return vec3f(0, 0, 0);
	const Value& a = doc[key];
	if (a.Size() != 3) return vec3f(0, 0, 0);
	return vec3f((float)a[0].GetDouble(), (float)a[1].GetDouble(), (float)a[2].GetDouble());
}

Pi3Cmatrix Pi3Cjson::readMatrix(const Value& doc, const char * key)
{
	Pi3Cmatrix matrix;
	if (!doc.HasMember(key)) return matrix;
	const Value& a = doc[key];
	if (a.Size() != 12) return matrix;
	matrix.set(a[0].GetDouble(), a[1].GetDouble(), a[2].GetDouble(), 0,
		a[3].GetDouble(), a[4].GetDouble(), a[5].GetDouble(), 0,
		a[6].GetDouble(), a[7].GetDouble(), a[8].GetDouble(), 0,
		a[9].GetDouble(), a[10].GetDouble(), a[11].GetDouble(), 1.f);
	return matrix;
}

std::string Pi3Cjson::readString(const Value& doc, const char * key) {
	return (doc.HasMember(key)) ? doc[key].GetString() : "";
}

double Pi3Cjson::formatDbl(float v)
{
	return std::floor((double)v * 10000) / 10000;
}

void Pi3Cjson::writeString(const char * key, const char * str)
{
	json_writer.Key(key);
	json_writer.String(str);
}

void Pi3Cjson::writeVec3f(const char * key, vec3f& v)
{
	json_writer.Key(key);
	json_writer.StartArray();
	json_writer.Double(formatDbl(v.x));
	json_writer.Double(formatDbl(v.y));
	json_writer.Double(formatDbl(v.z));
	json_writer.EndArray();
}

void Pi3Cjson::writeMatrix(const char * key, Pi3Cmatrix& matrix)
{
	json_writer.Key(key);
	json_writer.StartArray();
	const float* mv = matrix.get();
	json_writer.Double(formatDbl(mv[matrix.m00]));
	json_writer.Double(formatDbl(mv[matrix.m01]));
	json_writer.Double(formatDbl(mv[matrix.m02]));
	json_writer.Double(formatDbl(mv[matrix.m10]));
	json_writer.Double(formatDbl(mv[matrix.m11]));
	json_writer.Double(formatDbl(mv[matrix.m12]));
	json_writer.Double(formatDbl(mv[matrix.m20]));
	json_writer.Double(formatDbl(mv[matrix.m21]));
	json_writer.Double(formatDbl(mv[matrix.m22]));
	json_writer.Double(formatDbl(mv[matrix.m30]));
	json_writer.Double(formatDbl(mv[matrix.m31]));
	json_writer.Double(formatDbl(mv[matrix.m32]));
	json_writer.EndArray();
}

bool Pi3Cjson::saveFile(const char * file) {
	std::ofstream jsonfs(file, std::ofstream::out);
	if (jsonfs.fail()) return false;
	jsonfs << json_string_buffer.GetString() << std::endl;
	jsonfs.close();
	return true;
}