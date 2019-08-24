// cTestScene.cpp

#include "cTestScene.h"
#include "nSceneHelper.h"
#include <Engine\cEntityManager.h>
#include <Mesh\cVaoMeshManager.h>
#include <Scene\nSceneHelper.h>
#include <Math\nQuaternionHelper.h>

#include <Interfaces\Physics\iPlaneShape.h>
#include <Interfaces\Physics\iSphereShape.h>
#include <Interfaces\Physics\iBoxShape.h>
#include <Interfaces\Physics\iCylinderShape.h>
#include <Interfaces\Physics\iCapsuleShape.h>

#include <Entity\Components\sMeshComponent.h>
#include <Entity\Components\sLightComponent.h>
#include <Entity\Components\sCameraComponent.h>

#include <Interfaces\Physics\iCompoundBody.h>
#include <Interfaces\Controllers\i3DPersonController.h>
#include <Mesh\Assimp\cAssimpMeshLoader.h>

#include <Pools\cPoolManager.h>



cTestScene::cTestScene(unsigned long long sceneID)
{
	this->_sceneID = sceneID;
	this->_sceneState = eSceneStates::UNLOADED;

	return;
}

cTestScene::~cTestScene()
{
	this->EndScene();

	return;
}

unsigned long long cTestScene::GetSceneID()
{
	return this->_sceneID;
}

