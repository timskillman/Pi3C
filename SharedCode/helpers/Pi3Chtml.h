#pragma once
#include <map>
#include <string>
#include "Pi3Ctextformat.h"

namespace Pi3Chtml {
	static std::map<std::string, std::string> entities = { {"amp","&"}, {"lt","<"},{"gt",">"},{"nbsp","\xA0"},{"iexcl","\xA1"} };

	enum htmlTags { ht_html, ht_body, ht_title, ht_head, ht_p, ht_br, ht_pre };
	//static std::map<std::string, htmlTags> tags = { { "html", ht_html}, {"body", ht_body}, {"",""}, {"",""}, {"",""}, {"",""}, {"",""}, {"",""}, {"",""}, {"",""}, {"",""}, {"",""}, {"",""}, {"",""}, {"",""}, {"",""}, {"",""}, {"",""}, {"",""}, {"",""}, {"",""}, {"",""}, {"",""}, {"",""}, {"",""}, {"",""}, {"",""}, {"",""}, {"",""}, {"",""}, {"",""}, {"",""} };

	void readHTML(char &c, const std::string &text, uint32_t &i, Pi3CtextFormat &format);
}
