#include "Pi3Chtml.h"
#include <sstream>

namespace Pi3Chtml {

	void readHTML(char &c, std::string &text, uint32_t &i, Pi3CtextFormat &format)
	{
		uint32_t tsize = text.size();
		if (c == '&') {
			//HTML entities
			std::string ent;
			while (i < tsize && text[i] != ';') ent+=text[++i];
			auto findEntity = entities.find(ent);
			if (findEntity != entities.end()) c = findEntity->second[0]; else c = 0;
		}
		else if (c == '<') {
			//HTML formatting
			bool val = text[i + 1] != '/'; if (!val) i++;
			std::string tag;
			while (i < tsize && text[i] != '>') tag += text[++i];
			tag=tag.substr(0,tag.size()-1)+"   ";
			//auto findEntity = entities.find(tag);

			switch (tag[0]) {
			case 'i': if (tag[1] == 'n') format.insert = val; else format.italic = val;
				break;
			case 'b': format.bold = val; break;
			case 'd': format.deleted = val; break;
			case 'e': format.italic = val; break;
			case 'h': format.scale = (val) ? 1.f + (float)('6' - tag[2]) *0.02f : 1.f; break;
			case 'm': format.marked = val; break;
			case 'p':
				if (!val) format.endJustify = true; else
				if (tag.substr(0, 8) == "p style=") {
					uint32_t j = 7; while (tag[j++] != 34 && j < tag.size());
					uint32_t k = j + 1; while (tag[k++] != 34 && k < tag.size());
					std::string subtag = tag.substr(j, k - j - 1);
					if (subtag.substr(0, 11) == "text-align:") {
						std::string just = subtag.substr(11, subtag.size() - 11);
						just.erase(just.find_last_not_of(" ;\n\r\t") + 1);
						just.erase(0, just.find_first_not_of(" \n\r\t"));
						format.endJustify = false;
						format.justify = 
							(just == "center") ? Pi3CtextFormat::CENTER_JUSTIFY :
							(just == "right") ? Pi3CtextFormat::RIGHT_JUSTIFY : 
							(just == "justify") ? Pi3CtextFormat::FULL_JUSTIFY : 
							Pi3CtextFormat::LEFT_JUSTIFY;
					}
				}
				break;
			case 's':
				switch (tag[1]) {
				case 't': format.bold = val; break;
				case 'm': format.small = val; break;
				case 'u': 
					if (tag[2] == 'b') format.subscript = val; else format.superscript = val;
					format.scale = (val) ? 0.6f : 1.f;
					format.scaleYoffset = (val && format.superscript) ? 0.5f : 0.f;
				}
				break;
			}
			c = 0;
			format.recalc();
		}
	}

	/* Read whole line to determine height etc.. */
	void readAhead(std::string &text, uint32_t &i, Pi3CtextFormat &format)
	{

	}
}
