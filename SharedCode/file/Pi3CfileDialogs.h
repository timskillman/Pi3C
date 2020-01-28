#pragma once

#include "Pi3Cdirectory.h"
#include "Pi3Cwin.h"
#include "Pi3Cresource.h"
#include "Pi3Cimgui.h"

namespace Pi3CfileDialogs
{

	class FolderItem {
	public:

		FolderItem(Pi3Cresource * resource, Pi3Cwindow *window) { init(resource, window); }


		void OpenFileDialog(const char* path = nullptr)
		{
			
			std::vector<std::string> dirlist = Pi3Cdirectory::readDirectory(path);

		}

	private:

		void init(Pi3Cresource * resource, Pi3Cwindow *window) { 
			this->resource = resource; 
			this->window = window;
			gui.init(resource, window);
		}

		void drawOpenDialog();

		std::string path;
		std::string filename;

		Pi3Cresource *resource = nullptr;
		Pi3Cwindow *window = nullptr;
		Pi3Cimgui gui;
	};


}