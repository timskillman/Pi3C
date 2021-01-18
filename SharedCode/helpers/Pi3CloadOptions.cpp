#include "Pi3CloadOptions.h"

loadOptions::loadOptions(const char * file)
{
	load(file);
}

void loadOptions::load(const char * file)
{
	std::string line = "";
	std::ifstream opts(file);
	while (std::getline(opts, line)) {
		if (line != ""  && line[0] != '#') {
			std::string com = line.substr(0, line.find(":"));
			if (com.size() > 0) {
				std::string val = line.substr(com.size() + 1, line.size() - com.size() + 1);
				const auto &optit = options.find(com);
				if (optit != options.end()) {
					optit->second.push_back(val);
				}
				else {
					std::vector<std::string> vals;
					vals.push_back(val);
					options.insert({ com, vals });
				}
			}
		}
	}
}
