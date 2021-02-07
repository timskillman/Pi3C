#pragma once

#define openFBX

#include <string>
#ifdef openFBX
#include "ofbx.h"
#else
#include "fbxdocument.h"
//using namespace fbx;
#endif
#include "Pi3Cscene.h"
#include "Pi3Cresource.h"

#ifndef openFBX
class FBXobjectHeader {
public:

	int version = 7400;
	int FBXheaderVersion = 0;
	int FBXversion = version;
	int EncryptionType = 0;
	std::string CreationTimeStamp = "";
	std::string Creator = "";
	std::string SceneInfo = "";
	//UserData
	int userDataVersion = 0;
	std::string userTitle = "";
	std::string userSubject = "";
	std::string userAuthor = "";
	std::string userKeywords = "";
	std::string userRevision = "";
	std::string userComment = "";
	//Properties70
	std::string documentURL = "";
	std::string documentSrcURL = "";
	//Original
	std::string originalapplicationVendor = "";
	std::string originalapplicationName = "";
	std::string originalapplicationVersion = "";
	std::string originalDateTimeGMT = "";
	std::string originalFilename = "";
	//LastSaved
	std::string lastSaved = "";
	std::string lastSavedapplicationVendor = "";
	std::string lastSavedapplicationName = "";
	std::string lastSavedapplicationVersion = "";
	std::string lastSavedDateTimeGMT = "";

	std::string fileID = "";
	std::string fileCreationTime = "";
	std::string fileCreator = "";
	
	int globalSettingsVersion = 0;
	int gs_upAxis = 0;
	int gs_upAxisSign = 0;
	int gs_frontAxis = 0;
	int gs_frontAxisSign = 0;
	int gs_coordAxis = 0;
	int gs_coordAxisSign = 0;
	int gs_origUpAxis = 0;
	int gs_origUpAxisSign = 0;

	double unitScaleFactor = 1;
	double origUnitScaleFactor = 1;

	uint32_t ambientColor = 0x000000;
	std::string defaultCamera = "";
	int timeMode = 0;
	int timeProtocol = 0;
	int snapOnFrameMode = 0;
	long timeSpanStart = 0;
	long timeSpanStop = 0;
	double customFrameRate = 0;
	std::string timeMarker = "";
	int currentTimeMarker = 0;
};

class FBXmodel {
public:
	int count = 0;
	std::string propertyTemplate = "FbxNode";

	long propId = 0;
	std::string propName = "";
	std::string propType = "";
	int version = 232;

	int QuaternionInterpolate = 0;
	vec3f RotationOffset;
	vec3f RotationPivot;
	vec3f ScalingOffset;
	vec3f ScalingPivot;

	bool TranslationActive = false;
	vec3f TranslationMin;
	vec3f TranslationMax;
	bool TranslationMinX = false;
	bool TranslationMinY = false;
	bool TranslationMinZ = false;
	bool TranslationMaxX = false;
	bool TranslationMaxY = false;
	bool TranslationMaxZ = false;

	int RotationOrder = 0;
	bool RotationSpaceForLimitOnly = false;
	double RotationStiffnessX = 0;
	double RotationStiffnessY = 0;
	double RotationStiffnessZ = 0;
	double AxisLen = 0;

	vec3f PreRotation;
	vec3f PostRotation;
	int RotationActive;
	vec3f RotationMin;
	vec3f RotationMax;
	bool RotationMinX = false;
	bool RotationMinY = false;
	bool RotationMinZ = false;
	bool RotationMaxX = false;
	bool RotationMaxY = false;
	bool RotationMaxZ = false;

	int InheritType = 0;
	bool ScalingActive = false;
	vec3f ScalingMin;
	vec3f ScalingMax;
	bool ScalingMinX = false;
	bool ScalingMinY = false;
	bool ScalingMinZ = false;
	bool ScalingMaxX = false;
	bool ScalingMaxY = false;
	bool ScalingMaxZ = false;

	vec3f GeometricTranslation;
	vec3f GeometricRotation;
	vec3f GeometricScaling;

	double MinDampRangeX = 0;
	double MinDampRangeY = 0;
	double MinDampRangeZ = 0;
	double MaxDampRangeX = 0;
	double MaxDampRangeY = 0;
	double MaxDampRangeZ = 0;

