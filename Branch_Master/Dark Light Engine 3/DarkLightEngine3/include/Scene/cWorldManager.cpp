// cSceneController.cpp

#include "cWorldManager.h"
#include "cWorld.h"


cWorldManager::cWorldManager()
{
	this->_mainWorld = nullptr;

	return;
}

cWorldManager::~cWorldManager()
{
	return;
}

bool cWorldManager::CreateWorld(const std::string& worldName)
{
	cWorld* newWorld = new cWorld();
	newWorld->name = worldName;

	this->_mainWorld = newWorld;

	return this->_mainWorld != nullptr;
}

bool cWorldManager::DestroyWorld(const std::string & worldName)
{
	if (this->_mainWorld != nullptr)
	{
		delete this->_mainWorld;
		return true;
	}

	return false;
}

cWorld* cWorldManager::GetMainWorld()
{
	return this->_mainWorld;
}

