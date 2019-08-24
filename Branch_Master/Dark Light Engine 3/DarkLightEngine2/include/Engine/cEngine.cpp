// cEngine.cpp

#include "cEngine.hpp"


cEngine::cEngine()
{
	return;
}

cEngine::~cEngine()
{
	return;
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
