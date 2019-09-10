// cComponentManager.cpp

#include "cComponentManager.h"


cComponentManager::cComponentManager()
{
	this->_pComponentFactory = new cComponentFactory();

	return;
}

cComponentManager::~cComponentManager()
{
	this->_Cleanup();

	return;
}

void cComponentManager::_Cleanup()
{
	if (this->_pComponentFactory)
	{
		delete this->_pComponentFactory;
		this->_pComponentFactory = nullptr;
	}
	if (this->_pPhysicsFactory)
	{
		delete this->_pPhysicsFactory;
		this->_pPhysicsFactory = nullptr;
	}
}
