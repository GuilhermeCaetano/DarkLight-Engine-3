#ifndef _SceneHelper_HG_
#define _SceneHelper_HG_

#include <OpenGL\OpenGLHeaders.h>
#include <Scene\cMazeMaker.h>
#include <Mesh\sModelDrawInfo.h>
#include <string>

class cVaoMeshManager;
class cEntityManager;
class cTextureManager;
class cAssimpMeshLoader;


namespace nScene
{
	// Loads simple mesh into the CPU and GPU
	bool LoadOneMeshIntoCPU(const std::string& meshName, void** modeInfo, cVaoMeshManager* pVaoMeshManager);
	bool LoadOneTMeshIntoGPU(void* modelInfo, cVaoMeshManager* pVaoMeshManager, GLuint shaderProgramID);

	// Loads skinned mesh into the CPU and GPU
	bool LoadOneSkinnedMeshIntoCPU(const std::string& meshName, void** modelInfo, cAssimpMeshLoader* pAssimpLoader, cVaoMeshManager* pVaoMeshManager);
	bool LoadOneTSkinnedMeshIntoGPU(void* modelInfo, cVaoMeshManager* pVaoMeshManager, cAssimpMeshLoader* pAssimpLoader, GLuint shaderProgramID);

	// Loads one animation into CPU
	bool LoadOneAnimationIntoCPU(const std::string& animationName, const std::string& friendlyName, cAssimpMeshLoader* pAssimpLoader);

	/*void LoadThreadedMeshesIntoVAO(GLuint shaderProgram, cVaoMeshManager* pVaoMeshManager, cAssimpMeshLoader* pAssimpLoader, cShaderManager* pShaderManager);
	void LoadMeshesIntoVAO(GLuint shaderProgram, cVaoMeshManager* pVaoMeshManager, cAssimpMeshLoader* pAssimpLoader, cShaderManager* pShaderManager);
	void LoadSkinnedMeshesIntoVAO(cAssimpMeshLoader* pAssimpLoader, cVaoMeshManager* pVaoMeshManager, GLuint shaderProgramID);*/

	void LoadTextures(cTextureManager* pTextureManager);
	void LoadTexturesThreaded(cTextureManager* pTextureManager);
	void LoadCubemaps(cTextureManager* pTextureManager);
	void LoadModelsIntoScene(cEntityManager* pEntityManager, cVaoMeshManager* pVaoMeshManager, cAssimpMeshLoader* pAssimpLoader);

	//HACK: For a new scene
	void CreateScene();

	void Print3DMaze(cEntityManager* pEntityManager, cVaoMeshManager* pVaoMeshManager, cMazeMaker* pMaze);
	void Print3DMaze(cEntityManager* pEntityManager, cVaoMeshManager* pVaoMeshManager, cMazeMaker* pMaze, std::vector<glm::vec3>& emptyPositions);
	void Print3DMaze(cEntityManager* pEntityManager, cVaoMeshManager* pVaoMeshManager, cMazeMaker* pMaze, std::vector<glm::vec3>& emptyPositions, std::vector<glm::vec2>& emptyBlocks);
	void GetMazeEmptyPositions(cMazeMaker* pMaze, std::vector<glm::vec3>& emptyPositions);
}


#endif // !_SceneHelper_HG_