void cTestScene::CreateScene(cVaoMeshManager* pVaoMeshManager, cEntityManager* pEntityManager, cAssimpMeshLoader* pAssimpLoader)
{
	{
		sEntity* worldCamera = pEntityManager->GenerateEntityOfType("CameraPrefab");
		this->_pSceneEntities.push_back(worldCamera);
	}
	{
		sEntity* skybox = pEntityManager->GenerateEntityOfType("SkyboxPrefab");
		this->_pSceneEntities.push_back(skybox);
	}
	//{
	//	//sEntity* ground = cEntityManager::GenerateEntity("PhysicsPrefab");
	//	sEntity* terrain = pEntityManager->GenerateEntityOfType("PhysicsTerrainPrefab");
	//	this->_pSceneEntities.push_back(terrain);
	//}
	//{
	//	sEntity* TwoSidedQuad = pEntityManager->GenerateEntityOfType("EmptyPrefab");
	//	sMeshComponent* meshC = TwoSidedQuad->AddComponent<sMeshComponent>();

	//	TwoSidedQuad->SetIsActive(true);
	//	TwoSidedQuad->SetName("2SidedQuad");
	//	TwoSidedQuad->SetTag("FBO");
	//	TwoSidedQuad->transform->SetWorldPosition(glm::vec3(0.0f));

	//	meshC->meshName = "1x1_2Tri_Quad_2_Sided_xyz_n_uv.ply";
	//	sModelDrawInfo* meshInfo = new sModelDrawInfo();
	//	meshInfo->meshFileName = meshC->meshName;
	//	pVaoMeshManager->FindDrawInfoByName(*meshInfo);
	//	meshC->meshInfo = meshInfo;
	//	meshC->bIsAffectedByLights = false;
	//	meshC->bIsVisible = false;
	//	meshC->materialDiffuse = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	//	this->_pSceneEntities.push_back(TwoSidedQuad);
	//}
	//{
	//	sEntity* TVBody = pEntityManager->GenerateEntityOfType("EmptyPrefab");
	//	sMeshComponent* meshC = TVBody->AddComponent<sMeshComponent>();

	//	TVBody->SetIsActive(true);
	//	TVBody->SetName("TVBody");
	//	TVBody->transform->SetWorldPosition(glm::vec3(0.0f, 10.0f, 50.0f));
	//	TVBody->transform->SetWorldRotation(nQuaternionHelper::AdjOrientationEulerAngles(TVBody->transform->GetWorldRotation(), glm::vec3(0.0f, 180.0f, 0.0f), true));

	//	meshC->meshName = "RetroTV.edited.bodyonly.ply";
	//	sModelDrawInfo* meshInfo = new sModelDrawInfo();
	//	meshInfo->meshFileName = meshC->meshName;
	//	pVaoMeshManager->FindDrawInfoByName(*meshInfo);
	//	meshC->meshInfo = meshInfo;
	//	meshC->bIsAffectedByLights = true;
	//	meshC->materialDiffuse = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	//	sTextureInfo groundTexture;
	//	groundTexture.texName = "WWWood.bmp";
	//	groundTexture.texStrength = 1.0f;
	//	meshC->vecTextures.push_back(groundTexture);

	//	this->_pSceneEntities.push_back(TVBody);
	//}
	/*{
		sEntity* TVScreen = cEntityManager::GenerateEntity("EmptyPrefab");
		sMeshComponent* meshC = TVScreen->AddTComponent<sMeshComponent*>(MESH_COMPONENT);

		TVScreen->bIsActive = true;
		TVScreen->entityName = "TVScreen";
		TVScreen->entityTag = "FBO2";
		TVScreen->transform->position = glm::vec3(1.0f, 10.0f, 48.0f);
		TVScreen->transform->qOrientation = nQuaternionHelper::AdjOrientationEulerAngles(TVScreen->transform->qOrientation, glm::vec3(0.0f, 180.0f, 0.0f), true);

		meshC->meshName = "RetroTV.edited.screenonly.ply";
		sModelDrawInfo* meshInfo = new sModelDrawInfo();
		meshInfo->meshFileName = meshC->meshName;
		pVaoMeshManager->FindDrawInfoByName(*meshInfo);
		meshC->bIsVisible = false;
		meshC->meshInfo = meshInfo;
		meshC->bIsAffectedByLights = true;
		meshC->materialDiffuse = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	}*/
	/*{
		sEntity* sphere = pEntityManager->GenerateEntityOfType("EmptyPrefab");
		sMeshComponent* meshC = sphere->AddComponent<sMeshComponent>();

		sphere->SetIsActive(true);
		sphere->SetName("Sphere");
		sphere->SetTag("Rigidbody");
		sphere->transform->SetWorldPosition(glm::vec3(0.0f, 0.0f, 100.0f));

		nPhysics::sRigidbodyDef rbDef;
		rbDef.initialPosition = sphere->transform->GetWorldPosition();
		rbDef.initialRotation = sphere->transform->GetWorldRotation();
		nPhysics::iSphereShape* sphereShape = pPhysicsFactory->CreateSphereShape(2.5f);
		rbDef.collisionShape = sphereShape;
		rbDef.mass = 1.0f;
		nPhysics::iRigidbody* rigidbodyC = sphere->AddComponent<nPhysics::iRigidbody>(rbDef);

		meshC->meshName = "physicsSphere.ply";
		sModelDrawInfo* meshInfo = new sModelDrawInfo();
		meshInfo->meshFileName = meshC->meshName;
		pVaoMeshManager->FindDrawInfoByName(*meshInfo);
		meshC->meshInfo = meshInfo;
		meshC->bIsAffectedByLights = true;
		meshC->nonUniformScale = glm::vec3(5.0f);
		meshC->materialDiffuse = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		sTextureInfo groundTexture;
		groundTexture.texName = "stone.bmp";
		groundTexture.texStrength = 1.0f;
		meshC->vecTextures.push_back(groundTexture);

		this->_pSceneEntities.push_back(sphere);
	}
	{
		sEntity* box = pEntityManager->GenerateEntityOfType("EmptyPrefab");
		sMeshComponent* meshC = box->AddComponent<sMeshComponent>();

		box->SetIsActive(true);
		box->SetName("Box");
		box->SetTag("Rigidbody");
		box->transform->SetWorldPosition(glm::vec3(100.0f, 0.0f, 100.0f));

		nPhysics::sRigidbodyDef rbDef;
		rbDef.initialPosition = box->transform->GetWorldPosition();
		rbDef.initialRotation = box->transform->GetWorldRotation();
		nPhysics::iBoxShape* sphereShape = pPhysicsFactory->CreateBoxShape(glm::vec3(4.0f));
		rbDef.collisionShape = sphereShape;
		rbDef.mass = 1.0f;
		nPhysics::iRigidbody* rigidbodyC = box->AddComponent<nPhysics::iRigidbody>(rbDef);

		meshC->meshName = "physicsBox.ply";
		sModelDrawInfo* meshInfo = new sModelDrawInfo();
		meshInfo->meshFileName = meshC->meshName;
		pVaoMeshManager->FindDrawInfoByName(*meshInfo);
		meshC->meshInfo = meshInfo;
		meshC->bIsAffectedByLights = true;
		meshC->nonUniformScale = glm::vec3(4.0f);
		meshC->materialDiffuse = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		sTextureInfo groundTexture;
		groundTexture.texName = "WWRock.bmp";
		groundTexture.texStrength = 1.0f;
		meshC->vecTextures.push_back(groundTexture);

		this->_pSceneEntities.push_back(box);
	}
	{
		sEntity* cylinder = pEntityManager->GenerateEntityOfType("EmptyPrefab");
		sMeshComponent* meshC = cylinder->AddComponent<sMeshComponent>();

		cylinder->SetIsActive(true);
		cylinder->SetName("Cylinder");
		cylinder->SetTag("Rigidbody");
		cylinder->transform->SetWorldPosition(glm::vec3(-100.0f, 300.0f, 100.0f));

		nPhysics::sRigidbodyDef rbDef;
		rbDef.initialPosition = cylinder->transform->GetWorldPosition();
		rbDef.initialRotation = cylinder->transform->GetWorldRotation();
		nPhysics::iCylinderShape* cylinderShape = pPhysicsFactory->CreateCylinderShape(glm::vec3(4.0f, 8.0f, 4.0f));
		rbDef.collisionShape = cylinderShape;
		rbDef.mass = 1.0f;
		nPhysics::iRigidbody* rigidbodyC = cylinder->AddComponent<nPhysics::iRigidbody>(rbDef);

		meshC->meshName = "physicsCylinder.ply";
		sModelDrawInfo* meshInfo = new sModelDrawInfo();
		meshInfo->meshFileName = meshC->meshName;
		pVaoMeshManager->FindDrawInfoByName(*meshInfo);
		meshC->meshInfo = meshInfo;
		meshC->bIsAffectedByLights = true;
		meshC->nonUniformScale = glm::vec3(4.0f);
		meshC->materialDiffuse = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		meshC->refractionRate = 1.0f;

		sTextureInfo groundTexture;
		groundTexture.texName = "goldFlan.bmp";
		groundTexture.texStrength = 1.0f;
		meshC->vecTextures.push_back(groundTexture);

		this->_pSceneEntities.push_back(cylinder);
	}
	{
		sEntity* capsule = pEntityManager->GenerateEntityOfType("EmptyPrefab");
		sMeshComponent* meshC = capsule->AddComponent<sMeshComponent>();

		capsule->SetIsActive(true);
		capsule->SetName("Capsule");
		capsule->SetTag("Rigidbody");
		capsule->transform->SetWorldPosition(glm::vec3(-100.0f, 300.0f, -100.0f));

		nPhysics::sRigidbodyDef rbDef;
		rbDef.initialPosition = capsule->transform->GetWorldPosition();
		rbDef.initialRotation = capsule->transform->GetWorldRotation();
		nPhysics::iCapsuleShape* capsuleShape = pPhysicsFactory->CreateCapsuleShape(2.0f, 8.0f);
		rbDef.collisionShape = capsuleShape;
		rbDef.mass = 1.0f;
		nPhysics::iRigidbody* rigidbodyC = capsule->AddComponent<nPhysics::iRigidbody>(rbDef);

		meshC->meshName = "physicsCapsule.ply";
		sModelDrawInfo* meshInfo = new sModelDrawInfo();
		meshInfo->meshFileName = meshC->meshName;
		pVaoMeshManager->FindDrawInfoByName(*meshInfo);
		meshC->meshInfo = meshInfo;
		meshC->bIsAffectedByLights = true;
		meshC->nonUniformScale = glm::vec3(4.0f);
		meshC->materialDiffuse = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		meshC->reflectionRate = 1.0f;

		sTextureInfo groundTexture;
		groundTexture.texName = "fireFlan.bmp";
		groundTexture.texStrength = 1.0f;
		meshC->vecTextures.push_back(groundTexture);

		this->_pSceneEntities.push_back(capsule);
	}
	{
		sEntity* Ely = pEntityManager->GenerateEntityOfType("PlayerPrefab");
		this->_pSceneEntities.push_back(Ely);
	}*/
	//{
	//	sEntity* RPGChan = cEntityManager::GenerateEntity("EmptyPrefab");
	//	sMeshComponent* meshC = RPGChan->AddTComponent<sMeshComponent*>(MESH_COMPONENT);
	//	s3DPersonControllerComponent* controllerC = RPGChan->AddTComponent<s3DPersonControllerComponent*>(TDPERSONCONTROLLER_COMPONENT);
	//	controllerC->SetIsControlling(false);

	//	RPGChan->bIsActive = true;
	//	RPGChan->entityName = "RPGChan";
	//	RPGChan->entityTag = "Player";
	//	RPGChan->threatTag = "Enemy";
	//	RPGChan->transform->position = glm::vec3(40.0f, 0.0f, 40.0f);
	//	RPGChan->transform->qOrientation = nQuaternionHelper::AdjOrientationEulerAngles(RPGChan->transform->qOrientation, glm::vec3(0.0f, 0.0f, 0.0f), true);
	//	RPGChan->transform->respawnPosition = glm::vec3(0.0f, 0.0f, 0.0f);

	//	meshC->meshName = "RPG-Character(FBX2013).FBX";
	//	sModelDrawInfo* meshInfo = new sModelDrawInfo();
	//	meshInfo->meshFileName = meshC->meshName;
	//	pVaoMeshManager->FindDrawInfoByName(*meshInfo);
	//	meshC->meshInfo = meshInfo;
	//	meshC->nonUniformScale = glm::vec3(0.1f);
	//	meshC->materialDiffuse = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	//	meshC->bIsAffectedByLights = true;
	//	meshC->bIsVisible = true;
	//	sTextureInfo fahrenheitTexture;
	//	fahrenheitTexture.texName = "goldFlan.bmp";
	//	fahrenheitTexture.texStrength = 1.0f;
	//	meshC->vecTextures.push_back(fahrenheitTexture);

	//	nPhysics::sRigidbodyDef rbDef;
	//	rbDef.initialPosition = RPGChan->transform->position;
	//	rbDef.initialRotation = RPGChan->transform->qOrientation;
	//	rbDef.centerOfMassOffset = glm::vec3(0.0f, 12.0f, 0.0f);
	//	nPhysics::iCylinderShape* cylinderShape = pEntityManager->GetPhysicsFactory()->CreateCylinderShape(glm::vec3(12.0f, 24.0f, 12.0f));
	//	rbDef.collisionShape = cylinderShape;
	//	rbDef.mass = 0.0f;
	//	rbDef.bIsKinematic = true;
	//	rbDef.bUseGravity = true;
	//	nPhysics::iRigidbody* rigidbodyC = RPGChan->AddTComponent<nPhysics::iRigidbody*>(RIGIDBODY_COMPONENT, rbDef);

	//	controllerC->AddAttack(0, "RPGAttack", 3.0f);
	//	controllerC->idleName = "RPGIdle";
	//	controllerC->walkName = "RPGWalk";
	//	controllerC->runName = "RPGRun";

	//	meshC->SetAnimator();
	//	cAssimpMeshLoader::sAnimationInfo animInfo;

	//	pAssimpLoader->FindAnimationByName("RPGIdle", animInfo);
	//	meshC->animator->AddAnimation(animInfo); // Idle...
	//	pAssimpLoader->FindAnimationByName("RPGWalk", animInfo);
	//	meshC->animator->AddAnimation(animInfo); // Idle...
	//	pAssimpLoader->FindAnimationByName("RPGRun", animInfo);
	//	meshC->animator->AddAnimation(animInfo); // Run...
	//	pAssimpLoader->FindAnimationByName("RPGAttack", animInfo);
	//	meshC->animator->AddAnimation(animInfo); // Attack...
	//}
	/*{
		sEntity* light1 = pEntityManager->GenerateEntityOfType("LightPrefab");
		sEntity* light2 = pEntityManager->GenerateEntityOfType("LightPrefab");
		sLightComponent* light2C = light2->GetComponent<sLightComponent>();
		light2C->SetLightType(light2C->POINT_LIGHT);
		light2C->SetDiffuseColour(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		light2C->SetPosition(glm::vec4(0.0f, 100.0f, 0.0f, 1.0f));
		light2C->SetConstantAttenuation(1.18f);
		light2C->SetLinearAttenuation(0.02f);
		light2C->SetQuadraticAttenuation(0.0f);
		light2C->bisWireframeOn = true;

		this->_pSceneEntities.push_back(light1);
		this->_pSceneEntities.push_back(light2);
	}
	{
		sEntity* wireframe = pEntityManager->GenerateEntityOfType("StaticPrefab");
		wireframe->SetName("WireframeSphere");
		wireframe->SetTag("Static");

		sMeshComponent* meshC = wireframe->GetComponent<sMeshComponent>();
		meshC->meshName = "lowPolySphere.ply";
		sModelDrawInfo* meshInfo = new sModelDrawInfo();
		meshInfo->meshFileName = meshC->meshName;
		pVaoMeshManager->FindDrawInfoByName(*meshInfo);
		meshC->meshInfo = meshInfo;
		meshC->nonUniformScale = glm::vec3(1.0f);
		meshC->materialDiffuse = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		meshC->bIsAffectedByLights = false;
		meshC->bIsWireframe = true;
		meshC->bIsVisible = true;

		this->_pSceneEntities.push_back(wireframe);
	}
	{
		sEntity* debugSphere = pEntityManager->GenerateEntityOfType("StaticPrefab");
		debugSphere->SetName("DebugSphere");
		debugSphere->SetTag("Static");
		debugSphere->SetIsActive(false);

		sMeshComponent* meshC = debugSphere->GetComponent<sMeshComponent>();
		meshC->meshName = "debugSphere.ply";
		sModelDrawInfo* meshInfo = new sModelDrawInfo();
		meshInfo->meshFileName = meshC->meshName;
		pVaoMeshManager->FindDrawInfoByName(*meshInfo);
		meshC->meshInfo = meshInfo;
		meshC->nonUniformScale = glm::vec3(1.0f);
		meshC->materialDiffuse = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
		meshC->bIsAffectedByLights = true;
		meshC->bIsWireframe = false;
		meshC->bIsVisible = false;

		this->_pSceneEntities.push_back(debugSphere);
	}
	{
		sEntity* AABB = pEntityManager->GenerateEntityOfType("StaticPrefab");
		sMeshComponent* meshC = AABB->GetComponent<sMeshComponent>();

		AABB->SetIsActive(false);
		AABB->SetName("AABB");
		AABB->SetTag("Static");

		meshC->meshName = "AABB.ply";
		sModelDrawInfo* meshInfo = new sModelDrawInfo();
		meshInfo->meshFileName = meshC->meshName;
		pVaoMeshManager->FindDrawInfoByName(*meshInfo);
		meshC->meshInfo = meshInfo;
		meshC->nonUniformScale = glm::vec3(1.0f);
		meshC->materialDiffuse = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		meshC->bIsAffectedByLights = false;
		meshC->bIsWireframe = true;
		meshC->bIsVisible = false;

		this->_pSceneEntities.push_back(AABB);
	}

	{
		sEntity* BallAndChain = pEntityManager->GenerateEntityOfType("CompoundPhysicsPrefab");
		nPhysics::iCompoundBody* cb = BallAndChain->GetComponent<nPhysics::iCompoundBody>();
		std::vector<nPhysics::iRigidbody*> vecBodies;
		for (unsigned int i = 0; i < 4; i++)
		{
			sEntity* sphere = pEntityManager->GenerateEntityOfType("EmptyPrefab");
			sMeshComponent* meshC = sphere->AddComponent<sMeshComponent>();

			sphere->SetIsActive(true);
			sphere->SetName("Sphere");
			sphere->SetTag("Rigidbody");
			sphere->transform->SetWorldPosition(glm::vec3(200.0f, 50.0f - i * (10.0f), 100.0f));

			nPhysics::sRigidbodyDef rbDef;
			rbDef.initialPosition = sphere->transform->GetWorldPosition();
			rbDef.initialRotation = sphere->transform->GetWorldRotation();
			nPhysics::iSphereShape* sphereShape = pPhysicsFactory->CreateSphereShape(5.0f);
			rbDef.collisionShape = sphereShape;
			rbDef.mass = 1.0f;
			if (i == 0)
			{
				rbDef.mass = 0.0f;
				rbDef.bIsKinematic = true;
			}
			nPhysics::iRigidbody* rigidbodyC = sphere->AddComponent<nPhysics::iRigidbody>(rbDef);

			meshC->meshName = "physicsSphere.ply";
			sModelDrawInfo* meshInfo = new sModelDrawInfo();
			meshInfo->meshFileName = meshC->meshName;
			pVaoMeshManager->FindDrawInfoByName(*meshInfo);
			meshC->meshInfo = meshInfo;
			meshC->bIsAffectedByLights = true;
			meshC->nonUniformScale = glm::vec3(10.0f);
			meshC->materialDiffuse = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

			sTextureInfo groundTexture;
			groundTexture.texName = "stone.bmp";
			groundTexture.texStrength = 1.0f;
			meshC->vecTextures.push_back(groundTexture);

			cb->AddCompoundBody(rigidbodyC);
			vecBodies.push_back(rigidbodyC);
		}

		cb->AddConstraint(nPhysics::CONSTRAINT_BALL_AND_SOCKET, 0, 1, glm::vec3(0.0f), glm::vec3(vecBodies[0]->GetTransformComponent()->GetWorldPosition() - vecBodies[1]->GetTransformComponent()->GetWorldPosition()));
		cb->AddConstraint(nPhysics::CONSTRAINT_BALL_AND_SOCKET, 1, 2, glm::vec3(0.0f), glm::vec3(vecBodies[1]->GetTransformComponent()->GetWorldPosition() - vecBodies[2]->GetTransformComponent()->GetWorldPosition()));
		cb->AddConstraint(nPhysics::CONSTRAINT_BALL_AND_SOCKET, 2, 3, glm::vec3(0.0f), glm::vec3(vecBodies[2]->GetTransformComponent()->GetWorldPosition() - vecBodies[3]->GetTransformComponent()->GetWorldPosition()));

		this->_pSceneEntities.push_back(BallAndChain);
	}

	{
		sEntity* Hinge = pEntityManager->GenerateEntityOfType("CompoundPhysicsPrefab");
		nPhysics::iCompoundBody* cb = Hinge->GetComponent<nPhysics::iCompoundBody>();
		std::vector<nPhysics::iRigidbody*> vecBodies;
		for (unsigned int i = 0; i < 1; i++)
		{
			sEntity* sphere = pEntityManager->GenerateEntityOfType("EmptyPrefab");
			sMeshComponent* meshC = sphere->AddComponent<sMeshComponent>();

			sphere->SetIsActive(true);
			sphere->SetName("Box");
			sphere->SetTag("Rigidbody");
			sphere->transform->SetWorldPosition(glm::vec3(-30.0f, 20.0f, 20.0f));

			nPhysics::sRigidbodyDef rbDef;
			rbDef.initialPosition = sphere->transform->GetWorldPosition();
			rbDef.initialRotation = sphere->transform->GetWorldRotation();
			nPhysics::iBoxShape* boxShape = pPhysicsFactory->CreateBoxShape(glm::vec3(10.0f));
			rbDef.collisionShape = boxShape;
			rbDef.mass = 1.0f;
			nPhysics::iRigidbody* rigidbodyC = sphere->AddComponent<nPhysics::iRigidbody>(rbDef);

			meshC->meshName = "physicsBox.ply";
			sModelDrawInfo* meshInfo = new sModelDrawInfo();
			meshInfo->meshFileName = meshC->meshName;
			pVaoMeshManager->FindDrawInfoByName(*meshInfo);
			meshC->meshInfo = meshInfo;
			meshC->bIsAffectedByLights = true;
			meshC->nonUniformScale = glm::vec3(10.0f);
			meshC->materialDiffuse = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

			sTextureInfo groundTexture;
			groundTexture.texName = "stone.bmp";
			groundTexture.texStrength = 1.0f;
			meshC->vecTextures.push_back(groundTexture);

			cb->AddCompoundBody(rigidbodyC);
			vecBodies.push_back(rigidbodyC);
		}

		cb->AddConstraint(nPhysics::CONSTRAINT_HINGE, 0, glm::vec3(5.0f, 5.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		this->_pSceneEntities.push_back(Hinge);
	}
	{
		sEntity* masterSword = pEntityManager->GenerateEntityOfType("StaticPrefab");
		masterSword->SetName("MasterSword");
		masterSword->SetTag("Default");
		masterSword->transform->SetWorldPosition(glm::vec3(50.0f, 40.0f, 70.0f));
		masterSword->SetIsActive(true);

		sMeshComponent* meshC = masterSword->GetComponent<sMeshComponent>();
		meshC->meshName = "masterSword.ply";
		sModelDrawInfo* meshInfo = new sModelDrawInfo();
		meshInfo->meshFileName = meshC->meshName;
		pVaoMeshManager->FindDrawInfoByName(*meshInfo);
		meshC->meshInfo = meshInfo;
		meshC->nonUniformScale = glm::vec3(1.0f);
		meshC->materialDiffuse = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		meshC->bIsAffectedByLights = true;
		meshC->bIsWireframe = false;
		meshC->bIsVisible = true;

		nPhysics::sRigidbodyDef rbDef;
		rbDef.initialPosition = masterSword->transform->GetWorldPosition();
		rbDef.initialRotation = masterSword->transform->GetWorldRotation();
		nPhysics::iCapsuleShape* capsuleShape = pPhysicsFactory->CreateCapsuleShape(2.0f, 8.0f);
		rbDef.collisionShape = capsuleShape;
		rbDef.bIsKinematic = true;
		rbDef.mass = 0.0f;
		nPhysics::iRigidbody* rigidbodyC = masterSword->AddComponent<nPhysics::iRigidbody>(rbDef);

		sTextureInfo groundTexture;
		groundTexture.texName = "stone.bmp";
		groundTexture.texStrength = 1.0f;
		meshC->vecTextures.push_back(groundTexture);

		this->_pSceneEntities.push_back(masterSword);
	}*/

	//this->AwakeScene();
	//this->StartScene(pEntityManager, pPhysicsFactory);
}

