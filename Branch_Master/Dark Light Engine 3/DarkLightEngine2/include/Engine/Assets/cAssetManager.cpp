// cAssetManager.cpp

#include "cAssetManager.h"
#include <Engine\Graphics\cGraphicsControl.h>
#include <Scene\nSceneHelper.h>

extern cGraphicsControl* gGraphicsControl;


// The thread function that will do the actual loading of the data. This one is for a certain pack
DWORD WINAPI TLoadOneAsset(PVOID pvParam)
{
	// Convert the class back from the void*
	cAssetManager::sThreadPair* voidPair = static_cast<cAssetManager::sThreadPair*>(pvParam);

	cAssetManager* pManager = voidPair->_pAssetManager;
	cAssetManager::sAssetpack* assetPack = voidPair->_pAssetPack;
	delete voidPair;

	sAsset* asset = nullptr;

	while (true)
	{
		if (assetPack->_packState == sAsset::eAssetStates::LOADED_INTO_CPU)
		{
			break;
		}

		asset = pManager->GetAssetToLoad(assetPack);
		if (asset != nullptr)
		{
			bool ready = false;
			ready = pManager->LoadOneAsset(asset);
			
			if (ready)
			{
				// Add this asset to the GPU load
				pManager->SetOneAssetForGPU(assetPack, asset);
			}
			else
			{
				Sleep(1);
			}
		}
		else
		{
			// Nothing to load, sleep
			Sleep(1);
		}
	}
	printf("AssetPack thread finished.\n");

	return 0;
}

cAssetManager::cAssetManager()
{
	// Initialize the Critical Sections!
	InitializeCriticalSection(&(this->CS_AssetPackVecLock));
	InitializeCriticalSection(&(this->CS_AssetVecLock));
	InitializeCriticalSection(&(this->CS_PendingVecLock));
	InitializeCriticalSection(&(this->CS_GPUVecLock));
	InitializeCriticalSection(&(this->CS_MapLock));

	return;
}

cAssetManager::~cAssetManager()
{
	this->_Cleanup();

	return;
}

bool cAssetManager::_GenerateSingleAssetLoadThreads(cAssetManager::sAssetpack* assetPack, unsigned int numThreads)
{
	for (unsigned int i = 0; i < numThreads; i++)
	{
		// Create thread
		LPDWORD phThread = 0;
		DWORD hThread = 0;
		HANDLE hThreadHandle = 0;

		sThreadPair* pair = new sThreadPair(this, assetPack);
		void* pVoid = (void*)pair;

		// Pass the this pointer to the thread function
		hThreadHandle = CreateThread(NULL,	// Default security
			0,		// Stack size - default - win32 = 1 Mbyte
			&TLoadOneAsset, // Pointer to the thread's function
			pVoid,		// The value (parameter) we pass to the thread
			0,  // CREATE_SUSPENDED or 0 for threads
			(DWORD*)&phThread);		// pointer or ref to variable that will get loaded with threadID

		if (hThreadHandle == NULL)
		{
			return false;
			printf("Warning! Thread not created successfully...\n");
		}
	}

	return true;
}

void cAssetManager::LoadOneAssetPackIntoCPU(const std::string& packID, unsigned int numThreads)
{
	// Check all asset packs for a match to load, if not already
	printf("\nLoading AssetPack %s with %d threads...\n", packID.c_str(), numThreads);
	for (unsigned int i = 0; i < this->_vecAssetPacks.size(); i++)
	{
		sAssetpack* assetPack = this->_vecAssetPacks[i];
		if (assetPack->_packState == sAsset::UNLOADED && assetPack->_packID == packID)
		{
			this->_GenerateSingleAssetLoadThreads(assetPack, numThreads);
		}
	}
}

void cAssetManager::LoadOneAssetPackIntoGPU(const std::string& packID, GLuint shaderProgramID)
{
	// Check all asset packs for a match to load into the GPU, if not already
	for (unsigned int i = 0; i < this->_vecAssetPacks.size(); i++)
	{
		sAssetpack* assetPack = this->_vecAssetPacks[i];
		if (assetPack->_packID == packID)
		{
			assetPack->_packLoadMode = sAssetpack::eAssetLoadModes::READY_TO_LOAD;
		}
	}
}

