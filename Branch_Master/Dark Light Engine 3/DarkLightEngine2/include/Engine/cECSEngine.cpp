// cECSEngine.cpp

#include "cECSEngine.h"
#include "cECSControl.h"
#include "Graphics\cGraphicsControl.h"
#include "cEngine.hpp"
#include "cECSPoolContainer.h"
#include "ComponentHeaders.h"
#include "Configuration\cConfigManager.h"
#include "Assets\cAssetManager.h"
#include "Assets\nAssetPack_0.h"
#include "cEngineStateLoader.h"

#include <Shader\cShaderManager.h>
#include <Mesh\cVaoMeshManager.h>
#include <Mesh\Assimp\cAssimpMeshLoader.h>
#include <Texture\cTextureManager.h>
#include <Interfaces\Physics\iPhysicsFactory.h>
#include <Scene\nSceneHelper.h>

#include <Scene\cTestScene.h>


cECSControl* gECSControl;
cGraphicsControl* gGraphicsControl;
double cECSEngine::_deltaTime = 0.0;

cECSEngine::cECSEngine()
{
	// Create Engine objects
	this->_pEngine = new cEngine();
	gECSControl = new cECSControl();
	gGraphicsControl = new cGraphicsControl();
	this->_pConfigManager = new cConfigManager();
	this->_pEngineStateLoader = new cEngineStateLoader();
	this->_pECSPoolContainer = cECSPoolContainer::GetInstance();

	this->_pInitialScene = new cTestScene(0);

	/*this->_pShaderManager = new cShaderManager();
	this->_pVaoMeshManager = new cVaoMeshManager();
	this->_pAssimpLoader = new cAssimpMeshLoader();*/
	//this->_pAssetManager = new cAssetManager(this->_pVaoMeshManager, this->_pAssimpLoader);
	//this->_pTextureManager = new cTextureManager();

	return;
}

cECSEngine::~cECSEngine()
{
	return;
}

void cECSEngine::StartEngine()
{
	// Create the ECS
	gECSControl->CreateECS();

	// Load external configurations
	this->LoadExternalConfigurations();

	// Create graphics components
	gGraphicsControl->CreateGraphics();

	// Init graphics
	this->InitGraphicsPipeline();

	// Load initially used assets
	this->LoadAssets();

	// Pre allocate memory for ecs
	this->PreAllocateEngineMemory();

	// Start FPS calculation
	this->_lastTime = glfwGetTime();
	this->_deltaTime = this->_lastTime;
	printf("\nStarted time count.\n");

	// Test Unload from GPU
	//gGraphicsControl->GetAssetManager()->UnloadAssetPackFromGPU("StarterPack");

	bool breakpoint = true;
}

void cECSEngine::LoadExternalConfigurations()
{
	// Load physics from dll specified on the chosen config file
	if (!this->_pConfigManager->ConfigurePhysics("config.json"))
	{
		printf("Error! Couldn't load engine configurations!\n");
		return;
	}

	// Get factory for physics
	gECSControl->SetPhysicsFactory(this->_pConfigManager->GetPhysicsFactory());

	printf("External configurations loaded. Using %s.\n\n", this->_pConfigManager->GetPhysicsType().c_str());
}

void cECSEngine::LoadAssets()
{
	// Generate starting asset packs and load them
	double startTime = glfwGetTime();
	GLuint shaderProgramID = gGraphicsControl->GetShaderManager()->GetShaderProgramIDByName("simpleShader");
	cAssetManager::sAssetpack* assetPack_0 = nAssets::GenerateAssetPack_0(gGraphicsControl->GetAssetManager());
	gGraphicsControl->GetAssetManager()->LoadOneAssetPackIntoCPU(assetPack_0->_packID, 5);
	while (!assetPack_0->CheckPackIsInCPU())
	{
		// This isn't blocking, we could let the program load it on the background,
		// but these are essential to the engine, so wait for them for now.
	}
	gGraphicsControl->GetAssetManager()->LoadOneAssetPackIntoGPU(assetPack_0->_packID, shaderProgramID); // This is blocking, but GPU loading is fast.
	while (!assetPack_0->CheckPackIsInGPU())
	{
		gGraphicsControl->GetAssetManager()->Update(shaderProgramID);
	}
	double endTime = glfwGetTime() - startTime;
	printf("Loaded asset pack %s in %.1f seconds.\n", assetPack_0->_packID.c_str(), endTime);

	// TODO: Move texture loading to asset manager too
	startTime = glfwGetTime();
	nScene::LoadTexturesThreaded(gGraphicsControl->GetTextureManager());
	nScene::LoadCubemaps(gGraphicsControl->GetTextureManager());

	endTime = glfwGetTime() - startTime;
	printf("Loaded essential textures and cubemaps in %.1f seconds.\n", endTime);
}

