#pragma once

#include "Pi3Cmatrix.h"
#include "Pi3Cmodel.h"
#include <vector>

class Pi3Cundo {
public:

	enum ucom { UD_NONE, UD_MOVE, UD_SCALE, UD_ROTATE, UD_TRANS, UD_CREATE, UD_DELETE, UD_GROUP, UD_MATERIAL };

	void getSelections(std::vector<Pi3Cmodel> &models, std::vector<bool> &selections);
	void setSelections(std::vector<Pi3Cmodel> &models, std::vector<bool> &selections);
	void getTransSelections(std::vector<Pi3Cmodel> &models, std::vector<bool> &selections);
	void setTransSelections(std::vector<Pi3Cmodel> &models, std::vector<bool> &selections);
	void setMoveSelections(std::vector<Pi3Cmodel> &models, const vec3f& trans);
	void deleteSelections(std::vector<Pi3Cmodel> &models);
	void undeleteSelections(std::vector<Pi3Cmodel> &models);

	ucom undoCom = UD_NONE;
	vec3f move;								//used for tracking how selected objects are translated
	vec3f scale;							//used for tracking how selected objects are scaled
	vec3f rotate;							//used for tracking how selected objects are rotated
	std::vector<Pi3Cmatrix> matrices;		//used for tracking how selected objects are transformed
	std::vector<bool> preSelections;
	std::vector<bool> postSelections;
};

class Pi3Cundos {
public:

	void undo(std::vector<Pi3Cmodel> &models);
	void redo(std::vector<Pi3Cmodel> &models);

	void log(const Pi3Cundo::ucom com);
	void logPreSelection(std::vector<Pi3Cmodel> &models);							//Creates a new undo log and logs the selections BEFORE the action takes place
	void logPostSelection(std::vector<Pi3Cmodel>& models);							//Logs the selections AFTER the action has taken place
	void logCreate(std::vector<Pi3Cmodel>& models);									//Logs the selected models AFTER they are created (no other models should be selected)
	void logTransforms(std::vector<Pi3Cmodel>& models, const Pi3Cmatrix& matrix);	
	void logMove(std::vector<Pi3Cmodel>& models, const vec3f& trans);
	void logScale(std::vector<Pi3Cmodel>& models, const vec3f& scale);

private:

	void stopMoving();
	std::vector<Pi3Cundo> undos;
	uint32_t currentUndo = 0;
	bool moving = false;
};