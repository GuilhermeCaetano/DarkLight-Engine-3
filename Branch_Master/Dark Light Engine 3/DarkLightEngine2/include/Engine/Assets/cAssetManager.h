#ifndef _cAssetManager_HG_
#define _cAssetManager_HG_

/* AssetManager
 TODO: add support for the textures and channel loading here instead from the TextureManager, and also for audios
 - Create asset packs that can hold different assets inside.
 - Add as many assets as needed. Only one type of each is needed.
 - Load/Unload them to the CPU.
 - Load/Unload them to the GPU.
 - Assets that are in use will not be unloaded. This prevents dulicate loads and also avoids unloadingjust to be reloaded again.










 - Creates asset packs that can store multiple different assets inside
 - Assets can be added to the pack one by one. Currently mesh and skinned meshes only can be used.
 - Once a pack is ready, call BeginLoadProcess() to start loading them.
 - The AssetLoadMode allows us to choose if the CPU loading will be right after, or if the BeginLoad needs to be called.

 - BeginLoadProcess WILL LOAD THE ASSETS INTO THE CPU, NOT THE GPU!
   The number passed on the function will create threads that will help to load the pack.
   This is a BLOCKING call! This is deliberate, so to avoid any missing necessary assets during runtime!
   This is good because we can create all asset packs needed for the game, but only load specific ones at a time!
   For example, a level based game would only load what is needed on certain parts, not all at the beggining, saving a lot of processing!
 - Once the asset has been loaded into the CPU (the slowest part), LoadAssets() can be called to load them into the GPU.
   This is good because we can only load them to the GPU when needed and then call UnloadAssets to free space, but they will
   still be loaded into the CPU, not needing to waste time again. The GPU should be quite fast, allowing us to allocate and free GPU space at will!
*/

#include "sAsset.h"
#include <OpenGL\OpenGLHeaders.h>
#include <Mesh\sModelDrawInfo.h>
#include <Windows.h>
#include <vector>
#include <unordered_map>


class cAssetManager
{
public:
	cAssetManager();
	~cAssetManager();

	struct sAssetpack
	{
		// THIS IS EXTREMELY IMPORTANT TO CONTROL THE LOAD PROCESS.
		// USE THE CORRECT LOAD MODE OR IT WILL CAUSE PROBLEMS!
		enum eAssetLoadModes
		{
			WAIT_FOR_CALL,
			READY_TO_LOAD
		};

		sAssetpack(const std::string& packID, CRITICAL_SECTION* assetCriticalSection)
		{
			this->_packID = packID;
			this->_packLoadMode = WAIT_FOR_CALL;
			this->_packState = sAsset::eAssetStates::UNLOADED;
			this->CS_AssetVecLock = assetCriticalSection;
			this->_centerPos = glm::vec3(0.0f);
			this->_extents = glm::vec3(0.0f);
		}

		~sAssetpack()
		{
			this->Cleanup();
		}

		/* Sets flags for when the pack should be loaded into the CPU.
		   It can either be immediatly*/
		void SetAssetLoadingMode(eAssetLoadModes loadMode);

		// Add different kinds of assets to the pack
		void AddMeshAsset(const std::string& meshName);
		void AddSkinnedMeshAsset(const std::string& meshName);
		void AddAnimationAsset(const std::string& meshName, const std::string& friendlyName);
		void AddTextureAsset(const std::string& meshName, const std::string& friendlyName);
		void AddAudioAsset(const std::string& meshName, const std::string& friendlyName);

		bool CheckPackIsInCPU();
		bool CheckPackIsInGPU();

		void Cleanup();

		// Asset pack paramters
		std::string _packID;
		eAssetLoadModes _packLoadMode;
		sAsset::eAssetStates _packState;
		CRITICAL_SECTION* CS_AssetVecLock;

		// Load region
		glm::vec3 _centerPos;
		glm::vec3 _extents;

		// Assets to load
		std::vector<sAsset*> vecAssets;
		std::vector<sAsset*> vecGPUReadyAssets;
	};

	struct sThreadPair
	{
		sThreadPair(cAssetManager* pManager, cAssetManager::sAssetpack* pack)
		{
			this->_pAssetManager = pManager;
			this->_pAssetPack = pack;
		}

		cAssetManager* _pAssetManager;
		cAssetManager::sAssetpack* _pAssetPack;
	};

	// Creates an asset pack to load with an ID to identify it
	sAssetpack* CreateAssetPack(const std::string& packID);

	// Loads a specific or all asset packs into the CPU. CPU loading is slow.
	void LoadOneAssetPackIntoCPU(const std::string& packID, unsigned int numThreads);
	void LoadAllAssetPacksIntoCPU(unsigned int numThreads);

	// Loads a specific or all asset packs into the GPU. GPU loading is fast.
	void LoadOneAssetPackIntoGPU(const std::string& packID, GLuint shaderProgramID);
	void LoadAllAssetPacksIntoGPU(GLuint shaderProgramID);

	// Unloads a specific or all asset packs from the CPU.
	void UnloadAssetPackFromCPU(const std::string& packID);
	void UnloadAllAssetPacksFromCPU();

	// Unloads a specific or all asset packs from the GPU.
	void UnloadAssetPackFromGPU(const std::string& packID);
	void UnloadAllAssetPacksFromGPU();

	void AddAssetpackToMap(sAssetpack* pack);
	void RemoveAssetpackFromMap(sAssetpack* pack);
	sAsset* CheckAssetBeingReused(cAssetManager::sAssetpack* assetpack, sAsset* asset);
	sAsset* CheckAndAssimilateDuplicateAsset(cAssetManager::sAssetpack* assetpack, sAsset* asset);

	// Check for packs to load to the GPU
	void Update(GLuint shaderProgramID);

	//// Returns packets and assets to load to the CPU
	//cAssetManager::sAssetpack* GetPackToLoad();
	sAsset* GetAssetToLoad(sAssetpack* packIn);

	// Adds CPU ready asset into GPU waiting vector
	bool LoadOneAsset(sAsset* assetIn);
	bool SetOneAssetForGPU(sAssetpack* assetPack, sAsset* assetIn);
	

	//bool ready;

private:
	// Generates a number of threads to handle model loading from the CPU
	bool _GenerateSingleAssetLoadThreads(cAssetManager::sAssetpack* assetPack, unsigned int numThreads);
	bool _GenerateAllAssetsLoadThreads(unsigned int numThreads);

	// 
	void _CheckPendingGPULoads(cAssetManager::sAssetpack* assetPack, GLuint shaderProgramID);

	void _Cleanup();

	// Vector of all created asset packs
	std::vector<sAssetpack*> _vecAssetPacks;
	std::unordered_map<std::string, sAssetpack*> _mapInUsePacksByName;
	std::vector<sAsset*> _vecCreatedAssets; // Need to keep track for destruction

	// The critical section to protect the model vector
	CRITICAL_SECTION CS_AssetPackVecLock;
	CRITICAL_SECTION CS_AssetVecLock;
	CRITICAL_SECTION CS_PendingVecLock;
	CRITICAL_SECTION CS_MapLock;
	CRITICAL_SECTION CS_GPUVecLock;

};

#endif // !_cAssetManager_HG_