	double MinDampStrengthX = 0;
	double MinDampStrengthY = 0;
	double MinDampStrengthZ = 0;
	double MaxDampStrengthX = 0;
	double MaxDampStrengthY = 0;
	double MaxDampStrengthZ = 0;

	double PreferedAngleX = 0;
	double PreferedAngleY = 0;
	double PreferedAngleZ = 0;

	std::string LookAtProperty = "";
	std::string UpVectorProperty = "";

	bool Show = true;
	int NegativePercentShapeSupport = 1;
	int DefaultAttributeIndex = -1;

	bool Freeze = false;
	bool LODBox = false;

	vec3f LclTranslation;
	vec3f LclRotation;
	vec3f LclScaling;

	double visibility = 1;
	int VisibilityInheritance = 1;

	bool Shading = true;
	std::string Culling = "";

};

class FBXuv {
public:

	std::string uvChannel = "UVChannel_";
	std::string MappingInformationType = "ByPolygonVertex";
	std::string ReferenceInformationType = "IndexToDirect";

	std::vector<float> uv;
	std::vector<int> uvIndex;
};

class FBXmaterial {
public:
	int count = 0;
	std::string propertyTemplate = "FbxSurfacePhong";
	std::string ShadingModel = "Phong";
	bool MultiLayer = false;
	uint32_t EmissiveColor = 0x000000;
	float EmissiveFactor = 1.f;
	uint32_t AmbientColor = 0x000000;
	float AmbientFactor = 1.f;
	uint32_t DiffuseColor = 0x000000;
	float DiffuseFactor = 1.f;
	vec3f Bump;
	float BumpFactor = 1.f;
	vec3f NormalMap;
	uint32_t TransparentColor = 0x000000;
	float TransparentFactor = 1.f;
	uint32_t DisplacementColor = 0x000000;
	float DisplacementFactor = 1.f;
	uint32_t VectorDisplacementColor = 0x000000;
	float VectorDisplacementFactor = 1.f;
	uint32_t SpecularColor = 0x303030;
	float SpecularFactor = 1.f;
	double ShininessExponent = 10;
	uint32_t ReflectionColor = 0x000000;
	float ReflectionFactor = 1.f;
};

class FBXlayerElement {
public:
	int version = 100;
	std::vector<std::string> type;
	std::vector<int> typeIndex;
};

class FBXgeometry {
public:
	int count = 0;
	std::string propertyTemplate = "FbxMesh";
	std::string name = "";
	long id = 0;

	int GeometryVersion = 0;
	int LayerElementNormal = 0;

	uint32_t colour = 0x000000;
	vec3f BBoxMin;
	vec3f BBoxMax;
	bool PrimaryVisibility = true;
	bool CastsShadows = true;
	bool ReceiveShadows = true;

	std::vector<float> vertices;
	std::vector<int> indexes;
	std::vector<int> edges;
	std::vector<float> normals;
	std::vector<float> normalsW;
	std::vector<FBXuv> uvs;
	std::vector<FBXmaterial> materials;
	
	FBXlayerElement layers;
};


class FBXtexture {
public:
	int count = 0;
	std::string propertyTemplate = "FbxFileTexture";

	int TextureTypeUse = 0;
	double TextureAlpha = 1.0;
	int CurrentMappingType = 0;
	int WrapModeU = 0;
	int WrapModeV = 0;
	bool UVSwap = false;
	int PremultiplyAlpha = 0;

	vec3f Translation;
	vec3f Rotation;
	vec3f Scaling;
	vec3f TextureRotationPivot;
	vec3f TextureScalingPivot;
	int CurrentTextureBlendMode = 0;

	std::string UVSet = "default";
	bool UseMaterial = false;
	bool UseMipMap = false;
};
#endif

class Pi3CopenFBX {
public:
	Pi3CopenFBX(const std::string& file);
	void importToScene(Pi3Cresource* resource, Pi3Cscene* scene);

#ifdef openFBX
	ofbx::IScene* g_scene = nullptr;
	const ofbx::IElement* g_selected_element = nullptr;
	const ofbx::Object* g_selected_object = nullptr;
#else
	//Copy 'fbx' files back into 'ThirdParty' folder before use
	fbx::FBXDocument fbxdoc;
#endif
};