#pragma once

#include "Pi3Cundo.h"
#include "Pi3Cmodel.h"

class Pi3Cedit {
public:

	void undo(std::vector<Pi3Cmodel> &models) { undos.undo(models); }
	void redo(std::vector<Pi3Cmodel> &models) { undos.redo(models); }

	void selectModel(std::vector<Pi3Cmodel>& models, Pi3Cmodel *model, const bool multi);
	void selectAll(std::vector<Pi3Cmodel>& models);
	void clearSelections(std::vector<Pi3Cmodel>& models);
	void deleteSelection(std::vector<Pi3Cmodel>& models);
	void create(std::vector<Pi3Cmodel>& models);
	void moveSelections(std::vector<Pi3Cmodel>& models, const vec3f& vec);
	void scaleSelections(std::vector<Pi3Cmodel>& models, const vec3f& vec);
	void rotateSelections(std::vector<Pi3Cmodel>& models, const vec3f& vec);
	void setSelectionCentre(std::vector<Pi3Cmodel>& models);

	Pi3Cbbox3d getSelectionBounds(std::vector<Pi3Cmodel>& models);

	Pi3Cundos undos;
	vec3f centre;
};