void cECSEngine::PreAllocateEngineMemory()
{
	cECSPoolContainer* ecsPool = cECSPoolContainer::GetInstance();
	sMemoryChunk* chunkA = ecsPool->CreateMemoryChunk();
	printf("\nCreated Memory ChunkA.\n");
	chunkA->AllocateMemory<sEntity>(5000);
	
	sMemoryChunk* chunkB = ecsPool->CreateMemoryChunk();
	printf("\nCreated Memory ChunkB.\n");
	chunkB->AllocateMemory<sLocationComponent>(500);
	chunkB->AllocateMemory<sCameraComponent>(500);
	chunkB->AllocateMemory<sMeshComponent>(500);
	chunkB->AllocateMemory<sSkinnedMeshComponent>(500);
	chunkB->AllocateMemory<sLightComponent>(500);
	chunkB->AllocateMemory<sAudioComponent>(500);
}

void cECSEngine::InitGraphicsPipeline()
{
	// Load all shaders used by the engine
	gGraphicsControl->GetShaderManager()->SetBasePath("assets/shaders/");
	this->_pConfigManager->ConfigureShader(gGraphicsControl->GetShaderManager(), "vertex01.glsl", "fragment01.glsl", "simpleShader");
	this->_pConfigManager->ConfigureShader(gGraphicsControl->GetShaderManager(), "vertexText.glsl", "fragmentText.glsl", "textShader");
}

void cECSEngine::SaveCurrentEngineState(const std::string& stateFilename)
{
	// Save engine state into json
	this->_pEngineStateLoader->SaveEngineState_Json();
}

void cECSEngine::LoadLastEngineState(const std::string& stateFilename)
{
	// Set state filename and load the last engine state
	this->_pEngineStateLoader->SetEngineStateSaveName(stateFilename);

	// Check if there is an engine save, otherwise create first scene
	if (!this->_pEngineStateLoader->CheckEngineStateExists(stateFilename))
	{
		this->_pInitialScene->CreateScene(gGraphicsControl->GetVaoMeshManager(), gECSControl->GetEntityManager(), gGraphicsControl->GetAssimpMeshLoader());
	}
	else
	{
		this->_pEngineStateLoader->LoadEngineState_Json(gECSControl->GetEntityManager(), this->_pConfigManager->GetPhysicsFactory());
	}
}

void cECSEngine::UpdateEngine()
{
	// Calculate frame time
	double currentTime = glfwGetTime();
	this->_deltaTime = currentTime - this->_lastTime;
	this->_lastTime = currentTime;

	// Update ECS
	gECSControl->UpdateECS(this->_deltaTime);
}

void cECSEngine::StopEngine()
{
	// Delete Engine objects
	cECSPoolContainer::GetInstance()->CleanAllMemoryChunks();

	delete this->_pEngine;
	delete gECSControl;
	delete gGraphicsControl;
	delete this->_pConfigManager;
	delete this->_pEngineStateLoader;
	delete this->_pECSPoolContainer;

	//delete this->_pShaderManager;
	//delete this->_pVaoMeshManager;
	//delete this->_pAssetManager;
	//delete this->_pAssimpLoader;
	//delete this->_pTextureManager;

	delete this->_pInitialScene;
}
