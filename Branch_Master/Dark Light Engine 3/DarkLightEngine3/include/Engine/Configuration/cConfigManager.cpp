// cConfigManager.cpp

#include "cConfigManager.h"
#include "Physics\nPhysicsConfigLoader.h"
#include <Shader\cShaderManager.h>
#include <Interfaces\Physics\iPhysicsFactory.h>
#include <Interfaces\Physics\iPhysicsSystem.h>


cConfigManager::cConfigManager()
{
	this->_basePath = "config/";

	this->_hGetProckDLL = NULL;

	return;
}

cConfigManager::~cConfigManager()
{
	this->_Cleanup();

	return;
}

bool cConfigManager::SetBasePath(const std::string& filePath)
{
	// Check if the file path exists
	struct stat info;
	if (stat(filePath.c_str(), &info) != 0)
	{
		printf("Configuration path %s couldn't be accessed!\n", filePath);
		return false;
	}
	else if (info.st_mode& S_IFDIR)
	{
		// Found, set it to be used
		this->_basePath = filePath;
		return true;
	}

	return false;
}

bool cConfigManager::ConfigurePhysics(const std::string& filename)
{
	// Load chosen DLL for physics
	std::string fullPathname = this->_basePath + filename;
	bool result = nConfig::LoadPhysicsDLL(fullPathname, this->_phyEngineConfig);
	assert(result);

	// Create physics factory to use when generating physics
	this->_pPhysicsFactory = nConfig::CreatePhysicsFactory(this->_hGetProckDLL, this->_phyEngineConfig);
	result = this->_pPhysicsFactory != nullptr;
	assert(result);

	return result;
}

bool cConfigManager::ConfigureShader(cShaderManager* pShadermanager, const std::string& vertex, const std::string& fragment, const std::string& progName)
{
	cShaderManager::cShader vertexShader = pShadermanager->CreateShaderFromFile(vertex, cShaderManager::cShader::VERTEX_SHADER);
	cShaderManager::cShader fragmentShader = pShadermanager->CreateShaderFromFile(fragment, cShaderManager::cShader::FRAGMENT_SHADER);

	if (!pShadermanager->CreateShaderProgramFromFiles(progName, vertexShader, fragmentShader))
	{
		// Wait for input, so we can see the code
		getchar();
		return false;
	}

	printf("Loaded %s shader.\n", progName.c_str());
	return true;
}

bool cConfigManager::ConfigureShader(cShaderManager* pShadermanager, const std::string& vertex, const std::string& fragment, const std::string& progName,
	const std::string& tesselation, const std::string& geometry)
{
	cShaderManager::cShader vertexShader = pShadermanager->CreateShaderFromFile(vertex, cShaderManager::cShader::VERTEX_SHADER);
	cShaderManager::cShader fragmentShader = pShadermanager->CreateShaderFromFile(fragment, cShaderManager::cShader::FRAGMENT_SHADER);
	cShaderManager::cShader geometryShader = pShadermanager->CreateShaderFromFile(fragment, cShaderManager::cShader::GEOMETRY_SHADER);
	//cShaderManager::cShader tesselationShader = pShadermanager->CreateShaderFromFile(fragment, cShaderManager::cShader::TESSELATION_SHADER);

	if (!pShadermanager->CreateShaderProgramFromFiles(progName, vertexShader, fragmentShader, geometryShader))
	{
		// Wait for input, so we can see the code
		getchar();
		return false;
	}

	printf("Loaded %s shader.\n", progName.c_str());
	return true;
}

std::string cConfigManager::GetPhysicsType()
{
	return this->_phyEngineConfig.physicsDLLType;
}

nPhysics::iPhysicsFactory* cConfigManager::GetPhysicsFactory()
{
	return this->_pPhysicsFactory;
}

void cConfigManager::_Cleanup()
{
	if (this->_hGetProckDLL)
	{
		FreeLibrary(this->_hGetProckDLL);
		this->_hGetProckDLL = NULL;
	}
}
