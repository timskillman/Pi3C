#include "Pi3CfileOBJ.h"
//#include "Pi3Cgizmos.h"
#include <vector>
#include <fstream>
#include <functional>
#include <sstream>
#include "Pi3CfastRead.h"

#define MAXVALS 1000	//vertice block size (increases by this amount if more to read)

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

using namespace Pi3CfastRead;

namespace Pi3CfileOBJ {

	// OBJ load helpers ...

	void addMesh(Pi3Cresource *resource, Pi3Cmesh &mesh, int32_t groupId, const bool addColliderGrid)
	{
		//SDL_Log("Meshes %d,%d,%d,%d,%d",resource->meshes.size(),resource->materials.size(),resource->textures.size(),resource->vertBuffer.size(),resource->currentBuffer);
		mesh.mode = GL_TRIANGLES;
		mesh.bbox.bboxFromVerts(mesh.verts, 0, mesh.vc, mesh.stride);
		if (addColliderGrid) 
			mesh.hasColliderGrid = mesh.createColliderGrid();
		int meshref = resource->addMesh(&mesh, groupId);
		resource->addMeshOutlines(meshref);

	}
	
	void addMaterial(Pi3Cresource* resource, Pi3Cmaterial &objmat)
	{
		if (objmat.texRef < 0) {
			//if no texture, then apply default texture
			objmat.texID = resource->defaultMaterial()->texID;
			objmat.texRef = resource->defaultMaterial()->texRef;
		}
		//else if (objmat.texID >= 0) {
		//	int32_t bpp = resource->textures[objmat.texRef]->GetDepth();
		//}

		for (size_t i = 0; i < resource->materials.size(); i++) {
			if (resource->materials[i].name == objmat.name) return;
		}

		resource->materials.push_back(objmat);
	}

	std::string formatFileString(const std::string &file)
	{
		std::string f = file;
		int i = f.find("\\");
		while (i >= 0) {
			f = f.substr(0, i) + "/" + f.substr(i + 1, f.size() - 1);
			i = f.find("\\");
		}
		return f;
	}

	vec3f readVec3f(std::string& vals)
	{
		static char vs1[128], vs2[128], vs3[128];
		sscanf(vals.c_str(), "%s %s %s", vs1, vs2, vs3);
		return vec3f(mystrtof(vs1),mystrtof(vs2),mystrtof(vs3));
	}
	
	vec2f readVec2f(std::string& vals)
	{
		static char vs1[128], vs2[128];
		sscanf(vals.c_str(), "%s %s", vs1, vs2);
		return vec2f(mystrtof(vs1),mystrtof(vs2));
	}
	
