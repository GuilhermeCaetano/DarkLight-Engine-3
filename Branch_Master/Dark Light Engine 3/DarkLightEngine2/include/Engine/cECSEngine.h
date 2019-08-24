#ifndef _cECSEngine_HG_
#define _cECSEngine_HG_


/* ECS Engine Class
 - Responsible for creating and configuring the engine
 - Responsible for loading engine status
 - Contains the creation of the ECS 
 - Commands and spreads low level engine calls
*/

#include <string>

class cConfigManager;
//class cShaderManager;
//class cVaoMeshManager;
//class cAssimpMeshLoader;
class cAssetManager;
//class cTextureManager;
class cEngineStateLoader;
class cEngine;
class cTestScene;
class cECSPoolContainer;

class cECSControl;
class cGraphicsControl;


class cECSEngine
{
public:
	cECSEngine();
	~cECSEngine();

	// Create managers and setup them
	void StartEngine();

	// Load external engine configurations
	void LoadExternalConfigurations();

	// Initialize graphics side with shaders
	// and prepare engine for the rendering pipeline
	void InitGraphicsPipeline();

	// Pre Allocate initial memory
	void PreAllocateEngineMemory();

	// Pack assets used by the engine and star loading them
	// Essentials will block the engine first and the rest will load during runtime
	void LoadAssets();

	// Load last engine save state
	void SaveCurrentEngineState(const std::string& stateFilename);
	void LoadLastEngineState(const std::string& stateFilename);

	// Update all engine parts
	void UpdateEngine();

	// Stop the managers and cleanup
	void StopEngine();

private:
	cConfigManager* _pConfigManager;
	//cShaderManager* _pShaderManager;
	//cVaoMeshManager* _pVaoMeshManager;
	//cAssimpMeshLoader* _pAssimpLoader;
	//cAssetManager* _pAssetManager;
	//cTextureManager* _pTextureManager;
	cEngineStateLoader* _pEngineStateLoader;
	//cECSControl* _pECSControl;
	cEngine* _pEngine;
	cECSPoolContainer* _pECSPoolContainer;

	cTestScene* _pInitialScene;

	double _lastTime;
	static double _deltaTime;

};


#endif // !_cECSEngine_HG_