void cAssetManager::UnloadAssetPackFromGPU(const std::string& packID)
{
	std::unordered_map<std::string, sAssetpack*>::iterator mapIter = this->_mapInUsePacksByName.find(packID);

	// If no match was found, bail
	if (mapIter == this->_mapInUsePacksByName.end())
	{
		return;
	}

	// If it's not on the GPU, get out.
	sAssetpack* pack = mapIter->second;
	if (pack->_packState != sAsset::eAssetStates::LOADED_INTO_GPU)
	{
		return;
	}

	// Try to unload all assets. This should only unload if previously loaded and not currently being reused.
	for (unsigned int i = 0; i < pack->vecAssets.size(); i++)
	{
		sAsset* asset = pack->vecAssets[i];

		// If it's not loaded, or if it's being reused, don't unload
		if (asset->_assetState != sAsset::eAssetStates::LOADED_INTO_GPU || this->CheckAssetBeingReused(pack, asset))
		{
			continue;
		}

		gGraphicsControl->GetVaoMeshManager()->UnloadModelFromVAO(static_cast<sModelDrawInfo*>(asset->_assetInfo));
		asset->_assetState = sAsset::eAssetStates::LOADED_INTO_CPU;
	}
	pack->_packState = sAsset::eAssetStates::LOADED_INTO_CPU;
}

void cAssetManager::UnloadAssetPackFromCPU(const std::string& packID)
{
	std::unordered_map<std::string, sAssetpack*>::iterator mapIter = this->_mapInUsePacksByName.find(packID);

	// If no match was found, bail
	if (mapIter == this->_mapInUsePacksByName.end())
	{
		return;
	}

	// Check if the pack needs to be unloaded from the GPU first
	sAssetpack* pack = mapIter->second;
	if (pack->_packState == sAsset::eAssetStates::LOADED_INTO_GPU)
	{
		this->UnloadAssetPackFromGPU(packID);
	}

	// If it's not on the CPU, get out.
	if (pack->_packState != sAsset::eAssetStates::LOADED_INTO_CPU)
	{
		return;
	}

	// Try to unload all assets. This should only unload if previously loaded and not currently being reused.
	for (unsigned int i = 0; i < pack->vecAssets.size(); i++)
	{
		sAsset* asset = pack->vecAssets[i];

		// If it's not loaded, or if it's being reused, don't unload
		if (asset->_assetState != sAsset::eAssetStates::LOADED_INTO_CPU || this->CheckAssetBeingReused(pack, asset))
		{
			continue;
		}

		if (asset->_assetInfo != nullptr)
		{
			asset->DestroyAssetInfo();
		}
		asset->_assetState = sAsset::eAssetStates::UNLOADED;
	}

	// Reset flag and remove from map
	pack->_packState = sAsset::eAssetStates::UNLOADED;
	this->_mapInUsePacksByName.erase(pack->_packID);
}

void cAssetManager::AddAssetpackToMap(sAssetpack* pack)
{
	EnterCriticalSection(&(CS_MapLock));
	this->_mapInUsePacksByName[pack->_packID] = pack;
	LeaveCriticalSection(&(CS_MapLock));
}

void cAssetManager::RemoveAssetpackFromMap(sAssetpack* pack)
{
	EnterCriticalSection(&(CS_MapLock));
	this->_mapInUsePacksByName.erase(pack->_packID);
	LeaveCriticalSection(&(CS_MapLock));
}

sAsset* cAssetManager::CheckAssetBeingReused(cAssetManager::sAssetpack* assetpack, sAsset* asset)
{
	std::unordered_map<std::string, cAssetManager::sAssetpack*>::iterator mapIter = this->_mapInUsePacksByName.begin();
	for (mapIter; mapIter != this->_mapInUsePacksByName.end(); mapIter++)
	{
		sAssetpack* pack = mapIter->second;
		if (pack == assetpack)
		{
			// Don't check asset owner pack
			continue;
		}

		for (unsigned int i = 0; i < pack->vecAssets.size(); i++)
		{
			sAsset* duplicateAsset = pack->vecAssets[i];
			if (duplicateAsset->_name == asset->_name && duplicateAsset->_assetState != sAsset::eAssetStates::UNLOADED)
			{
				// Assimilate and return
				return duplicateAsset;
			}
		}
	}

	return nullptr;
}

sAsset* cAssetManager::CheckAndAssimilateDuplicateAsset(cAssetManager::sAssetpack* assetpack, sAsset* asset)
{
	sAsset* duplicateAsset = this->CheckAssetBeingReused(assetpack, asset);
	if (duplicateAsset != nullptr)
	{
		// Assimilate and return
		asset->_assetState = duplicateAsset->_assetState;
		asset->_assetInfo = duplicateAsset->_assetInfo;
		return duplicateAsset;
	}

	return nullptr;
}

cAssetManager::sAssetpack* cAssetManager::CreateAssetPack(const std::string& packID)
{
	sAssetpack* assetPack = new sAssetpack(packID, &this->CS_AssetVecLock);
	this->_vecAssetPacks.push_back(assetPack);
	return assetPack;
}