	void getMatLib(const std::string &path, const std::string &name, Pi3Cresource* resource, const uint32_t groupId)
	{
		std::string line;
		Pi3Cmaterial objmat;
		objmat.groupID = groupId; //used to separate groups of materials - e.g. system, models, skyboxes etc..
		//SDL_Log("Loading material '%s'", (path + name).c_str());
		std::ifstream matfile(path + name, std::ifstream::in);
		if (!matfile.is_open()) {
			SDL_Log("*** Material '%s' not found! ***", name.c_str());
			matfile.close();
			return;
		}

		uint32_t ax, ay, frames;
		while (!matfile.eof())
		{
			std::getline(matfile, line);
			//SDL_Log("%s",line.c_str());
			int lo = line.find(" ");
			if (lo > 0) {
				std::string com = line.substr(0, lo);

				if (com != "" && com != "#" && line.size() > 1)
				{
					std::string vals = line.substr(com.size() + 1, line.size() - com.size() - 1);

					if (com == "newmtl") {
						if (objmat.name != "") addMaterial(resource, objmat);
						objmat.init(name + "_" + vals);
					}
					else if (com == "Ka") objmat.colAmbient = vec4f(readVec3f(vals), 1.f);
					else if (com == "Kd") objmat.colDiffuse = vec4f(readVec3f(vals), 1.f);
					else if (com == "Ks") objmat.colSpecular = vec4f(readVec3f(vals), 1.f);
					else if (com == "Ke") objmat.colEmissive = vec4f(readVec3f(vals), 1.f);
					else if (com == "Tr" || com == "d") //
						sscanf(vals.c_str(), "%f", &objmat.alpha);
					else if (com == "illum") {
						sscanf(vals.c_str(), "%d", &objmat.illum);
						if (objmat.illum == 3) {
							objmat.illum = 2;
							objmat.reflective = 1;
						}
					}
					else if (com == "animsheet") {
						sscanf(vals.c_str(), "%d %d %d", &ax, &ay, &frames);
						objmat.SetAnimationSheet(ax, ay, frames);
					}
					else if (com == "animtex") {
						sscanf(vals.c_str(), "%f %f %f %f", &objmat.animsize.x, &objmat.animsize.y, &objmat.animvec.x, &objmat.animvec.y);
						objmat.animFrames = 0;
					}
					else if (com == "map_Kd") {
						objmat.texRef = resource->loadTexture(path, vals);
						if (objmat.texRef >= 0) {
							objmat.texID = resource->getTextureID(objmat.texRef);
							objmat.texName = vals;
							//if (resource->textures[objmat.texRef]->GetDepth() == 4) 
							//	objmat.alpha = 0.999f; //if 32bit, then mark as alpha channel image
						}
					}
					//else if (com == "map_Bump") {
					//	objmat.texRef = resource->loadTexture(path, vals);
					//	if (objmat.texRef >= 0)
					//		objmat.texBumpID = resource->getTextureID(objmat.texRef);
					//		objmat.texBumpName = vals;
					//}
				}
			}
		}
		if (objmat.name != "") addMaterial(resource, objmat);
		matfile.close();
		SDL_Log("Finished loading material");
	}

	///// Load OBJ with Material support //////////////////////////////////////////////////////////////////

