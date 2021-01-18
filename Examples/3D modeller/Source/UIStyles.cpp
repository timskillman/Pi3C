#include "UIStyles.h"

void UI_Style::LoadUIstyle(const std::string& file)
{
	Pi3Cjson json(file.c_str());

	if (!json.doc.IsObject()) return;

	const Value& gc = json.doc["GUIcolours"];
	backColour = json.readColour(gc,"BackCol", backColour);
	iconColour = json.readColour(gc, "IconCol", iconColour);
	selectColour = json.readColour(gc, "SelectCol", selectColour);
	highlight = json.readColour(gc, "HighlightCol", highlight);
	borderColour = json.readColour(gc, "BorderCol", borderColour);
}