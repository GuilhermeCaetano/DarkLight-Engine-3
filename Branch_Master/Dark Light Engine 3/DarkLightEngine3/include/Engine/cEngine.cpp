// cEngine.cpp

#include "cEngine.hpp"
#include "cECSPoolContainer.h"
#include "cECSControl.h"
#include "Graphics\cGraphicsControl.h"
#include <IO\cIOManager.h>
#include <Scene\cWorldManager.h>
#include <Shader\cShaderManager.h>

extern cECSControl* gECSControl;
extern cGraphicsControl* gGraphicsControl;
extern cWorldManager* gWorldManager;
extern cIOManager* gIOManager;


cEngine::cEngine()
{
	return;
}

cEngine::~cEngine()
{
	return;
}

cWorld* cEngine::GetWorld()
{
	return gWorldManager->GetMainWorld();
}

cGraphicsControl* cEngine::GetGraphicsControl()
{
	return gGraphicsControl;
}

cAssetManager* cEngine::GetAssetManager()
{
	return gAssetManager;
}

cIOManager* cEngine::GetIOManager()
{
	return gIOManager;
}

sEntity* cEngine::CreateEntity()
{
	return gECSControl->GetEntityManager()->GenerateEntityOfType("EmptyPrefab");
}

sEntity* cEngine::CreateEntity(const std::string& prefabType)
{
	return gECSControl->GetEntityManager()->GenerateEntityOfType(prefabType);
}

bool cEngine::FindMeshByName(const std::string& meshName, sModelDrawInfo** modelInfo)
{
	return gGraphicsControl->GetVaoMeshManager()->FindDrawInfoByName(meshName, modelInfo);
}

sModelDrawInfo* cEngine::FindMeshByName(const std::string & meshName)
{
	return gGraphicsControl->GetVaoMeshManager()->FindDrawInfoByName(meshName);
}

GLuint cEngine::GetShaderProgramIDByName(const std::string& programName)
{
	return gGraphicsControl->GetShaderManager()->GetShaderProgramIDByName(programName);
}

sEntity* cEngine::FindEntityByName(const std::string& entityName, bool isEnabled)
{
	void* object = cECSPoolContainer::GetInstance()->GetPoolEntityByName(entityName, isEnabled);
	if (object != nullptr)
	{
		sEntity* entity = static_cast<sEntity*>(object);
		return entity;
	}

	return nullptr;
}
