// cSystemManager.cpp

#include "cSystemManager.h"
#include "cECSPoolContainer.h"

#include <Entity\Systems\cDrawSystem.h>
#include <Entity\Systems\cCameraSystem.h>
#include <Entity\Systems\cLightSystem.h>
#include <Entity\Systems\cAnimationSystem.h>
#include <Entity\Systems\cAudioSystem.h>
#include <Entity\Systems\cControllerSystem.h>
#include <Entity\Systems\cBehaviourSystem.h>
#include <Entity\Systems\cNeuralNetworkSystem.h>
#include <Entity\Components\sCameraComponent.h>
#include <Engine\cEngine.hpp>
#include <Engine\cECSControl.h>

#include <Engine\Graphics\cGraphicsControl.h>


cSystemManager::cSystemManager()
{
	this->CreateSystems();
	this->_bIsLightUnilocSet = false;

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
	this->CreateRenderSystems();
	this->CreateAudioSystems();
	this->CreateControllerSystems();
	this->CreateAISystems();
}

void cSystemManager::CreateRenderSystems()
{
	cCameraSystem* pCameraSystem = new cCameraSystem();
	cLightSystem* pLightSystem = new cLightSystem();
	cDrawSystem* pDrawSystem = new cDrawSystem();

	GLuint shaderProgramID = cEngine::GetShaderProgramIDByName("simpleShader");
	pCameraSystem->SetupSystem(shaderProgramID);
	pDrawSystem->SetupSystem(shaderProgramID, cEngine::GetGraphicsControl()->GetVaoMeshManager(), cEngine::GetGraphicsControl()->GetTextureManager());

	this->_vecRenderSystems.push_back(pCameraSystem);
	this->_vecRenderSystems.push_back(pLightSystem);
	this->_vecRenderSystems.push_back(pDrawSystem);
}

void cSystemManager::CreateAudioSystems()
{
	cAudioSystem* pAudioSystem = new cAudioSystem();
	pAudioSystem->LoadSounds();
	this->_vecAudioSystems.push_back(pAudioSystem);
}

void cSystemManager::CreateControllerSystems()
{
	cControllerSystem* pControllerSystem = new cControllerSystem();
	cAnimationSystem* pAnimationSystem = new cAnimationSystem();

	this->_vecControllerSystems.push_back(pControllerSystem);
	this->_vecControllerSystems.push_back(pAnimationSystem);
}

void cSystemManager::CreateAISystems()
{
	cBehaviourSystem* pBehaviourSystem = new cBehaviourSystem();
	cNeuralNetworkSystem* pNeuralNetworkSystem = new cNeuralNetworkSystem();

	this->_vecAISystems.push_back(pBehaviourSystem);
	this->_vecAISystems.push_back(pNeuralNetworkSystem);
}

void cSystemManager::UpdateSystems(double deltaTime)
{
	// Update all the systems from the vector
	//this->_vecRenderSystems[0]->UpdateSystem(deltaTime);
}

void cSystemManager::UpdateRender(int width, int height, GLuint shaderProgramID, double deltaTime)
{
	// Update Rendering. Pay attention to the creation order.

	// Update light first to set light and colour values
	cLightSystem* lightSystem = static_cast<cLightSystem*>(this->_vecRenderSystems[1]);
	lightSystem->CopyLightValuesToShader(shaderProgramID);

	// Update camera to update lookat and camera vectors
	cCameraSystem* cameraSystem = static_cast<cCameraSystem*>(this->_vecRenderSystems[0]);
	cameraSystem->UpdateSystem(deltaTime);

	// Draw on the screen

	// FBO Process
	// 1. Clear everything and set the framebuffer output to the main framebuffer
	// 2. Bind 1 texture and draw scene into it 
	// 4. Draw a single quad with the scene quad
	// 5. ???
	// 6. Profit

	cDrawSystem* drawSystem = static_cast<cDrawSystem*>(this->_vecRenderSystems[2]);
	sEntity* FBO = cEngine::FindEntityByName("FBO");
	sCameraComponent* worldCamera = cEngine::FindEntityByName("WorldCamera")->GetComponent<sCameraComponent>();
	drawSystem->DrawCall1Pass(worldCamera->eyePosition, worldCamera->GetLookAtInWorldSpace(), width, height, shaderProgramID, deltaTime);
	drawSystem->DrawCall2Pass(FBO, width, height, shaderProgramID);
}

void cSystemManager::UpdateAudio(double deltaTime)
{
	// Update Audio
	cAudioSystem* audioSystem = static_cast<cAudioSystem*>(this->_vecAudioSystems[0]);
	audioSystem->UpdateSystem(deltaTime);
}

void cSystemManager::UpdateControllers(double deltaTime)
{
	// Update controllers
	//this->_vecControllerSystems[0]->UpdateSystem(deltaTime);
}

void cSystemManager::UpdateAI(double deltaTime)
{
	// Update AIs
	
}

void cSystemManager::DeleteSystems()
{
	// Delete all the systems from the vector and clear it.
	for (unsigned int i = 0; i < this->_vecRenderSystems.size(); i++)
	{
		delete this->_vecRenderSystems[i];
	}
	for (unsigned int i = 0; i < this->_vecAudioSystems.size(); i++)
	{
		delete this->_vecAudioSystems[i];
	}
	for (unsigned int i = 0; i < this->_vecControllerSystems.size(); i++)
	{
		delete this->_vecControllerSystems[i];
	}
	for (unsigned int i = 0; i < this->_vecAISystems.size(); i++)
	{
		delete this->_vecAISystems[i];
	}
}
