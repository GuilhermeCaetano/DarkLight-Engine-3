// cEntityBuilder.cpp

#include "cEntityBuilder.h"
#include <Entity\sEntity.h>
#include <Math\nQuaternionHelper.h>

#include <Entity\Components\sMeshComponent.h>
#include <Entity\Components\sCameraComponent.h>
#include <Entity\Components\sLightComponent.h>
#include <Entity\Components\sAudioComponent.h>

#include <Interfaces\Physics\sRigidbodyDef.h>
#include <Interfaces\Physics\sSoftbodyDef.h>
#include <Interfaces\Physics\sCompoundbodyDef.h>
#include <Interfaces\Physics\iRigidbody.h>
#include <Interfaces\Physics\iSoftbody.h>
#include <Interfaces\Physics\iCompoundBody.h>
#include <Interfaces\Controllers\i3DPersonController.h>


cEntityBuilder::cEntityBuilder()
{
	return;
}

cEntityBuilder::~cEntityBuilder()
{
	return;
}

void cEntityBuilder::BuildEntity(const std::string& prefabType, sEntity* prefabToBuild)
{
	if (prefabType == "CameraPrefab")
	{
		this->_BuildCameraPrefab(prefabToBuild);
	}
	else if (prefabType == "StaticPrefab")
	{
		this->_BuildStaticPrefab(prefabToBuild);
	}
	else if (prefabType == "LightPrefab")
	{
		this->_BuildLightPrefab(prefabToBuild);
	}
	else if (prefabType == "SkyboxPrefab")
	{
		this->_BuildSkyboxPrefab(prefabToBuild);
	}
	else if (prefabType == "PlayerPrefab")
	{
		this->_BuildPlayerPrefab(prefabToBuild);
	}
	else if (prefabType == "PatrolEnemyPrefab")
	{
		this->_BuildPatrolEnemyPrefab(prefabToBuild);
	}
	else if (prefabType == "PhysicsPrefab")
	{
		this->_BuildRigidbodyPrefab(prefabToBuild);
	}
	else if (prefabType == "PhysicsTerrainPrefab")
	{
		this->_BuildPhysicsTerrain(prefabToBuild);
	}
	else if (prefabType == "CompoundPhysicsPrefab")
	{
		this->_BuildCompoundbodyPrefab(prefabToBuild);
	}
}

void cEntityBuilder::_BuildStaticPrefab(sEntity* prefabToBuild)
{
	sMeshComponent* meshC = prefabToBuild->AddComponent<sMeshComponent>();

	prefabToBuild->SetIsActive(true);
	prefabToBuild->SetName("Entity");
	prefabToBuild->SetTag("Default");

	meshC->nonUniformScale = glm::vec3(1.0f);
	meshC->materialDiffuse = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	meshC->bIsAffectedByLights = true;
	meshC->bIsWireframe = false;
	meshC->bIsVisible = true;
}

void cEntityBuilder::_BuildSkyboxPrefab(sEntity* prefabToBuild)
{
	prefabToBuild->SetIsActive(true);
	prefabToBuild->SetName("Skybox");
	prefabToBuild->SetTag("Skybox");
	prefabToBuild->transform->SetWorldPosition(glm::vec3(0.0f));

	sMeshComponent* meshC = prefabToBuild->AddComponent<sMeshComponent>();
	meshC->meshName = "skyboxSphere.ply";
	meshC->SetMesh(meshC->meshName);
	meshC->nonUniformScale = glm::vec3(1000.0f);
	meshC->materialDiffuse = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	meshC->bIsAffectedByLights = false;
	meshC->bIsVisible = true;
}

void cEntityBuilder::_BuildCameraPrefab(sEntity* prefabToBuild)
{
	prefabToBuild->SetIsActive(true);
	prefabToBuild->SetName("WorldCamera");
	prefabToBuild->SetTag("Camera");
	prefabToBuild->transform->SetWorldPosition(glm::vec3(0.0f));

	sCameraComponent* cameraC = prefabToBuild->AddComponent<sCameraComponent>();
	cameraC->eyePosition = glm::vec3(0.0f, 20.0f, -50.0f);
	cameraC->qOrientation = nQuaternionHelper::AdjOrientationEulerAngles(cameraC->qOrientation, glm::vec3(0.0f, 0.0f, 0.0f), true);
	cameraC->bFollowTarget = false;

	sAudioComponent* audioC = prefabToBuild->AddComponent<sAudioComponent>();
	audioC->bPlayOnStart = true;
	audioC->filename = "BGM_FFX-SightOfSpira.wav";
	audioC->volume = 1.0f;
	audioC->bLoop = true;
}

