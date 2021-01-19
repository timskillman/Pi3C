#include "Pi3Cjson.h"
#include "Pi3Ccolours.h"

Pi3Cjson::Pi3Cjson() 
{
	json_string_buffer.Clear();
	json_writer.Reset(json_string_buffer);
	json_writer.SetFormatOptions(rapidjson::kFormatSingleLineArray);
}

Pi3Cjson::Pi3Cjson(const char * file)
{
	std::ifstream ifs(file);
	rapidjson::IStreamWrapper isw(ifs);
	doc.ParseStream(isw);
}

int Pi3Cjson::readInt(const rapidjson::Value& doc, const char * key)
{
	if (!doc.HasMember(key)) return 0;
	return doc[key].GetInt();
}

void Pi3Cjson::readInt2(const rapidjson::Value& doc, const char * key, int32_t &v1, int32_t &v2)
{
	if (!doc.HasMember(key)) return;
	const rapidjson::Value& a = doc[key];
	if (a.Size() != 2) return;
	v1 = a[0].GetInt();
	v2 = a[1].GetInt();
}
void Pi3Cjson::readInt3(const rapidjson::Value& doc, const char * key, int32_t &v1, int32_t &v2, int32_t &v3)
{
	if (!doc.HasMember(key)) return;
	const rapidjson::Value& a = doc[key];
	if (a.Size() != 3) return;
	v1 = a[0].GetInt();
	v2 = a[1].GetInt();
	v3 = a[2].GetInt();
}

bool Pi3Cjson::readBool(const rapidjson::Value& doc, const char * key)
{
	if (!doc.HasMember(key)) return false;
	return doc[key].GetBool();
}

uint32_t readHexVal(const rapidjson::Value& doc, const char* key)
{
	std::string v = doc[key].GetString();
	long n = std::stol(v, nullptr, 16);
	return (uint32_t)n;
}

uint32_t Pi3Cjson::readHex(const rapidjson::Value& doc, const char * key)
{
	if (!doc.HasMember(key)) return 0;
	return readHexVal(doc, key);
}

uint32_t Pi3Cjson::readHexDefault(const rapidjson::Value& doc, const char* key, uint32_t def)
{
	if (!doc.HasMember(key)) return def;
	return readHexVal(doc, key);
}

uint32_t Pi3Cjson::readColour(const rapidjson::Value& doc, const char* key, const uint32_t def)
{
	if (!doc.HasMember(key)) return def;
	return Pi3Ccolours::getPi3Ccolour(doc[key].GetString());
}

float Pi3Cjson::readFloat(const rapidjson::Value& doc, const char * key)
{
	if (!doc.HasMember(key)) return 0;
	return doc[key].GetFloat();
}

void Pi3Cjson::readFloat2(const rapidjson::Value& doc, const char * key, float& v1, float& v2)
{
	if (!doc.HasMember(key)) return;
	const rapidjson::Value& a = doc[key];
	if (a.Size() != 2) return;
	v1 = a[0].GetFloat();
	v2 = a[1].GetFloat();
}

vec2f Pi3Cjson::readVec2f(const rapidjson::Value& doc, const char * key)
{
	if (!doc.HasMember(key)) return vec2f(0, 0);
	const rapidjson::Value& a = doc[key];
	if (a.Size() != 2) return vec2f(0, 0);
	return vec2f((float)a[0].GetDouble(), (float)a[1].GetDouble());

}
vec3f Pi3Cjson::readVec3f(const rapidjson::Value& doc, const char * key)
{
	if (!doc.HasMember(key)) return vec3f(0, 0, 0);
	const rapidjson::Value& a = doc[key];
	if (a.Size() != 3) return vec3f(0, 0, 0);
	return vec3f((float)a[0].GetDouble(), (float)a[1].GetDouble(), (float)a[2].GetDouble());
}

Pi3Cmatrix Pi3Cjson::readMatrix(const rapidjson::Value& doc, const char * key)
{
	Pi3Cmatrix matrix;
	if (!doc.HasMember(key)) return matrix;
	const rapidjson::Value& a = doc[key];
	if (a.Size() != 12) return matrix;
	matrix.set((float)a[0].GetDouble(), (float)a[1].GetDouble(), (float)a[2].GetDouble(), 0,
		(float)a[3].GetDouble(), (float)a[4].GetDouble(), (float)a[5].GetDouble(), 0,
		(float)a[6].GetDouble(), (float)a[7].GetDouble(), (float)a[8].GetDouble(), 0,
		(float)a[9].GetDouble(), (float)a[10].GetDouble(), (float)a[11].GetDouble(), 1.f);
	return matrix;
}

std::string Pi3Cjson::readString(const rapidjson::Value& doc, const char * key) {
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
