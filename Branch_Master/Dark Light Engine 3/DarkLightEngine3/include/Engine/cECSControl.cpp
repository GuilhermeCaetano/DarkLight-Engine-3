// cECSControl.cpp

#include "cECSControl.h"
#include "cEntityManager.h"
#include "cComponentManager.h"
#include "cSystemManager.h"
#include <Interfaces\Physics\iPhysicsFactory.h>


// Initialize static variables
cEntityManager* cECSControl::_pEntityManager = nullptr;
cComponentManager* cECSControl::_pComponentManager = nullptr;
cSystemManager* cECSControl::_pSystemManager = nullptr;
nPhysics::iPhysicsFactory* cECSControl::_pPhysicsFactory = nullptr;

cECSControl::cECSControl()
{
	return;
}

cECSControl::~cECSControl()
{
	this->DestroyECS();
	return;
}

void cECSControl::CreateECS()
{
	// Create ECS managers
	_pEntityManager = new cEntityManager();
	_pComponentManager = new cComponentManager();
	_pSystemManager = new cSystemManager();
}

void cECSControl::DestroyECS()
{
	// Delete ECS managers
	if (_pEntityManager)
	{
		delete _pEntityManager;
		_pEntityManager = nullptr;
	}
	if (_pComponentManager)
	{
		delete _pComponentManager;
		_pComponentManager = nullptr;
	}
	if (_pSystemManager)
	{
		delete _pSystemManager;
		_pSystemManager = nullptr;
	}
}

void cECSControl::UpdateECS(double deltaTime)
{
	// Update the managers
	//this->_pEntityManager->UpdateEntities();
	this->_pSystemManager->UpdateSystems(deltaTime);
}

cEntityManager* cECSControl::GetEntityManager()
{
	return _pEntityManager;
}

cComponentManager* cECSControl::GetComponentManager()
{
	return _pComponentManager;
}

cSystemManager* cECSControl::GetSystemManager()
{
	return _pSystemManager;
}

nPhysics::iPhysicsFactory* cECSControl::GetPhysicsFactory()
{
	return _pPhysicsFactory;
}

void cECSControl::SetPhysicsFactory(nPhysics::iPhysicsFactory* pPhysicsFactory)
{
	_pPhysicsFactory = pPhysicsFactory;
}