	void load(const std::string &path, const std::string &filename, Pi3Cresource *resource, const std::function<void(float)> showProgressCB, const bool asCollider, const bool addColliderGrid, const int32_t groupId, std::string &error)
	{
		int32_t v[64];
		int32_t c;

		std::vector<vec3f> temp_vertices;
		temp_vertices.resize(MAXVALS);
		std::vector<vec3f> temp_normals;
		temp_normals.resize(MAXVALS);
		std::vector<vec2f> temp_uvs;
		temp_uvs.resize(MAXVALS);
		
		uint32_t meshStart = resource->meshCount();
		//uint32_t materialStart = resource->materials.size();

		bool readFaces = false;
		int readType = 0;

		std::string gpath = formatFileString(path);
		gpath = (gpath.size() > 0 && *(gpath.end() - 1) != '/') ? gpath + "/" : gpath;
		const std::string filepath = gpath + filename;

		std::string line;

		vec3f vertex;
		vec2f texcoords;
		vec3f normal;
		uint32_t tv = 0; uint32_t tn = 0; uint32_t tu = 0;
		//uint32_t lineno = 0;
		//uint32_t meshc = 0;
		std::string materialName = "";
		std::string vals = "";
		std::string s = "";
		std::string com = "";
		std::string matlibname = "";
		
		std::vector<Pi3Cmesh> meshModel;	//models are setup by material count

		int32_t materialfound = -1;
		//GLfloat * cols = NULL; //[4] = { 0.2f, 0.2f, 1.f, 1.f };
		uint32_t col = 0xffffffff;
		uint32_t tri = 0;

		uint32_t tm = SDL_GetTicks();
		// Open OBJ file as string ...
		SDL_RWops *rw = SDL_RWFromFile(filepath.c_str(), "r");
		if (rw == NULL) return;

		size_t fsize = (size_t)SDL_RWsize(rw);
		std::string filestr = "";
		filestr.resize(fsize);
		SDL_RWread(rw, &filestr[0], fsize, 1);
		SDL_RWclose(rw);
		
		uint32_t lineno = 0;
		uint32_t ptr = 0;
		while (ptr<filestr.size())
		{
			line = filestr.substr(ptr, filestr.find_first_of('\n', ptr) - ptr);
			ptr += line.size()+1;

			lineno++;
			if ((lineno % 1000)==0 && showProgressCB!=nullptr) showProgressCB(((float)ptr / (float)filestr.size())*100.f);

			if (line.length()>1) {

				com = line.substr(0, line.find(" "));
				if (com.size() < line.size())
					vals = line.substr(com.size() + 1, line.size() - com.size() - 1); else vals = "";
				vals.erase(vals.find_last_not_of(" \n\r\t") + 1);
				int j = 0; while (com[j] == ' ') j++;

				switch (com[j]) {
				case 'v':
					if (com.size()-j == 1) {
						if (tv + 1 > temp_vertices.size()) temp_vertices.resize(temp_vertices.size() + MAXVALS);
						temp_vertices[tv] = readVec3f(vals);
						tv++;
					} 
					else {
						switch (com[j + 1]) {
						case 't':
							if (tu + 1 > temp_uvs.size()) temp_uvs.resize(temp_uvs.size() + MAXVALS);
							temp_uvs[tu] = readVec2f(vals);
							tu++;
							break;
						case 'n':
							if (tn + 1 > temp_normals.size()) temp_normals.resize(temp_normals.size() + MAXVALS);
							temp_normals[tn] = readVec3f(vals);
							temp_normals[tn].normalise();
							tn++;
							break;
						}
					}
					break;
				case 'f':
					if (!readFaces) {
						//read slashes to determine read type ...
						int32_t i = vals.find("/", 0);
						if (i < 0)
							readType = 2;  // no slash (verts only)
						else {
							int32_t j = vals.find("/", i + 1);
							if (j == i + 1)
								readType = 1;  // 'verts//uvs'
							else
								readType = (static_cast<int32_t>(vals.find(" ", i + 1)) > j) ? 0 : 3; // 'verts/normals/uvs' else 'verts/normals'
						}
						readFaces = true;
					}

					if (materialfound<0) {
						//SDL_Log("No material found - using default one");
						materialfound = 0;
						meshModel.resize(1);
						meshModel[0].materialRef = 0;
					}

					{
						Pi3Cmesh &mesh = meshModel[materialfound];
						switch (readType) {
						case 0:
							c = sscanf(vals.c_str(), "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", 
								&v[0], &v[1], &v[2], &v[3], &v[4], &v[5], &v[6], &v[7], &v[8], 
								&v[9], &v[10], &v[11], &v[12], &v[13], &v[14], &v[15], &v[16], &v[17],
								&v[18], &v[19], &v[20], &v[21], &v[22], &v[23], &v[24], &v[25], &v[26]);
							if (v[0] < 0) {
								for (int32_t i = 0; i < c; i += 3) { v[i] += tv + 1; v[i + 1] += tu + 1; v[i + 2] += tn + 1; }
							}
							for (int32_t i = 0; i < (c-6); i += 3) {
								mesh.addPackedVert(temp_vertices[v[i + 6] - 1], temp_normals[v[i + 8] - 1], temp_uvs[v[i + 7] - 1], col);
								mesh.addPackedVert(temp_vertices[v[0] - 1], temp_normals[v[2] - 1], temp_uvs[v[1] - 1], col);
								mesh.addPackedVert(temp_vertices[v[i + 3] - 1], temp_normals[v[i + 5] - 1], temp_uvs[v[i + 4] - 1], col);
								tri++;
							}
							break;
						case 1:
							c = sscanf(vals.c_str(), "%d//%d %d//%d %d//%d %d//%d %d//%d %d//%d %d//%d %d//%d %d//%d",
								&v[0], &v[1], &v[2], &v[3], &v[4], &v[5], 
								&v[6], &v[7], &v[8], &v[9], &v[10], &v[11],
								&v[12], &v[13], &v[14], &v[15], &v[16], &v[17]);
							if (v[0] < 0) {
								for (int32_t i = 0; i < c; i += 2) { v[i] += tv + 1; v[i + 1] += tn + 1; }
							}
							for (int32_t i = 0; i < (c-4); i += 2) {
								mesh.addPackedVert(temp_vertices[v[i + 4] - 1], temp_normals[v[i + 5] - 1], vec2f(0, 0), col);
								mesh.addPackedVert(temp_vertices[v[0] - 1], temp_normals[v[1] - 1], vec2f(0, 1.f), col);
								mesh.addPackedVert(temp_vertices[v[i + 2] - 1], temp_normals[v[i + 3] - 1], vec2f(1.f, 0), col);
								tri++;
							}
							tri++;
							break;
						case 2:
							c = sscanf(vals.c_str(), "%d %d %d %d %d %d %d %d %d", &v[0], &v[1], &v[2], &v[3], &v[4], &v[5], &v[6], &v[7], &v[8]); //contains vertices only
							normal = temp_vertices[v[0] - 1].trinormal(temp_vertices[v[1] - 1], temp_vertices[v[2] - 1]); //create a surface normal
							if (v[0] < 0) {
								for (int32_t i = 0; i < c; i++) v[i] += tv + 1;
							}
							for (int32_t i = 0; i < (c-2); i++) {
								mesh.addPackedVert(temp_vertices[v[i + 2] - 1], normal, vec2f(0, 0), col);
								mesh.addPackedVert(temp_vertices[v[0] - 1], normal, vec2f(0, 1.f), col);
								mesh.addPackedVert(temp_vertices[v[i + 1] - 1], normal, vec2f(1.f, 0), col);
								tri++;
							}
							tri++;
							break;
						case 3: //verts,uv's
							c = sscanf(vals.c_str(), "%d/%d %d/%d %d/%d %d/%d %d/%d %d/%d %d/%d %d/%d %d/%d",
								&v[0], &v[1], &v[2], &v[3], &v[4], &v[5],
								&v[6], &v[7], &v[8], &v[9], &v[10], &v[11],
								&v[12], &v[13], &v[14], &v[15], &v[16], &v[17]);
							normal = temp_vertices[v[0] - 1].trinormal(temp_vertices[v[2] - 1], temp_vertices[v[4] - 1]); //create a surface normal
							if (v[0] < 0) {
								for (int32_t i = 0; i < c; i += 2) { v[i] += tv + 1; v[i + 1] += tu + 1; }
							}
							for (int32_t i = 0; i < (c-4); i += 2) {
								mesh.addPackedVert(temp_vertices[v[i + 4] - 1], normal, temp_uvs[v[i + 5] - 1], col);
								mesh.addPackedVert(temp_vertices[v[0] - 1], normal, temp_uvs[v[1] - 1], col);
								mesh.addPackedVert(temp_vertices[v[i + 2] - 1], normal, temp_uvs[v[i + 3] - 1], col);
								tri++;
							}
							tri++;
							break;
						}
					}
					break;
				case 'g':

					break;
				case 'm': //get material library
					if (com == "mtllib") {
						matlibname = vals;
						getMatLib(gpath, matlibname, resource, groupId);
						meshModel.resize(resource->materials.size());
					}
					break;
				case 'u':  //use material
					if (com.substr(0,6) == "usemtl") {
						//Find and set material ...
						materialName = matlibname + "_" + vals;
						materialfound = -1;

						//SDL_Log("Use material: %s, %d", vals.c_str(),materials.size());
						for (size_t i = 0; i < resource->materials.size() && (materialfound < 0); i++) {
							if (materialName == resource->materials[i].name) materialfound = i;
						}

						if (materialfound < 0) {
							SDL_Log("ERROR! - Material '%s' not found", materialName.c_str());
							if (meshModel.size()==0) meshModel.resize(1);
							materialfound = 0; //use default
						}
						else {
							//SDL_Log("Material found %d, %s", materialfound, materialName.c_str());
						}
						if (meshModel.size() > (size_t)materialfound) {
							meshModel[materialfound].materialRef = materialfound; //passed back to model
							if (meshModel[materialfound].name == "") {
								//SDL_Log("Add new mesh: mesh_%s",vals.c_str());
								meshModel[materialfound].name = "mesh_" + vals; //Set new mesh name
							}
						}
						readFaces = false;
					}
					break;
				}
			}
		}

		//SDL_Log("Creating resource ...");
		
		//Add all models with no alpha materials
		for (size_t i=0; i<meshModel.size(); i++) {
			if (meshModel[i].verts.size()>0 && resource->materials[meshModel[i].materialRef].alpha >= 1.0f) {
				addMesh(resource, meshModel[i], groupId, addColliderGrid);
			}
		}
		
		//Lastly, add all models with alpha materials (else z depth of alpha will obscure non-alpha objects)
		//However, overlapping alpha's will obscure each other unless z-depth is turned off
		for (size_t i=0; i<meshModel.size(); i++) {
			if (meshModel[i].verts.size() > 0 && resource->materials[meshModel[i].materialRef].alpha < 1.0f) {
				addMesh(resource, meshModel[i], groupId, addColliderGrid);
			}
		}
		
		SDL_Log("Total time = %f secs, triangles = %d, mesh parts = %d, total = %d", (float)(SDL_GetTicks() - tm) / 1000.f, tri, resource->meshCount() - meshStart, resource->meshCount()); //, bbox.width(),bbox.height(),bbox.depth());

	}

