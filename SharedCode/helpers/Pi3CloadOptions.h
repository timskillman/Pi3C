#pragma once

#include "Pi3Cvector.h"
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>

// =======================================================================
// Pi3C Raspberry Pi Graphics Library
// =======================================================================
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

class loadOptions {
public:

	loadOptions(const char * file);
	loadOptions() {}

	std::vector<std::string> asStringArray(const std::string &id) const
	{
		const auto &optit = options.find(id);
		if (optit != options.end()) return optit->second;
		return empty; //not found - just return vector with empty string
	}

	std::string asString(const std::string &id) const
	{
		return asStringArray(id)[0];
	}

	float asBool(std::string id) const
	{
		return asString(id)=="true";
	}

	int asInt(std::string id) const
	{
		return std::stoi(asString(id));
	}

	float asFloat(std::string id) const
	{
		return std::stof(asString(id));
	}

	vec3f asVec3f(std::string id) const
	{
		vec3f v;
		std::stringstream ss(asString(id));
		ss >> v.x >> v.y >> v.z;
		return v;
	}

	vec2f asVec2f(std::string id) const
	{
		vec2f v;
		std::stringstream ss(asString(id));
		ss >> v.x >> v.y;
		return v;
	}

private:

	std::map<std::string, std::vector<std::string>> options;
	std::vector<std::string> empty = { "" };

	void load(const char * file);
	
};