sAsset* cAssetManager::GetAssetToLoad(sAssetpack* packIn)
{
	EnterCriticalSection(&(CS_AssetVecLock));
	
	sAssetpack* assetPack = packIn;
	sAsset* asset = nullptr;
	bool allAssetsInCPU = true;

	if (assetPack != nullptr)
	{
		if (assetPack->_packState == sAsset::LOADED_INTO_CPU)
		{
			LeaveCriticalSection(&(CS_AssetVecLock));
			return nullptr;
		}

		for (unsigned int i = 0; i < assetPack->vecAssets.size(); i++)
		{
			// Check map of in use pacls to see if there are any duplicates
			asset = assetPack->vecAssets[i];
			if (this->CheckAndAssimilateDuplicateAsset(assetPack, asset) != nullptr)
			{
				continue;
			}
			
			// Asset is not loaded, so check to see if this asset is not currently loading
			if (assetPack->vecAssets[i]->_assetState == sAsset::eAssetStates::UNLOADED)
			{
				assetPack->vecAssets[i]->_assetState = sAsset::eAssetStates::LOADING;
				asset = assetPack->vecAssets[i];
				allAssetsInCPU = false;
				break;
			}

			if (assetPack->vecAssets[i]->_assetState == sAsset::eAssetStates::LOADING)
			{
				// If it's still loading, flag the pack as not ready
				allAssetsInCPU = false;
			}
		}
	}

	// Is the asset pack all loaded into the CPU and ready for GPU?
	if (assetPack != nullptr && allAssetsInCPU && assetPack->vecAssets.size() > 0)
	{
		assetPack->_packState = sAsset::LOADED_INTO_CPU;
		this->AddAssetpackToMap(assetPack);
	}

	LeaveCriticalSection(&(CS_AssetVecLock));
	return asset;
}

bool cAssetManager::LoadOneAsset(sAsset* assetIn)
{
	EnterCriticalSection(&(CS_PendingVecLock));
	bool result = false;

	//printf("Loading %s asset...\n", assetIn->_name.c_str());
	switch (assetIn->_assetType)
	{
	case sAsset::MESH_COMMON:
		result = nScene::LoadOneMeshIntoCPU(assetIn->_name, &assetIn->_assetInfo, gGraphicsControl->GetVaoMeshManager());
		assetIn->_assetState = sAsset::eAssetStates::LOADED_INTO_CPU;
		break;
	case sAsset::MESH_SKINNED:
		result = nScene::LoadOneSkinnedMeshIntoCPU(assetIn->_name, &assetIn->_assetInfo, gGraphicsControl->GetAssimpMeshLoader(), gGraphicsControl->GetVaoMeshManager());
		assetIn->_assetState = sAsset::eAssetStates::LOADED_INTO_CPU;
		break;
	case sAsset::MESH_ANIMATION:
		result = nScene::LoadOneAnimationIntoCPU(assetIn->_name, assetIn->_friendlyName, gGraphicsControl->GetAssimpMeshLoader());
		assetIn->_assetState = sAsset::eAssetStates::LOADED_INTO_CPU;
		break;
	}
	//printf("Loaded %s asset.\n", assetIn->_name.c_str());

	assetIn->_ownsData = result;
	LeaveCriticalSection(&(CS_PendingVecLock));

	return result;
}

bool cAssetManager::SetOneAssetForGPU(sAssetpack* assetPack, sAsset* assetIn)
{
	EnterCriticalSection(&(this->CS_GPUVecLock));

	assetPack->vecGPUReadyAssets.push_back(assetIn);

	LeaveCriticalSection(&(this->CS_GPUVecLock));

	return false;
}

void cAssetManager::Update(GLuint shaderProgramID)
{
	// Check for Assets that have GPU load pending and are to be loaded immediately
	for (unsigned int i = 0; i < this->_vecAssetPacks.size(); i++)
	{
		sAssetpack* assetPack = this->_vecAssetPacks[i];
		this->_CheckPendingGPULoads(assetPack, shaderProgramID);
	}

	/*bool done = true;
	for (unsigned int i = 0; i < this->_vecAssetPacks.size(); i++)
	{
		if (this->_vecAssetPacks[i]->_assetState != sAssetpack::LOADED)
		{
			done = false;
		}
	}

	this->ready = done;*/
}

