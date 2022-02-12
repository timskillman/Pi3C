#include "Pi3CfileScene.h"

Pi3CfileScene::Pi3CfileScene()
{

}

void Pi3CfileScene::save(const std::string& filename, Pi3Cscene& scene)
{
	//vec3f ppos = player.getPosition();
	//vec3f prot = player.getRotation();;

	Pi3Cjson json;

	json.StartObject();		//<<<There must be a root object surrounding everything

	json.writeString("title", filename.c_str());
	//json.writeVec3f("grid", grid);

	//json.Key("player");
	//json.StartObject();
	//json.writeVec3f("position", ppos);
	//json.writeVec3f("rotation", prot);
	//json.EndObject();

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

	json.saveFile((filename + ".scene").c_str());
}