#ifndef _cSceneManager_HG_
#define _cSceneManager_HG_

#include <vector>
#include <map>
#include <string>
#include <OpenGL\OpenGLHeaders.h>
#include <Mesh\sModelDrawInfo.h>
#include <Windows.h>

/*------------------------- SCENE THREAD -----------------------------------------------------
1 - Create a set number of threads to deal with loading needed models
2 - Lock the vector so it can't be altered at the same time
3 - Each thread will:
	- Lock the vector
	- Get the last element of the vector, remove it and unlock the vector
	- Load that named model into the CPU
	- Sleep when there are no more models to be loaded
4 - Tell the manager when all threads are done, so the scene can be created safely
---------------------------------------------------------------------------------------------*/


class cVaoMeshManager;
class cEntityManager;
class cAssimpMeshLoader;
class cSceneManager
{
public:
	cSceneManager(GLuint shaderProgramID, cVaoMeshManager* pVaoMeshManager, cEntityManager* pEntityManager, cAssimpMeshLoader* pAssimpLoader);
	~cSceneManager();

	// Returns the list of names of models that we have to load
	unsigned long long GetCurrentSceneID();

	bool LoadOneMeshIntoCPU(sModelDrawInfo& modelInfo);

	// Creates a scene by ID
	void CreateScene(unsigned long long sceneID);

	// Loads a scene by ID
	void LoadScene(unsigned long long sceneID);

	// Updates the current scene, checking for transitions
	void Update();
	bool GetModelNameToLoad(sModelDrawInfo& modelToLoad);
	bool SetModelReadyForGPU(sModelDrawInfo& modelInfo);

	// Ends a scene
	void UnloadScene();

private:
	// Generates a number of threads to handle model loading from the CPU
	bool _GenerateModelLoadThreads(unsigned int numThreads);

	// Loads the vector with models that need to be loaded
	void _LoadSceneModels(unsigned long long sceneID);

	// Check if a scene already exists
	bool _SceneExists(unsigned long long sceneID);

	void _UpdateScene();

	enum eSceneLoadStates
	{
		WAITING,
		PENDING,
		LOADED_INTO_CPU,
		LOADED_INTO_GPU,
		READY
	};

	unsigned long long _currentSceneID;
	eSceneLoadStates _sceneLoadState;

	std::vector<sModelDrawInfo> _vecModelsToLoad;
	std::vector<sModelDrawInfo&> _vecGPUReadyModels;
	std::map<unsigned long long/*scene ID*/, iScene*/*scene*/> _mapSceneByID;

	cVaoMeshManager* _pVaoMeshManager;
	cEntityManager* _pEntityManager;
	cAssimpMeshLoader* _pAssimpLoader;

	GLuint _shaderProgramID;

	// The critical section to protect the model vector
	CRITICAL_SECTION CS_MeshVecLock;
	CRITICAL_SECTION CS_GPUVecLock;

};


#endif // !_cSceneManager_HG_