void cAssetManager::_CheckPendingGPULoads(cAssetManager::sAssetpack* assetPack, GLuint shaderProgramID)
{
	EnterCriticalSection(&(this->CS_MapLock));

	bool ready = true;
	std::unordered_map<std::string, cAssetManager::sAssetpack*>::iterator mapIter = this->_mapInUsePacksByName.begin();
	for (mapIter; mapIter != this->_mapInUsePacksByName.end(); mapIter++)
	{
		sAssetpack* pack = mapIter->second;
		if (pack->_packLoadMode == sAssetpack::eAssetLoadModes::READY_TO_LOAD)
		{
			for (unsigned int i = 0; i < pack->vecGPUReadyAssets.size(); i++)
			{
				sAsset* asset = pack->vecGPUReadyAssets[i];
				if (asset->_assetState == sAsset::LOADED_INTO_CPU)
				{
					if (asset->_assetType == sAsset::eAssetTypes::MESH_COMMON || asset->_assetType == sAsset::eAssetTypes::MESH_SKINNED)
					{
						bool result = nScene::LoadOneTMeshIntoGPU(asset->_assetInfo, gGraphicsControl->GetVaoMeshManager(), shaderProgramID);
						asset->_assetState = sAsset::eAssetStates::LOADED_INTO_GPU;
					}
				}
			}
		}
	}

	/*if (assetPack->_packState != sAsset::eAssetStates::LOADED_INTO_CPU)
	{
		return;
	}*/

	/*for (unsigned int i = 0; i < assetPack->vecGPUReadyAssets.size(); i++)
	{
		sAsset* asset = assetPack->vecGPUReadyAssets[i];

		if (asset->_assetState == sAsset::eAssetStates::LOADED_INTO_CPU)
		{
			if (asset->_assetType == sAsset::eAssetTypes::MESH_COMMON)
			{
				bool result = nScene::LoadOneTMeshIntoGPU(asset->_assetInfo, gGraphicsControl->GetVaoMeshManager(), shaderProgramID);
				asset->_assetState = sAsset::eAssetStates::LOADED_INTO_GPU;
			}
			else if (asset->_assetType == sAsset::eAssetTypes::MESH_SKINNED)
			{
				bool result = nScene::LoadOneTSkinnedMeshIntoGPU(asset->_assetInfo, gGraphicsControl->GetVaoMeshManager(), gGraphicsControl->GetAssimpMeshLoader(), shaderProgramID);
				asset->_assetState = sAsset::eAssetStates::LOADED_INTO_GPU;
			}
		}
	}*/

	if (ready)
	{
		assetPack->_packState = sAsset::eAssetStates::LOADED_INTO_GPU;
	}

	//LeaveCriticalSection(&(this->CS_GPUVecLock));
}

void cAssetManager::_Cleanup()
{
	// Cleanup deleting the Critical Sections!
	DeleteCriticalSection(&(this->CS_AssetPackVecLock));
	DeleteCriticalSection(&(this->CS_AssetVecLock));
	DeleteCriticalSection(&(this->CS_PendingVecLock));
	DeleteCriticalSection(&(this->CS_GPUVecLock));
	DeleteCriticalSection(&(this->CS_MapLock));

	// Delete all asset packs
	for (unsigned int i = 0; i < this->_vecAssetPacks.size(); i++)
	{
		delete this->_vecAssetPacks[i];
	}
}


void cAssetManager::sAssetpack::SetAssetLoadingMode(eAssetLoadModes loadMode)
{
	this->_packLoadMode = loadMode;
}

void cAssetManager::sAssetpack::AddMeshAsset(const std::string& meshName)
{
	this->vecAssets.push_back(new sAsset(meshName, meshName, sAsset::eAssetTypes::MESH_COMMON));
}

void cAssetManager::sAssetpack::AddSkinnedMeshAsset(const std::string& meshName)
{
	this->vecAssets.push_back(new sAsset(meshName, meshName, sAsset::eAssetTypes::MESH_SKINNED));
}

void cAssetManager::sAssetpack::AddAnimationAsset(const std::string& meshName, const std::string& friendlyName)
{
	this->vecAssets.push_back(new sAsset(meshName, friendlyName, sAsset::eAssetTypes::MESH_ANIMATION));
}

bool cAssetManager::sAssetpack::CheckPackIsInCPU()
{
	return this->_packState == sAsset::eAssetStates::LOADED_INTO_CPU;
}

bool cAssetManager::sAssetpack::CheckPackIsInGPU()
{
	return this->_packState == sAsset::eAssetStates::LOADED_INTO_GPU;
}

void cAssetManager::sAssetpack::Cleanup()
{
	// Delete all created assets
	for (unsigned int i = 0; i < this->vecAssets.size(); i++)
	{
		delete this->vecAssets[i];
	}
}