void cTestScene::AwakeScene()
{
}

void cTestScene::StartScene(cEntityManager* pEntityManager)
{
	//cPoolManager* pool = cPoolManager::GetInstance();

	//sEntity* ely = pool->GetEntityByName("Ely");
	//sEntity* masterSword = pool->GetEntityByName("MasterSword");
	//nPhysics::iRigidbody* swordBody = masterSword->GetComponent<nPhysics::iRigidbody>();
	//i3DPersonController* controller = ely->GetComponent<i3DPersonController>();
	//controller->AddSocketBody(swordBody);
	//nPhysics::iSphereShape* handSphere = pPhysicsFactory->CreateSphereShape(5.0f);
	//controller->SetGhostBody(handSphere);

	//sEntity* worldCamera = pool->GetEntityByName("WorldCamera");

	//sCameraComponent* cameraC = worldCamera->GetComponent<sCameraComponent>();
	////cameraC->eyePosition = glm::vec3(0.0f, 50.0f, 0.0f);
	////cameraC->_lookAt = glm::vec3(0.0f, -1.0f, 0.0f);

	//cameraC->SetTarget(ely, glm::vec3(0.0f, 30.0f, -30.0f), 5.0f, 20.0f);
	//cameraC->SetLookAtOffset(glm::vec3(0.0f, 20.0f, 1.0f));
	//cameraC->SetCameraFollowType(sCameraComponent::eFollowTypes::ZELDA);
}

void cTestScene::UpdateScene()
{
}

void cTestScene::EndScene()
{

}

sEntity* CreateOneObject(const std::string& prefabName, const std::string& objName) 
{
	return nullptr;
}

void cTestScene::_CreateObjects()
{
	
}

void cTestScene::_CreatePhysics()
{

}

sEntity* cTestScene::GetEntityByName(const std::string& entityName)
{
	for (unsigned int i = 0; i < this->_pSceneEntities.size(); i++)
	{
		sEntity* entity = this->_pSceneEntities[i];

		if (entity->GetName() == entityName)
		{
			return entity;
		}
	}

	return nullptr;
}
