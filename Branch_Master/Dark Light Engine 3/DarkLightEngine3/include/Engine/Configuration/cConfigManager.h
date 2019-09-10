#ifndef _cConfigManager_HG_
#define _cConfigManager_HG_

#include "sConfig.h"

#include <Windows.h>
#include <OpenGL\OpenGLHeaders.h>

class cShaderManager;

namespace nPhysics
{
	class iPhysicsFactory;
}

class cConfigManager
{
public:
	cConfigManager();
	~cConfigManager();

	// Sets the path to find config files. Returns result
	bool SetBasePath(const std::string& filePath);

	bool ConfigurePhysics(const std::string& filename);
	bool ConfigureShader(cShaderManager* pShadermanager, const std::string& vertex, const std::string& fragment, const std::string& progName);
	bool ConfigureShader(cShaderManager* pShadermanager, const std::string& vertex, const std::string& fragment, const std::string& progName,
		const std::string& tesselation, const std::string& geometry);
	std::string GetPhysicsType();

	nPhysics::iPhysicsFactory* GetPhysicsFactory();

private:
	void _Cleanup();

	std::string _basePath;
	nConfig::sConfig _phyEngineConfig;
	HINSTANCE _hGetProckDLL;

	nPhysics::iPhysicsFactory* _pPhysicsFactory;

};

#endif // !_cConfigManager_HG_
