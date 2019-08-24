// cSystemManager.cpp

#include "cSystemManager.h"

#include <Entity\Systems\cDrawSystem.h>
#include <Entity\Systems\cCameraSystem.h>
#include <Entity\Systems\cLightSystem.h>
#include <Entity\Systems\cAnimationSystem.h>
#include <Entity\Systems\cAudioSystem.h>
#include <Entity\Systems\cControllerSystem.h>
#include <Entity\Systems\cBehaviourSystem.h>
#include <Entity\Systems\cNeuralNetworkSystem.h>


cSystemManager::cSystemManager()
{
	return;
}

cSystemManager::~cSystemManager()
{
	// Cleanup
	this->DeleteSystems();

	return;
}

void cSystemManager::CreateSystems()
{
	cDrawSystem* pDrawSystem = new cDrawSystem();
	cCameraSystem* pCameraSystem = new cCameraSystem();
	cLightSystem* pLightSystem = new cLightSystem();
	cAnimationSystem* pAnimationSystem = new cAnimationSystem();
	cAudioSystem* pAudioSystem = new cAudioSystem();
	cControllerSystem* pControllerSystem = new cControllerSystem();
	cBehaviourSystem* pBehaviourSystem = new cBehaviourSystem();
	cNeuralNetworkSystem* pNeuralNetworkSystem = new cNeuralNetworkSystem();

	this->_AddSystem(pDrawSystem);
	this->_AddSystem(pCameraSystem);
	this->_AddSystem(pLightSystem);
	this->_AddSystem(pAnimationSystem);
	this->_AddSystem(pAudioSystem);
	this->_AddSystem(pControllerSystem);
	this->_AddSystem(pBehaviourSystem);
	this->_AddSystem(pNeuralNetworkSystem);
}

void cSystemManager::UpdateSystems(double deltaTime)
{
	// Update all the systems from the vector
	for (unsigned int i = 0; i < this->_vecSystems.size(); i++)
	{
		this->_vecSystems[i]->UpdateSystem(deltaTime);
	}
}

void cSystemManager::DeleteSystems()
{
	// Delete all the systems from the vector and clear it.
	for (unsigned int i = 0; i < this->_vecSystems.size(); i++)
	{
		delete this->_vecSystems[i];
	}

	this->_vecSystems.clear();
}

void cSystemManager::_AddSystem(iSystem* ecsSystem)
{
	this->_vecSystems.push_back(ecsSystem);
}
