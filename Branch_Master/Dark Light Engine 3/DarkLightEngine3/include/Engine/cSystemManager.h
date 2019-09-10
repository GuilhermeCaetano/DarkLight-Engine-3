#ifndef _cSystemManager_HG_
#define _cSystemManager_HG_

#include <OpenGL\OpenGLHeaders.h>
#include <vector>


class iSystem;
class cSystemManager
{
public:
	cSystemManager();
	~cSystemManager();

	// Create all separate system types.
	void CreateSystems();

	// Create Draw, Camera, Light and Animation systems
	void CreateRenderSystems();
	void CreateAudioSystems();
	void CreateControllerSystems();
	void CreateAISystems();

	// Update all created systems
	void UpdateSystems(double deltaTime);

	void UpdateRender(int width, int height, GLuint shaderProgramID, double deltaTime);
	void UpdateAudio(double deltaTime);
	void UpdateControllers(double deltaTime);
	void UpdateAI(double deltaTime);
	
	// Delete and cleanup
	void DeleteSystems();

private:
	std::vector<iSystem*> _vecRenderSystems;
	std::vector<iSystem*> _vecAudioSystems;
	std::vector<iSystem*> _vecControllerSystems;
	std::vector<iSystem*> _vecAISystems;

	bool _bIsLightUnilocSet;
};

#endif // !_cSystemManager_HG_

