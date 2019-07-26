#include "Pi3Cdirectory.h"

namespace Pi3Cdirectory {

	std::vector<std::string> readDirectory(const char * path)
	{
		std::vector<std::string> dirlist;
#ifdef __WINDOWS__

		//based on code from https://docs.microsoft.com/en-gb/windows/desktop/FileIO/listing-the-files-in-a-directory

		WIN32_FIND_DATA ffd;
		HANDLE hFind = INVALID_HANDLE_VALUE;

		std::string spath = path;
		if (spath[spath.size() - 1] != '/') spath += "/";
		spath += "*"; //add wildcard search

		hFind = FindFirstFile(spath.c_str(), &ffd);
		if (hFind != INVALID_HANDLE_VALUE) {
			do {
				std::string item(ffd.cFileName);
				if (item != "." && item != "..") {
					if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) item += "/";
					dirlist.push_back(item);
				}
			} while (FindNextFile(hFind, &ffd));
			FindClose(hFind);
		}
#else
		//based on RPi post https://www.raspberrypi.org/forums/viewtopic.php?t=9338
		DIR * dir = opendir(path);
		if (dir) {
			struct dirent * entry;
			while (entry = readdir(dir)) {
				std::string item(entry->d_name);
				if (item != "." && item != "..") {
					if (entry->d_type == DT_DIR) item += "/";
					dirlist.push_back(item);
				}
			}
			closedir(dir);
		}
#endif
		return dirlist;
	}

}