	///// Save OBJ with Material support //////////////////////////////////////////////////////////////////

	void saveMaterial(std::stringstream& ss, const Pi3Cmaterial& material, const std::string& subPath, uint32_t counter)
	{
		ss << "newmtl " + material.name << "_" << counter << "\n";
		ss << "Kd " << material.colDiffuse.x << " " << material.colDiffuse.y << " " << material.colDiffuse.z << "\n";
		ss << "Ks " << material.colSpecular.x << " " << material.colSpecular.y << " " << material.colSpecular.z << "\n";
		ss << "Ka " << material.colAmbient.x << " " << material.colAmbient.y << " " << material.colAmbient.z << "\n";
		ss << "Ke " << material.colEmissive.x << " " << material.colEmissive.y << " " << material.colEmissive.z << "\n";
		ss << "illum " << material.illum << "\n";
		if (material.alpha < 1.f) ss << "Tr " << material.alpha << "\n";
		if (material.texName != "" && material.texName != "default") ss << "map_Kd " << subPath << material.texName << "\n";
		if (material.animvec.x != 0 || material.animvec.y != 0) {
			if (material.animFrames == 0) {
				ss << "animtex " << material.animsize.x << " " << material.animsize.y << " " << material.animvec.x << " " << material.animvec.y << "\n";
			}
			else {
				ss << "animsheet " << (1.f / material.animvec.x) << " " << (1.f / material.animvec.y) << " " << material.animFrames << "\n";
			}
		}
		ss << "\n";
	}

