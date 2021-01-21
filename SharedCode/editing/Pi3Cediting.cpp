#include "Pi3Cediting.h"

void Pi3Cedit::deleteSelection(std::vector<Pi3Cmodel> &models)
{
	undos.log(Pi3Cundo::UD_DELETE);
	undos.logPreSelection(models);
	for (auto &model : models) if (model.selected) model.deleted = true;
}

void Pi3Cedit::selectModel(std::vector<Pi3Cmodel>& models, Pi3Cmodel *selmodel, const bool multi)
{
	//If model already selected then return with other models selected as well
	if (multi) {
		for (auto &model : models) {
			if (&model == selmodel) { selmodel->selected = !selmodel->selected; return; }
		}
	}
	if (selmodel->selected) return;

	//otherwise clear other model selections and select only this one..
	if (!multi) clearSelections(models);
	selmodel->selected = true;
}

void Pi3Cedit::selectAll(std::vector<Pi3Cmodel>& models)
{
	for (auto& model : models) if (model.visible && !model.deleted && model.touchable) model.selected = true;
}

void Pi3Cedit::clearSelections(std::vector<Pi3Cmodel>& models)
{
	for (auto& model : models) if (model.visible && !model.deleted && model.touchable) model.selected = false;
}

void Pi3Cedit::create(std::vector<Pi3Cmodel>& models)
{
	undos.log(Pi3Cundo::UD_CREATE);
	undos.logPreSelection(models);
}

void Pi3Cedit::moveSelections(std::vector<Pi3Cmodel>& models, const vec3f& vec)
{
	undos.logMove(models, vec); 
	for (auto& model : models) {
		if (model.selected && !model.deleted) model.matrix.Translate(vec);
	}
}

void Pi3Cedit::setSelectionCentre(std::vector<Pi3Cmodel>& models)
{
	centre = getSelectionBounds(models).center();
}

Pi3Cbbox3d Pi3Cedit::getSelectionBounds(std::vector<Pi3Cmodel>& models)
{
	Pi3Cbbox3d bounds;
	for (auto& model : models) {
		if (model.selected && !model.deleted) {
			bounds.update(model.bbox, &model.matrix);
		}
	}
	return bounds;
}

void Pi3Cedit::scaleSelections(std::vector<Pi3Cmodel>& models, const vec3f& scale)
{
	undos.logScale(models, scale);
	Pi3Cmatrix scmatrix;
	scmatrix.SetScales((scale / 2.f)+vec3f(1.f,1.f,1.f));
	for (auto& model : models) {
		if (model.selected && !model.deleted) {
			model.matrix.Translate(-centre);
			model.matrix = model.matrix*scmatrix;
			model.matrix.Translate(centre);
		}
	}
}

void Pi3Cedit::duplicateSelection(std::vector<Pi3Cmodel>& models)
{
	undos.log(Pi3Cundo::UD_CREATE);
	for (auto& model : models) {
		if (model.selected && model.visible && !model.deleted) {
			model.selected = false;
			Pi3Cmodel dupMod = model;
			dupMod.selected = true;
			dupMod.matrix.Translate(vec3f(1.f, 0, 0));
			models.push_back(dupMod);
		}
	}
}

void Pi3Cedit::rotateSelections(std::vector<Pi3Cmodel>& models, const vec3f& vec)
{

}
