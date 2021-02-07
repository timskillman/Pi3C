#include "Pi3CfileScene.h"
#include "Pi3Cutils.h"
#include "Pi3CfileOBJ.h"

Pi3CfileScene::Pi3CfileScene()
{

}

void Pi3CfileScene::save(const std::string& path, const std::string& filename, bool selected, Pi3Cscene& scene)
{
	//vec3f ppos = player.getPosition();
	//vec3f prot = player.getRotation();;
	std::string filepath = Pi3Cutils::filepath(path, filename,"rp3d");
	Pi3Cjson json;

	json.StartObject();		//<<<There must be a root object surrounding everything

	json.writeString("title", filename.c_str());
	json.writeString("filepath", "models/"); // path.c_str());
	//json.writeVec3f("grid", grid);

	json.Key("player");
	json.StartObject();
	json.writeMatrix("matrix", *scene.getModelMatrix());
	//json.writeVec3f("rotation", prot);
	json.EndObject();

	json.Key("scene");
	json.StartArray();
	std::vector<Pi3Cmodel>& models = scene.models;
	for (auto& model : models) {
		if (!model.deleted) {
			json.StartObject();
			json.writeString("name", model.name.c_str());
			json.writeMatrix("matrix", model.matrix);
			json.EndObject();
		}
	}
	json.EndArray();

	json.EndObject();

	json.saveFile(filepath.c_str());
}

void Pi3CfileScene::loadSceneJSON(const std::string& file, Pi3Cscene& scene)
{
	Pi3Cjson json(file.c_str());

	Pi3Cmodel model;
	Pi3Cmatrix matrix;
	std::string objfile;
	std::string filepath = "models/";

	if (!json.doc.IsObject()) return;// model;
	model.name = json.readString(json.doc, "title");
	filepath = json.readString(json.doc, "filepath");

	//grid = json.readVec3f(json.doc, "grid");
	const Value& p = json.doc["player"];
	if (p.HasMember("matrix")) {
		//scene.setSceneMatrix(json.readMatrix(p, "matrix"));
	}
	//if (p.HasMember("position")) player.setPosition(json.readVec3f(p, "position"));
	//if (p.HasMember("rotation")) player.setRotation(json.readVec3f(p, "rotation"));

	const Value& obj = json.doc["objects"];
	if (obj.IsArray()) {
		for (SizeType i = 0; i < obj.Size(); i++) {
			if (obj[i].IsObject()) {
				const Value& ob = obj[i];
				if (ob.HasMember("name")) objfile = ob["name"].GetString();
				if (ob.HasMember("matrix")) matrix = json.readMatrix(ob, "matrix");
				int32_t modelRef = scene.loadModelOBJ(filepath, objfile, vec3f(), true, 1);
				if (modelRef >= 0) {
					scene.models[modelRef].matrix = matrix;
				}
				//Pi3Cmodel* m = findModel(objname);
				//if (m) model.append(resource, *findModel(objname), matrix);
			}
		}
	}

	//return model;
}