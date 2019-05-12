#pragma once

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>

/* String handler functions */

std::string nextString(std::string &str, uint32_t offset);
void insertDefs(std::map<std::string, std::string> &defs, std::string &val);
std::string cleanStr(std::string val);
std::string convertTabs(std::string val);
void splitParamBlocks(std::string val, std::vector<std::string> &params, char d1 = '(', char d2 = ')');
void paramPair(const std::string &param, std::string &com, std::string &val);
std::string loadString(const std::string &file);