	void saveModel(std::stringstream &ss, std::stringstream &matstr, Pi3Cresource *resource, const Pi3Cmodel &model, const Pi3Cmatrix &mtx, uint32_t &voff, uint32_t &uoff, uint32_t &noff, uint32_t counter, bool saveTriangles)
	{
		Pi3Cmatrix tmtx = model.matrix * mtx;

		if (model.group.size() > 0) {
			for (auto &gmodel : model.group) {
				saveModel(ss, matstr, resource, gmodel, tmtx, voff, uoff, noff, counter, saveTriangles);
			}
		}
		else {
			if (model.meshRef >= 0) {

				saveMaterial(matstr, model.material, "", counter);

				Pi3Cmesh &mesh = resource->meshes[model.meshRef];
				std::vector<float> &verts = resource->vertBuffer[mesh.bufRef].verts;

				if (saveTriangles) {
					
					uint32_t moff = mesh.vertOffset * mesh.stride;

					//Output as triangles only ...
					for (size_t i = moff; i < (moff + mesh.vc); i += mesh.stride) {
						vec3f v = tmtx.transformVec(vec3f(verts[i], verts[i + 1], verts[i + 2]));
						ss << "v " << v.x << " " << v.y << " " << v.z << "\n";
					}

					for (size_t i = moff; i < (moff + mesh.vc); i += mesh.stride) {
						vec3f n = tmtx.transformRotateVec(vec3f(verts[i + 3], verts[i + 4], verts[i + 5]));
						ss << "vn " << n.x << " " << n.y << " " << n.z << "\n";
					}

					for (size_t i = moff; i < (moff + mesh.vc); i += mesh.stride)
						ss << "vt " << verts[i + 6] << " " << verts[i + 7] << "\n";

					//output material reference ...
					ss << "usemtl " << model.material.name << "_" << counter << "\n";

					uint32_t j = 0;
					for (size_t i = 0; i < mesh.vc; i += mesh.stride * 3) {
						ss << "f "
							<< j + voff + 1 << "/" << j + uoff + 1 << "/" << j + noff + 1 << " "
							<< j + voff + 2 << "/" << j + uoff + 2 << "/" << j + noff + 2 << " "
							<< j + voff + 3 << "/" << j + uoff + 3 << "/" << j + noff + 3 << "\n";
						j += 3;
					}

					voff += j;
					uoff += j;
					noff += j;

				}
				else {
					//create shared index mesh ...
					std::vector<uint32_t> vertindexes;
					std::vector<float> newverts; 
					std::vector<uint32_t> normindexes;
					std::vector<float> newnorms;
					std::vector<uint32_t> uvindexes;
					std::vector<float> newuvs;
					mesh.createSharedTriangleList(verts, vertindexes, newverts, normindexes, newnorms, uvindexes, newuvs, 0.001f);

					//output new, transformed vertices ... 
					for (size_t i = 0; i < newverts.size(); i += 3) {
						vec3f v = tmtx.transformVec(vec3f(newverts[i], newverts[i + 1], newverts[i + 2]));
						ss << "v " << v.x << " " << v.y << " " << v.z << "\n";
					}

					//output new, transformed normals ... 
					for (size_t i = 0; i < newnorms.size(); i += 3) {
						vec3f vn = tmtx.transformRotateVec(vec3f(newnorms[i], newnorms[i + 1], newnorms[i + 2]));
						ss << "vn " << vn.x << " " << vn.y << " " << vn.z << "\n";
					}

					//output new uv's
					for (size_t i = 0; i < newuvs.size(); i += 2)
						ss << "vt " << newuvs[i] << " " << newuvs[i + 1] << "\n";

					//output material reference ...
					ss << "\n" << "usemtl " << model.material.name << "_" << counter << "\n";

					//output faces ...
					for (size_t i = 0; i < vertindexes.size(); i += 3) {
						//if (vertindexes[i] != vertindexes[i + 1] && vertindexes[i] != vertindexes[i + 2] && vertindexes[i + 1] != vertindexes[i + 2]) {
							ss << "f " << vertindexes[i] + voff + 1 << "/" << uvindexes[i] + uoff + 1 << "/" << normindexes[i] + noff + 1 << " "
								<< vertindexes[i + 1] + voff + 1 << "/" << uvindexes[i + 1] + uoff + 1 << "/" << normindexes[i + 1] + noff + 1 << " "
								<< vertindexes[i + 2] + voff + 1 << "/" << uvindexes[i + 2] + uoff + 1 << "/" << normindexes[i + 2] + noff + 1 << "\n";
						//}
					}

					voff += newverts.size() / 3;
					noff += newnorms.size() / 3;
					uoff += newuvs.size() / 2;
				}
			
			}
		}
	}

