#ifndef _cAssimpMeshLoader_HG_
#define _cAssimpMeshLoader_HG_

#include <Animation\Assimp\scene.h>
#include <Mesh\sModelDrawInfo.h>
#include <map>
#include <Mesh\cVaoMeshManager.h>

struct sAnimationInfo
{
	std::string friendlyName;
	std::string fileName;
	Assimp::Importer* pImporter;
	const aiScene* pAIScene;
};

class cAssimpMeshLoader
{
public:
	cAssimpMeshLoader();
	~cAssimpMeshLoader();

	glm::mat4 AIMatrixToGLMMatrix(const aiMatrix4x4 &mat);

	bool SetBasePath(std::string basePath, std::string& pathToSet); // Path for the model files to be found. Returns false if path doesn't exists

	// Loads a mesh
	bool LoadMeshFromFile(const std::string& filename, sModelDrawInfo& meshInfo);
	bool LoadMeshAnimation(const std::string& filename, const std::string& friendlyName);
	bool LoadAssimpDrawInfo(sModelDrawInfo& meshInfo, unsigned int modelIndexInScene = 0);
	bool ConvertAssimpToVAO(cVaoMeshManager* pVaoMeshManager, sModelDrawInfo& meshInfo, GLuint shaderProgramID);

	bool FindAnimationByName(const std::string& animationName, sAnimationInfo& animationInfo);

	// Loads the bones
	bool LoadBones(const aiMesh* mesh, sSkinnedModelInfo* meshInfo);

	void GetListOfBoneIDandNames(std::vector<std::string>& vecBoneNames);

private:
	// Creates a meshDrawInfo, sets it and returns it by value.
	// returns false if any error happened.
	bool _InitializeBones(const aiScene* pScene, sModelDrawInfo& meshInfo, sSkinnedModelInfo* skinnedMeshInfo);
	
	void _Cleanup();

	std::string _baseModelPath;
	std::string _baseAnimationPath;

	// Map of aiScenes by model name
	//std::map<std::string/*Model name*/, aiScene*/*Ai scene*/> _mapAiSceneByModelName;
	std::map< std::string /*Friendly name*/, sAnimationInfo/*Animations*/> mapAnimationsByName; // Animations
	
};



#endif // !_cAssimpMeshLoader_HG_
