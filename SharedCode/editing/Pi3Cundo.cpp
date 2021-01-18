#include "Pi3Cundo.h"

void Pi3Cundo::getSelections(std::vector<Pi3Cmodel> &models, std::vector<bool> &selections)
{
	selections.resize(0);
	for (auto &model : models) {
		selections.push_back(model.selected && !model.deleted);
	}
}

void Pi3Cundo::setSelections(std::vector<Pi3Cmodel> &models, std::vector<bool> &selections)
{
	for (size_t i = 0; i < selections.size(); i++) {
		models[i].selected = selections[i];
	}
}

void Pi3Cundo::getTransSelections(std::vector<Pi3Cmodel> &models, std::vector<bool> &selections)
{
	selections.resize(0);
	matrices.resize(0);
	for (auto &model : models) {
		selections.push_back(model.selected && !model.deleted);
		if (model.selected) matrices.push_back(model.matrix);
	}
}

void Pi3Cundo::setTransSelections(std::vector<Pi3Cmodel> &models, std::vector<bool> &selections)
{
	size_t j = 0;
	for (size_t i=0; i < selections.size(); i++) {
		if (selections[i]) {
			models[i].selected = true;
			models[i].matrix = matrices[j++];
		}
	}
}

//void Pi3Cundo::getMoveSelections(std::vector<Pi3Cmodel> &models)
//{
//	preSelections.resize(0);
//	for (auto &model : models) {
//		preSelections.push_back(model.selected && !model.deleted);
//		if (model.selected && !model.deleted) moves.push_back(model.matrix.position());
//	}
//}

void Pi3Cundo::setMoveSelections(std::vector<Pi3Cmodel> &models, const vec3f& trans)
{
	uint32_t j = 0;
	for (size_t i = 0; i < preSelections.size(); i++) {
		if (preSelections[i]) models[i].matrix.Translate(trans);
	}
}

void Pi3Cundo::deleteSelections(std::vector<Pi3Cmodel> &models)
{
	for (size_t i = 0; i < preSelections.size(); i++) {
		if (preSelections[i]) models[i].deleted = true;
	}
}

void Pi3Cundo::undeleteSelections(std::vector<Pi3Cmodel> &models)
{
	for (size_t i = 0; i < preSelections.size(); i++) {
		if (preSelections[i]) models[i].deleted = false;
	}
}

///// Pi3Cundos functions ///////////////////////////////////////////////////////////////////////

void Pi3Cundos::undo(std::vector<Pi3Cmodel> &models)
{
	stopMoving();
	if (currentUndo <= 0) return;

	Pi3Cundo &undo = undos[--currentUndo];
	switch (undo.undoCom)
	{
	case Pi3Cundo::UD_CREATE: undo.deleteSelections(models); break;
	case Pi3Cundo::UD_DELETE: undo.undeleteSelections(models); break;
	case Pi3Cundo::UD_MOVE: undo.setMoveSelections(models, -undo.move); break;
	case Pi3Cundo::UD_TRANS: undo.setTransSelections(models, undo.preSelections); break;
	case Pi3Cundo::UD_GROUP: break;
	case Pi3Cundo::UD_MATERIAL: break;
	default:
		break;
	}
}

void Pi3Cundos::redo(std::vector<Pi3Cmodel> &models)
{
	stopMoving();
	if (currentUndo >= undos.size()) return;

	Pi3Cundo &undo = undos[currentUndo++];
	switch (undo.undoCom)
	{
	case Pi3Cundo::UD_CREATE: undo.undeleteSelections(models); break;
	case Pi3Cundo::UD_DELETE: undo.undeleteSelections(models); break; 
	case Pi3Cundo::UD_MOVE: undo.setMoveSelections(models, undo.move); break; //????
	case Pi3Cundo::UD_TRANS: undo.setTransSelections(models, undo.postSelections); break;
	case Pi3Cundo::UD_GROUP: break;
	case Pi3Cundo::UD_MATERIAL: break;
	}
}

///// Pi3Cundos logging functions ////////////////////////////////////////////////////////////////

void Pi3Cundos::stopMoving()
{
	if (moving) {
		moving = false;
	}
}

void Pi3Cundos::log(const Pi3Cundo::ucom com)
{
	stopMoving();
	Pi3Cundo newundo;
	newundo.undoCom = com;
	undos.push_back(newundo);
	currentUndo++;
}

void Pi3Cundos::logPreSelection(std::vector<Pi3Cmodel> &models)
{
	if (currentUndo < 1) return;
	undos[currentUndo - 1].getSelections(models, undos[currentUndo - 1].preSelections);
}

void Pi3Cundos::logPostSelection(std::vector<Pi3Cmodel>& models)
{
	if (currentUndo < 1) return;
	undos[currentUndo-1].getSelections(models, undos[currentUndo - 1].postSelections);
}

void Pi3Cundos::logCreate(std::vector<Pi3Cmodel>& models)
{
	log(Pi3Cundo::UD_CREATE);
	undos[currentUndo - 1].getTransSelections(models, undos[currentUndo - 1].preSelections);
}

void Pi3Cundos::logTransforms(std::vector<Pi3Cmodel>& models, const Pi3Cmatrix& matrix)
{
	log(Pi3Cundo::UD_TRANS);
	undos[currentUndo - 1].getTransSelections(models, undos[currentUndo - 1].preSelections);
}

void Pi3Cundos::logMove(std::vector<Pi3Cmodel>& models, const vec3f& trans)
{
	if (moving) {
		undos[currentUndo - 1].move += trans;
		return;
	}
	log(Pi3Cundo::UD_MOVE);
	logPreSelection(models);
	moving = true;
}

void Pi3Cundos::logScale(std::vector<Pi3Cmodel>& models, const vec3f& scale)
{
	if (moving) {
		undos[currentUndo - 1].scale += scale;
		return;
	}
	log(Pi3Cundo::UD_SCALE);
	logPreSelection(models);
	moving = true;
}

