#include "Pi3Cstring.h"

std::string nextString(std::string &str, uint32_t offset)
{
	std::string strcopy = str;
	int32_t s = str.find_first_not_of(' ', offset);
	int32_t e = str.find_first_of(' ', s + 1);
	std::string val = (e<0) ? str.substr(s, str.size() - s) : str.substr(s, e - s);
	str = (e<0) ? "" : str.substr(e);
	return val;
}

void insertDefs(std::map<std::string, std::string> &defs, std::string &val)
{
	int32_t s = val.find('<');
	while (s >= 0) {
		int32_t e = val.find('>');
		std::string defname = val.substr(s + 1, e - s - 1);
		std::map<std::string, std::string>::iterator defInMap = defs.find(defname);
		std::string insertStr = "";
		if (defInMap != defs.end()) insertStr = defInMap->second;
		val = val.substr(0, s) + insertStr + val.substr(e + 1, val.size() - e - 1);
		s = val.find('<');
	}
}

std::string cleanStr(std::string val) 
{

	//clean trailing comments
	int32_t h = val.find('#');
	if (h >= 0) val = val.substr(0, h);
	if (val == "") return "";

	//clean leading spaces and tabs
	size_t c = 0;
	while (c < val.size() && (val[c] == ' ' || val[c] == '\t')) c++;
	val = (c > 0) ? val.substr(c, val.size() - c) : val;

	//clean trailing spaces and tabs
	c = val.size();
	while (c>0 && (val[c] == ' ' || val[c] == '\t')) c--;
	val = (c >= 0) ? val.substr(0, c) : val;

	return val;
}

std::string convertTabs(std::string val)
{
	std::string tab = "    ";
	uint32_t tabSize = tab.size() - 1;
	uint32_t c = 0;
	while (c < val.size()) {
		if (val[c] == '\t') {
			val = val.substr(0, c) + tab + val.substr(c + 1, val.size() - c - 1);
			c += tabSize;
		}
		c++;
	}
	return val;
}

void splitParamBlocks(std::string val, std::vector<std::string> &params, char d1, char d2)
{
	int32_t c = val.find(d1);
	while (c >= 0 && (size_t)c<val.size()) {
		int32_t e = val.find(d2, c);
		std::string param = val.substr((size_t)(c + 1), (size_t)(e - c - 1));
		params.push_back(param);
		//uint32_t c = param.find(":");
		//if (c > 0) {
		//	params.push_back(std::make_pair(param.substr(0, c), param.substr(c + 1, param.size() - c - 1)));
	//	}
		c = e + 1;
		c = val.find(d1, c);
	}
}

void paramPair(const std::string &param, std::string &com, std::string &val)
{
	uint32_t c = param.find(":");
	if (c > 0) {
		com = param.substr(0, c);
		val = param.substr(c + 1, param.size() - c - 1);
	}
}

std::string loadString(const std::string &file)
{
	std::ifstream strfile(file.c_str(), std::ios::binary);
	if (strfile) {
		std::ostringstream os;
		os << strfile.rdbuf();
		return os.str();
	}
	return "";
}