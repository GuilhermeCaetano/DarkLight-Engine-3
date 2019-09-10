#ifndef _cECSEngine_HG_
#define _cECSEngine_HG_


/* ECS Engine Class
 - Responsible for creating and configuring the engine
 - Responsible for loading engine status
 - Contains the creation of the ECS 
 - Commands and spreads low level engine calls
*/

#include <OpenGL\OpenGLHeaders.h>
#include <string>

class cConfigManager;
class cAssetManager;
class cEngineStateLoader;
class cEngine;
class cTestScene;
class cECSPoolContainer;

class cECSControl;
class cGraphicsControl;
class cIOManager;


class cECSEngine
{
public:
	cECSEngine(GLFWwindow* window);
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
	void UpdateEngine(GLFWwindow* window);

	// Stop the managers and cleanup
	void StopEngine();

private:
	cEngine* _pEngine;
	cConfigManager* _pConfigManager;
	cIOManager* _pIOManager;
	cEngineStateLoader* _pEngineStateLoader;
	cECSPoolContainer* _pECSPoolContainer;

	cTestScene* _pInitialScene;

	double _lastTime;
	static double _deltaTime;

};


#endif // !_cECSEngine_HG_

