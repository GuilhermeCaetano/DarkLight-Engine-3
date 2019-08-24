// cSceneManager.cpp

#include "cSceneManager.h"

#include "cTestScene.h"
#include "nSceneHelper.h"


// The thread function that will do the actual loading of the data
DWORD WINAPI TLoadModel(PVOID pvParam)
{
	// Convert the class back from the void*
	cSceneManager* pManager = static_cast<cSceneManager*>(pvParam);
	sModelDrawInfo modelInfo;

	while (true)
	{
		// Try to get a name to load
		bool meshResult = pManager->GetModelNameToLoad(modelInfo);

		if (!meshResult)
		{
			// Nothing to load, sleep
			
		}
		else
		{
			// Load model into the CPU
			pManager->LoadOneMeshIntoCPU(modelInfo);
		}
	}

	return 0;
}

cSceneManager::cSceneManager(GLuint shaderProgramID, cVaoMeshManager* pVaoMeshManager, cEntityManager* pEntityManager, cAssimpMeshLoader* pAssimpLoader)
{
	this->_currentSceneID = 0;
	this->_sceneLoadState = eSceneLoadStates::WAITING;

	this->_shaderProgramID = shaderProgramID;
	this->_pVaoMeshManager = pVaoMeshManager;
	this->_pEntityManager = pEntityManager;
	this->_pAssimpLoader = pAssimpLoader;

	this->_GenerateModelLoadThreads(3);

	return;
}

cSceneManager::~cSceneManager()
{
	std::map<unsigned long long, iScene*>::iterator mapIter = this->_mapSceneByID.begin();
	for (mapIter; mapIter != this->_mapSceneByID.end(); mapIter++)
	{
		delete mapIter->second;
		mapIter->second = nullptr;
	}
	this->_mapSceneByID.clear();

	return;
}

unsigned long long cSceneManager::GetCurrentSceneID()
{
	return this->_currentSceneID;
}

//bool cSceneManager::IsSceneReady()
//{
//	return this->_sceneLoadState == eSceneLoadStates::READY;
//}

bool cSceneManager::LoadOneMeshIntoCPU(sModelDrawInfo& modelInfo)
{
	/*switch (modelInfo.modelType)
	{
	case eModelTypes::COMMON:
		nScene::LoadOneMeshIntoCPU(modelInfo, this->_pVaoMeshManager);
		this->SetModelReadyForGPU(modelInfo);
		break;
	case eModelTypes::SKINNED:
		nScene::LoadOneSkinnedMeshIntoCPU(modelInfo, this->_pAssimpLoader, this->_pVaoMeshManager, this->_shaderProgramID);
		this->SetModelReadyForGPU(modelInfo);
		break;
	case eModelTypes::ANIMATION:
		nScene::LoadOneAnimationIntoVAO(modelInfo.meshFileName, modelInfo.friendlyName, this->_pAssimpLoader);
		break;
	}*/

	return false;
}

void cSceneManager::CreateScene(unsigned long long sceneID)
{
	if (this->_SceneExists(sceneID))
	{
		return;
	}

	switch (sceneID)
	{
	case 0:
		cTestScene* pScene = new cTestScene(sceneID);
		this->_mapSceneByID[sceneID] = pScene;
		pScene->_GenerateSceneDrawDependencies(this->_vecModelsToLoad);
		break;
	}

	// We have new models to load
	if (this->_vecModelsToLoad.size() > 0)
	{
		this->_sceneLoadState = eSceneLoadStates::PENDING;
	}
}

void cSceneManager::LoadScene(unsigned long long sceneID)
{

}

void cSceneManager::Update()
{
	switch (this->_sceneLoadState)
	{
	case eSceneLoadStates::WAITING:
		// Nothing to do
		break;
	case eSceneLoadStates::PENDING:
		if (this->_vecModelsToLoad.size() == 0)
		{
			this->_sceneLoadState = eSceneLoadStates::LOADED_INTO_CPU;
		}
		break;
	case eSceneLoadStates::LOADED_INTO_CPU:
		if (this->_vecGPUReadyModels.size() > 0)
		{
			this->_pVaoMeshManager->LoadThreadModelIntoGPU(this->_vecGPUReadyModels[this->_vecGPUReadyModels.size() - 1], this->_shaderProgramID);
			this->_vecGPUReadyModels.pop_back();
		}
		else
		{
			this->_sceneLoadState = eSceneLoadStates::READY;
		}
		break;
	case eSceneLoadStates::LOADED_INTO_GPU:
		if (this->_vecGPUReadyModels.size() > 0)
		{
			this->_pVaoMeshManager->LoadThreadModelIntoGPU(this->_vecGPUReadyModels[this->_vecGPUReadyModels.size() - 1], this->_shaderProgramID);
			this->_vecGPUReadyModels.pop_back();
		}
		else
		{
			this->_sceneLoadState = eSceneLoadStates::READY;
		}
		break;
	case eSceneLoadStates::READY:
		this->_UpdateScene();
		break;
	}
}

void cSceneManager::UnloadScene()
{

}

bool cSceneManager::_GenerateModelLoadThreads(unsigned int numThreads)
{
	for (unsigned int i = 0; i < numThreads; i++)
	{
		// Set triple buffer
		//this->_pTexTripleBuffer = pTexTripleBuffer;

		// Create thread
		LPDWORD phThread = 0;
		DWORD hThread = 0;
		HANDLE hThreadHandle = 0;

		void* pVoid = (void*)this;

		// Pass the this pointer to the thread function
		hThreadHandle = CreateThread(NULL,	// Default security
			0,		// Stack size - default - win32 = 1 Mbyte
			&TLoadModel, // Pointer to the thread's function
			pVoid,		// The value (parameter) we pass to the thread
				   // This is a pointer to void... more on this evil thing later...
			0,  // CREATE_SUSPENDED or 0 for threads...
			(DWORD*)&phThread);		// pointer or ref to variable that will get loaded with threadID

		if (hThreadHandle == NULL)
		{
			return false;
			printf("Warning! Thread not created successfully...\n");
		}

		return true;
	}
}

bool cSceneManager::GetModelNameToLoad(sModelDrawInfo& modelToLoad)
{
	if (this->_vecModelsToLoad.size() == 0)
	{
		return false;
	}

	EnterCriticalSection(&(this->CS_MeshVecLock));

	modelToLoad = this->_vecModelsToLoad[this->_vecModelsToLoad.size() - 1];
	this->_vecModelsToLoad.pop_back();

	LeaveCriticalSection(&(this->CS_MeshVecLock));

	return true;
}

bool cSceneManager::SetModelReadyForGPU(sModelDrawInfo& modelInfo)
{
	EnterCriticalSection(&(this->CS_GPUVecLock));

	this->_vecGPUReadyModels.push_back(modelInfo);

	LeaveCriticalSection(&(this->CS_GPUVecLock));

	return false;
}

void cSceneManager::_LoadSceneModels(unsigned long long sceneID)
{

}

bool cSceneManager::_SceneExists(unsigned long long sceneID)
{
	std::map<unsigned long long, iScene*>::iterator mapIter = this->_mapSceneByID.find(sceneID);

	if (mapIter != this->_mapSceneByID.end())
	{
		// Found it
		return true;
	}

	// Nope
	return false;
}

void cSceneManager::_UpdateScene()
{

}