void cEntityBuilder::_BuildLightPrefab(sEntity* prefabToBuild)
{
	sLightComponent* lightC = prefabToBuild->AddComponent<sLightComponent>();

	prefabToBuild->SetIsActive(true);
	prefabToBuild->SetName("Light");
	prefabToBuild->SetTag("Static");
	
	lightC->SetLightType(lightC->DIRECTIONAL_LIGHT);
	lightC->SetDiffuseColour(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	lightC->SetPosition(glm::vec4(0.0f, 1000.0f, 0.0f, 1.0f));
	lightC->bisWireframeOn = true;
}

void cEntityBuilder::_BuildPlayerPrefab(sEntity* prefabToBuild)
{ 
	// PLAYER
	sMeshComponent* meshC = prefabToBuild->AddComponent<sMeshComponent>();

	prefabToBuild->SetIsActive(true);
	prefabToBuild->SetName("Ely");
	prefabToBuild->SetTag("Player");
	prefabToBuild->transform->SetWorldPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	prefabToBuild->transform->SetWorldRotation(nQuaternionHelper::AdjOrientationEulerAngles(prefabToBuild->transform->GetWorldRotation(), glm::vec3(0.0f, 0.0f, 0.0f), true));

	meshC->meshName = "ely.fbx";
	meshC->SetMesh(meshC->meshName);
	meshC->nonUniformScale = glm::vec3(0.14f);
	meshC->materialDiffuse = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	meshC->bIsAffectedByLights = true;
	meshC->bIsVisible = true;
	meshC->bUseVertexColour = false;
	sTextureInfo fahrenheitTexture;
	fahrenheitTexture.texName = "ely.bmp";
	fahrenheitTexture.texStrength = 1.0f;
	meshC->vecTextures.push_back(fahrenheitTexture);

	/*nPhysics::sRigidbodyDef rbDef;
	rbDef.initialPosition = prefabToBuild->transform->position;
	rbDef.initialRotation = prefabToBuild->transform->qOrientation;
	rbDef.centerOfMassOffset = glm::vec3(0.0f, 12.0f, 0.0f);
	nPhysics::iCylinderShape* cylinderShape = nEngineGlobals::pPhysicsFactory->CreateCylinderShape(glm::vec3(12.0f, 24.0f, 12.0f));
	rbDef.collisionShape = cylinderShape;
	rbDef.mass = 1000.0f;
	rbDef.bIsKinematic = true;
	rbDef.bUseGravity = true;
	nPhysics::iRigidbody* rigidbodyC = prefabToBuild->AddTComponent<nPhysics::iRigidbody*>(RIGIDBODY_COMPONENT, rbDef);

	i3DPersonController* controller =  prefabToBuild->AddTComponent<i3DPersonController*>(TDPERSONCONTROLLER_COMPONENT);
	controller->SetCollisionBody(rigidbodyC);
	nPhysics::iSphereShape* handSphere = nEngineGlobals::pPhysicsFactory->CreateSphereShape(5.0f);
	controller->SetGhostBody(handSphere);*/

	// Add animations
	meshC->SetAnimator();
	meshC->animator->AddAnimation("Idle"); // Idle...
	meshC->animator->AddAnimation("Walk"); // Idle...
	meshC->animator->AddAnimation("Run"); // Run...
	meshC->animator->AddAnimation("JumpUp"); // Run...
	meshC->animator->AddAnimation("JumpAir"); // Run...
	meshC->animator->AddAnimation("JumpLand1"); // Run...
	meshC->animator->AddAnimation("JumpLand2"); // Run...
	meshC->animator->AddAnimation("Attack1"); // Attack...
}

void cEntityBuilder::_BuildRigidbodyPrefab(sEntity* prefabToBuild)
{
	sMeshComponent* meshC = prefabToBuild->AddComponent<sMeshComponent>();

	prefabToBuild->SetIsActive(true);
	prefabToBuild->SetName("Rigidbody");
	prefabToBuild->SetTag("Rigidbody");
	prefabToBuild->transform->SetWorldPosition(glm::vec3(0.0f));

	/*nPhysics::sRigidbodyDef rbDef;
	rbDef.initialPosition = prefabToBuild->transform->position;
	rbDef.initialRotation = prefabToBuild->transform->qOrientation;
	nPhysics::iPlaneShape* planeShape = nEngineGlobals::pPhysicsFactory->CreatePlaneShape(glm::vec3(0.0f, 1.0f, 0.0f), 0.0f);
	rbDef.collisionShape = planeShape;
	rbDef.normal = glm::vec3(0.0f, 1.0f, 0.0f);
	rbDef.mass = 0.0f;
	nPhysics::iRigidbody* rigidbodyC = prefabToBuild->AddTComponent<nPhysics::iRigidbody*>(RIGIDBODY_COMPONENT, rbDef);*/

	meshC->meshName = "physicsPlane.ply";
	meshC->SetMesh(meshC->meshName);
	meshC->bIsAffectedByLights = true;
	meshC->nonUniformScale = glm::vec3(100000.0f);
	meshC->materialDiffuse = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	sTextureInfo groundTexture;
	groundTexture.texName = "WWPawprintIslandGrass.bmp";
	groundTexture.texStrength = 1.0f;
	meshC->vecTextures.push_back(groundTexture);
}

void cEntityBuilder::_BuildCompoundbodyPrefab(sEntity* prefabToBuild)
{
	nPhysics::sCompoundbodyDef def;
	def.constrainType = nPhysics::CONSTRAINT_BALL_AND_SOCKET;
	nPhysics::iCompoundBody* compoundBody = prefabToBuild->AddComponent<nPhysics::iCompoundBody>(def);

	prefabToBuild->SetIsActive(true);
	prefabToBuild->SetName("Compoundbody");
	prefabToBuild->SetTag("Compoundbody");
	prefabToBuild->transform->SetWorldPosition(glm::vec3(0.0f));

	compoundBody->SetTransformComponent(prefabToBuild->transform);
}

void cEntityBuilder::_BuildPhysicsTerrain(sEntity* prefabToBuild)
{
	sMeshComponent* meshC = prefabToBuild->AddComponent<sMeshComponent>();

	prefabToBuild->SetIsActive(true);
	prefabToBuild->SetName("Terrain");
	prefabToBuild->SetTag("Rigidbody");
	prefabToBuild->transform->SetWorldPosition(glm::vec3(0.0f, 0.0f, 0.0f));

	meshC->meshName = "castle.ply";
	meshC->SetMesh(meshC->meshName);
	meshC->bIsAffectedByLights = true;
	meshC->nonUniformScale = glm::vec3(1.0f);
	meshC->materialDiffuse = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	nPhysics::sRigidbodyDef rbDef;
	rbDef.initialPosition = prefabToBuild->transform->GetWorldPosition();
	rbDef.initialRotation = prefabToBuild->transform->GetWorldRotation();
	rbDef.bIsKinematic = true;

	/*nPhysics::iBvhTriangleMeshShape* terrainShape = nEngineGlobals::pPhysicsFactory->CreateBvhTriangleMeshShape(meshC->meshInfo, true, true);
	rbDef.collisionShape = terrainShape;
	rbDef.mass = 0.0f;
	nPhysics::iRigidbody* rigidbodyC = prefabToBuild->AddTComponent<nPhysics::iRigidbody*>(RIGIDBODY_COMPONENT, rbDef);*/
	//rigidbodyC->SetCollisionLayerMask(eCollisionLayerMask::GROUND);

	sTextureInfo groundTexture;
	groundTexture.texName = "angkorWatTex.bmp";
	groundTexture.texStrength = 1.0f;
	meshC->vecTextures.push_back(groundTexture);
}

void cEntityBuilder::_BuildPatrolEnemyPrefab(sEntity* prefabToBuild)
{

}

//void cEntityBuilder::_BuildPhysicsTerrain(sEntity* prefabToBuild)
//{
//	sMeshComponent* meshC = prefabToBuild->AddTComponent<sMeshComponent*>(MESH_COMPONENT);
//
//	prefabToBuild->bIsActive = true;
//	prefabToBuild->entityName = "Terrain";
//	prefabToBuild->entityTag = "Rigidbody";
//	prefabToBuild->transform->position = glm::vec3(0.0f, 0.0f, 0.0f);
//	prefabToBuild->transform->respawnPosition = glm::vec3(0.0f, 0.0f, 0.0f);
//
//	meshC->meshName = "castle.ply";
//	sModelDrawInfo* meshInfo = new sModelDrawInfo();
//	meshInfo->meshFileName = meshC->meshName;
//	this->_pVaoMeshManager->FindDrawInfoByName(*meshInfo);
//	meshC->meshInfo = meshInfo;
//	meshC->bIsAffectedByLights = true;
//	meshC->nonUniformScale = glm::vec3(1.0f);
//	meshC->materialDiffuse = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
//
//	nPhysics::sRigidbodyDef rbDef;
//	rbDef.initialPosition = prefabToBuild->transform->position;
//	rbDef.initialRotation = prefabToBuild->transform->qOrientation;
//	rbDef.bIsKinematic = true;
//
//	float maxY = 170.0f;
//	float minY = -10.0f;
//	float medium = (fabs(maxY) + fabs(minY)) * 0.5f;
//
//	int num = meshInfo->numberOfVertices;
//	for (unsigned int i = 0; i < num; i++)
//	{
//		//meshInfo->heightMap[i] -= 100.0f;
//	}
//
//	nPhysics::iHeightfieldTerrainShape* terrainShape = this->_pEntityManager->GetPhysicsFactory()->CreateHeightfieldTerrainShape(865, 754, meshInfo->heightMap, 1.0f, -90.79f, 90.79f, 1, nPhysics::PHY_FLOAT, true);
//	rbDef.collisionShape = terrainShape;
//	rbDef.mass = 0.0f;
//	nPhysics::iRigidbody* rigidbodyC = prefabToBuild->AddTComponent<nPhysics::iRigidbody*>(RIGIDBODY_COMPONENT, rbDef);
//
//	sTextureInfo groundTexture;
//	groundTexture.texName = "angkorWatTex.bmp";
//	groundTexture.texStrength = 1.0f;
//	meshC->vecTextures.push_back(groundTexture);
//}