	void save(const std::string &path, const std::string &filename, Pi3Cscene *scene, const bool selected, const std::function<void(float)> showProgressCB, std::string &error)
	{
		const std::string gpath = (path.size() > 0 && *(path.end() - 1) != '/') ? path + "/" : path;
		const std::string filepath = gpath + filename + ".obj";

		std::string matfilename = filename + ".mtl";
		const std::string matpath = gpath + matfilename;

		SDL_RWops *rw = SDL_RWFromFile(filepath.c_str(), "w");
		if (rw == NULL) return;

		std::stringstream ss;
		std::stringstream ssmat;
		Pi3Cmatrix imtx;

		uint32_t voff = 0, uoff = 0, noff = 0;

		ss << "mtllib ./" << matfilename << "\n";
		uint32_t c = 1; //material counter
		for (auto &model : scene->models) {
			if (model.visible && !model.deleted && model.touchable && (!selected || (model.selected && selected))) {
				saveModel(ss, ssmat, scene->resource, model, imtx, voff, uoff, noff, c, false);
				c++;
			}
		}

		//Save OBJ
		std::string filestr = ss.str();
		SDL_RWwrite(rw, &filestr[0], filestr.size(), 1);
		SDL_RWclose(rw);

		//Save MTL
		filestr = ssmat.str();
		SDL_RWops* rwmat = SDL_RWFromFile(matpath.c_str(), "w");
		SDL_RWwrite(rwmat, &filestr[0], filestr.size(), 1);
		SDL_RWclose(rwmat);

	}

}

