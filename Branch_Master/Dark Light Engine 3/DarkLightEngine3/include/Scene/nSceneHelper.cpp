// SceneHelper.cpp

#include <Scene\nSceneHelper.h>
#include <Entity\Components\sMeshComponent.h>
#include <Entity\Components\sLocationComponent.h>
#include <Pools\cPoolManager.h>
#include <Math\nQuaternionHelper.h>
#include <Math\nMathHelper.h>
#include <Texture\cTextureManager.h>
#include <Engine\cEntityManager.h>

#include <Interfaces\Physics\sRigidbodyDef.h>


namespace nScene
{
	bool LoadOneMeshIntoCPU(const std::string& meshName, void** modelInfo, cVaoMeshManager* pVaoMeshManager)
	{
		// Load model information from file
		sModelDrawInfo* newModel = new sModelDrawInfo();
		newModel->meshFileName = meshName;

		// Set model mesh into the CPU
		if (!pVaoMeshManager->LoadThreadModelIntoCPU(*newModel))
		{
			//printf("Error loading the %s mesh!", newModel.meshFileName.c_str());
			//printf("%s", pShaderManager->GetLastError().c_str());
			return false;
		}

		*modelInfo = newModel;
		return true;
	}

	bool LoadOneTMeshIntoGPU(void* modelInfo, cVaoMeshManager* pVaoMeshManager, GLuint shaderProgramID)
	{
		sModelDrawInfo* meshInfo = (sModelDrawInfo*)modelInfo;
		return pVaoMeshManager->LoadThreadModelIntoGPU(*meshInfo, shaderProgramID);
	}

	bool LoadOneSkinnedMeshIntoCPU(const std::string& meshName, void** modelInfo, cAssimpMeshLoader* pAssimpLoader, cVaoMeshManager* pVaoMeshManager)
	{
		// Load model information from file
		sModelDrawInfo* newModel = new sModelDrawInfo();
		newModel->meshFileName = meshName;

		std::string error = "";
		if (!pAssimpLoader->LoadMeshFromFile(meshName, *newModel))
		{
			// Failed to load skinned mesh
			printf("Error loading skinned mesh %s! Error %s!\n", meshName.c_str(), error.c_str());
			return false;
		}

		if (!pAssimpLoader->LoadAssimpDrawInfo(*newModel))
		{
			// Failed to load skinned mesh
			printf("Error loading skinned mesh %s draw info! Error %s!\n", meshName.c_str(), error.c_str());
			return false;
		}

		*modelInfo = newModel;
		return true;
	}

	bool LoadOneTSkinnedMeshIntoGPU(void* modelInfo, cVaoMeshManager* pVaoMeshManager, cAssimpMeshLoader* pAssimpLoader, GLuint shaderProgramID)
	{
		sModelDrawInfo* meshInfo = (sModelDrawInfo*)modelInfo;
		return pAssimpLoader->ConvertAssimpToVAO(pVaoMeshManager, *meshInfo, shaderProgramID);
	}

	void LoadOneMeshIntoVAO(const std::string& meshName, GLuint shaderProgramID, cVaoMeshManager* pVaoMeshManager)
	{
		// Set model mesh into the VAO
		sModelDrawInfo newModel;
		newModel.meshFileName = meshName;

		if (!pVaoMeshManager->LoadModelIntoVAO(newModel, shaderProgramID))
		{
			//printf("Error loading the %s mesh!", newModel.meshFileName.c_str());
			//printf("%s", pShaderManager->GetLastError().c_str());
		}
	}

	bool LoadOneAnimationIntoCPU(const std::string& animationName, const std::string& friendlyName, cAssimpMeshLoader* pAssimpLoader)
	{
		std::string error = "";
		if (!pAssimpLoader->LoadMeshAnimation(animationName, friendlyName))
		{
			// Failed to load animation
			printf("Error loading animation %s! Error %s!\n", animationName.c_str(), error.c_str());
			return false;
		}
		else
		{
			// Success in loading animation
			//printf("Loaded animation %s!\n", animationName.c_str());
			return true;
		}
	}

	//void LoadThreadedMeshesIntoVAO(GLuint shaderProgram, cVaoMeshManager* pVaoMeshManager, cAssimpMeshLoader* pAssimpLoader, cShaderManager* pShaderManager)
	//{

	//}

	//void LoadOneSkinnedMeshIntoVAO(const std::string& meshName, cAssimpMeshLoader* pAssimpLoader, cVaoMeshManager* pVaoMeshManager, GLuint shaderProgramID)
	//{
	//	// Set model mesh into the VAO
	//	sModelDrawInfo meshInfo;

	//	std::string error = "";
	//	if (!pAssimpLoader->LoadMeshFromFile(meshName, meshInfo))
	//	{
	//		// Failed to load skinned mesh
	//		printf("Error loading skinned mesh %s! Error %s!\n", meshName.c_str(), error.c_str());
	//	}
	//	else
	//	{
	//		// Success in loading skinned mesh
	//		printf("Loaded skinned mesh %s!\n", meshName.c_str());
	//	}

	//	meshInfo.friendlyName = "RPG-Chan";

	//	if (!pAssimpLoader->LoadAssimpDrawInfo(meshInfo))
	//	{
	//		// Failed to load skinned mesh
	//		printf("Error loading skinned mesh %s draw info! Error %s!\n", meshName.c_str(), error.c_str());
	//	}
	//	else
	//	{
	//		// Success loading draw info
	//		printf("Loaded %s draw info!\n", meshName.c_str());
	//	}

	//	pAssimpLoader->ConvertAssimpToVAO(pVaoMeshManager, meshInfo, shaderProgramID);
	//}

	//void LoadMeshesIntoVAO(GLuint shaderProgramID, cVaoMeshManager* pVaoMeshManager, cAssimpMeshLoader* pAssimpLoader, cShaderManager* pShaderManager)
	//{
	//	LoadOneMeshIntoVAO("castle.ply", shaderProgramID, pVaoMeshManager);
	//	LoadOneMeshIntoVAO("lucy.ply", shaderProgramID, pVaoMeshManager);
	//	//LoadOneMeshIntoVAO("fahrenheit.ply", shaderProgramID, pVaoMeshManager);
	//	//LoadOneMeshIntoVAO("cube.ply", shaderProgramID, pVaoMeshManager);
	//	//LoadOneMeshIntoVAO("dalek2005_xyz_uv_res_2.ply", shaderProgramID, pVaoMeshManager);

	//	LoadOneMeshIntoVAO("physicsSphere.ply", shaderProgramID, pVaoMeshManager);
	//	LoadOneMeshIntoVAO("physicsPlane.ply", shaderProgramID, pVaoMeshManager);
	//	LoadOneMeshIntoVAO("physicsBox.ply", shaderProgramID, pVaoMeshManager);
	//	LoadOneMeshIntoVAO("physicsTetrahedron.ply", shaderProgramID, pVaoMeshManager);
	//	LoadOneMeshIntoVAO("physicsCylinder.ply", shaderProgramID, pVaoMeshManager);
	//	LoadOneMeshIntoVAO("physicsCapsule.ply", shaderProgramID, pVaoMeshManager);

	//	LoadOneMeshIntoVAO("debugSphere.ply", shaderProgramID, pVaoMeshManager);
	//	LoadOneMeshIntoVAO("lowPolySphere.ply", shaderProgramID, pVaoMeshManager);
	//	LoadOneMeshIntoVAO("skyboxSphere.ply", shaderProgramID, pVaoMeshManager);
	//	LoadOneMeshIntoVAO("AABB.ply", shaderProgramID, pVaoMeshManager);
	//	LoadOneMeshIntoVAO("RetroTV.edited.bodyonly.ply", shaderProgramID, pVaoMeshManager);
	//	LoadOneMeshIntoVAO("RetroTV.edited.screenonly.ply", shaderProgramID, pVaoMeshManager);
	//	LoadOneMeshIntoVAO("1x1_2Tri_Quad_2_Sided_xyz_n_uv.ply", shaderProgramID, pVaoMeshManager);

	//	LoadOneSkinnedMeshIntoVAO("ely.fbx", pAssimpLoader, pVaoMeshManager, shaderProgramID);
	//	LoadOneAnimationIntoVAO("ely-Idle.fbx", "Idle", pAssimpLoader);
	//	LoadOneAnimationIntoVAO("ely-Walk.fbx", "Walk", pAssimpLoader);
	//	LoadOneAnimationIntoVAO("ely-Run.fbx", "Run", pAssimpLoader);
	//	LoadOneAnimationIntoVAO("ely-Strafe-Right.fbx", "StrafeRight", pAssimpLoader);
	//	LoadOneAnimationIntoVAO("ely-Strafe-Left.fbx", "StrafeLeft", pAssimpLoader);
	//	LoadOneAnimationIntoVAO("ely-Roll.fbx", "Roll", pAssimpLoader);
	//	LoadOneAnimationIntoVAO("ely-Jump-Up.fbx", "JumpUp", pAssimpLoader);
	//	LoadOneAnimationIntoVAO("ely-Jump-Air.fbx", "JumpAir", pAssimpLoader);
	//	LoadOneAnimationIntoVAO("ely-Outward-Slash.fbx", "Attack_1", pAssimpLoader);

	//	LoadOneSkinnedMeshIntoVAO("RPG-Character(FBX2013).FBX", pAssimpLoader, pVaoMeshManager, shaderProgramID);
	//	LoadOneAnimationIntoVAO("RPG-Character-Unarmed-Idle.FBX", "RPGIdle", pAssimpLoader);
	//	LoadOneAnimationIntoVAO("RPG-Character_Unarmed-Walk(FBX2013).FBX", "RPGWalk", pAssimpLoader);
	//	LoadOneAnimationIntoVAO("RPG-Character-Unarmed-Run-Forward.FBX", "RPGRun", pAssimpLoader);
	//	LoadOneAnimationIntoVAO("RPG-Character_Unarmed-Jump(FBX2013).FBX", "RPGJump", pAssimpLoader);
	//	LoadOneAnimationIntoVAO("RPG-Character_Unarmed-Fall(FBX2013).FBX", "RPGFall", pAssimpLoader);
	//	LoadOneAnimationIntoVAO("RPG-Character-Unarmed-Attack-R1.FBX", "RPGAttack", pAssimpLoader);
	//	LoadOneAnimationIntoVAO("RPG-Character_Unarmed-Attack-L3(FBX2013).FBX", "RPGAttackL3", pAssimpLoader);
	//	LoadOneAnimationIntoVAO("RPG-Character_Unarmed-Attack-R3(FBX2013).FBX", "RPGAttackR3", pAssimpLoader);

	//	// At this point, meshes are in GPU
	//	printf("All models were loaded successfully into the GPU.\n");
	//}

	//void LoadSkinnedMeshesIntoVAO(cAssimpMeshLoader* pAssimpLoader, cVaoMeshManager* pVaoMeshManager, GLuint shaderProgramID)
	//{
	//	LoadOneSkinnedMeshIntoVAO("RPG-Character(FBX2013).FBX", pAssimpLoader, pVaoMeshManager, shaderProgramID);
	//}

	void LoadTexture(std::string textureName, cTextureManager::eTexType texType, cTextureManager* pTextureManager, bool useMipMap = true)
	{
		switch (texType)
		{
		case cTextureManager::LOW_RES:
			if (!pTextureManager->Create2DLowResTexture_BMP(textureName, useMipMap))
			{
				printf("Error loading the %s texture!", textureName.c_str());
			}
			break;
		case cTextureManager::HIGH_RES:
			if (!pTextureManager->CreateThreaded2DHighResTexture_BMP(textureName, useMipMap))
			{
				printf("Error loading the %s texture!", textureName.c_str());
			}
			break;
		case cTextureManager::DEFAULT:
			if (!pTextureManager->Create2DTexture_BMP(textureName, "", useMipMap))
			{
				printf("Error loading the %s texture!", textureName.c_str());
			}
			break;
		default:
			break;
		}
	}

	void LoadTextures(cTextureManager* pTextureManager)
	{
		pTextureManager->SetBasePath("assets/textures");

		// Load low res textures
		//LoadTexture("angkorWatTex.bmp", cTextureManager::DEFAULT, pTextureManager);
		//LoadTexture("fahrenheit.bmp", cTextureManager::DEFAULT, pTextureManager);
		LoadTexture("WWPawprintIslandGrass.bmp", cTextureManager::HIGH_RES, pTextureManager);
		LoadTexture("WWRock.bmp", cTextureManager::HIGH_RES, pTextureManager);
		LoadTexture("stone.bmp", cTextureManager::HIGH_RES, pTextureManager);
		LoadTexture("goldFlan.bmp", cTextureManager::HIGH_RES, pTextureManager);
		LoadTexture("fireFlan.bmp", cTextureManager::HIGH_RES, pTextureManager);
		LoadTexture("ely.bmp", cTextureManager::HIGH_RES, pTextureManager);
		LoadTexture("WWWood.bmp", cTextureManager::HIGH_RES, pTextureManager);
		//LoadTexture("crosshair.bmp", cTextureManager::DEFAULT, pTextureManager);
	}

	void LoadTexturesThreaded(cTextureManager* pTextureManager)
	{
		pTextureManager->SetBasePath("assets/textures");

		// Load low res textures
		//LoadTexture("angkorWatTex.bmp", cTextureManager::DEFAULT, pTextureManager);
		//LoadTexture("fahrenheit.bmp", cTextureManager::DEFAULT, pTextureManager);
		//LoadTexture("WWPawprintIslandGrass.bmp", cTextureManager::LOW_RES, pTextureManager);
		LoadTexture("angkorWatTex.bmp", cTextureManager::LOW_RES, pTextureManager);
		//LoadTexture("shangai.bmp", cTextureManager::LOW_RES, pTextureManager);
		//LoadTexture("lightIsland.bmp", cTextureManager::LOW_RES, pTextureManager);
		//LoadTexture("WWRock.bmp", cTextureManager::LOW_RES, pTextureManager);
		//LoadTexture("stone.bmp", cTextureManager::LOW_RES, pTextureManager);
		//LoadTexture("goldFlan.bmp", cTextureManager::LOW_RES, pTextureManager);
		//LoadTexture("fireFlan.bmp", cTextureManager::LOW_RES, pTextureManager);
		//LoadTexture("ely.bmp", cTextureManager::LOW_RES, pTextureManager);
		//LoadTexture("WWWood.bmp", cTextureManager::LOW_RES, pTextureManager);

		// Load high res textures
		pTextureManager->CreateTextureTripleBuffer();
		//LoadTexture("WWPawprintIslandGrass.bmp", cTextureManager::HIGH_RES, pTextureManager);
		LoadTexture("angkorWatTex.bmp", cTextureManager::HIGH_RES, pTextureManager);
		//LoadTexture("shangai.bmp", cTextureManager::HIGH_RES, pTextureManager);
		//LoadTexture("lightIsland.bmp", cTextureManager::HIGH_RES, pTextureManager);
		//LoadTexture("WWRock.bmp", cTextureManager::HIGH_RES, pTextureManager);
		//LoadTexture("stone.bmp", cTextureManager::HIGH_RES, pTextureManager);
		//LoadTexture("goldFlan.bmp", cTextureManager::HIGH_RES, pTextureManager);
		//LoadTexture("fireFlan.bmp", cTextureManager::HIGH_RES, pTextureManager);
		//LoadTexture("ely.bmp", cTextureManager::HIGH_RES, pTextureManager);
		//LoadTexture("WWWood.bmp", cTextureManager::HIGH_RES, pTextureManager);
	}

	void LoadCubemaps(cTextureManager* pTextureManager)
	{
		// Load the cube map
		pTextureManager->SetBasePath("assets/cubemaps");
		std::string errorString = "";

		// Might need to reverse these, depending on the program that generated the cubemap
		if (pTextureManager->CreateCubeTextureFromBMPFiles("BrightSkyCubeMap",
			"TropicalSunnyDayLeft2048.bmp", "TropicalSunnyDayRight2048.bmp",
			"TropicalSunnyDayDown2048.bmp", "TropicalSunnyDayUp2048.bmp",
			"TropicalSunnyDayFront2048.bmp", "TropicalSunnyDayBack2048.bmp", true, errorString))
		{
			//printf("Loaded the Bright Sky cube map OK.\n");
		}
		else
		{
			printf("Error: Bright Sky cube map DIDN't load!\n");
		}

		if (pTextureManager->CreateCubeTextureFromBMPFiles("SpaceBoxCubeMap",
			"SpaceBox_right1_posX.bmp", "SpaceBox_left2_negX.bmp",
			"SpaceBox_top3_posY.bmp", "SpaceBox_bottom4_negY.bmp",
			"SpaceBox_front5_posZ.bmp", "SpaceBox_back6_negZ.bmp", true, errorString))
		{
			//printf("Loaded the Space Box cube map OK.\n");
		}
		else
		{
			printf("Error: Space Box cube map DIDN't load!\n");
		}

		if (errorString == "")
		{
			printf("\nLoaded all cubemaps.\n");
		}
	}

	//void LoadCompoundBodyLine2Line(cEntityManager* pEntityManager)
	//{
	//	sEntity* Sphere0 = pEntityManager->GenerateEntityOfType("PhysicsEntity");
	//	{
	//		
	//		sMeshComponent* meshC = static_cast<sMeshComponent*>(Sphere0->GetComponent(eComponentTypes::MESH_COMPONENT));

	//		nPhysics::sRigidbodyDef rbDef;
	//		rbDef.initialPosition = glm::vec3(40.0f, 56.0f, 40.0f);
	//		nPhysics::iSphereShape* sphereShape = pEntityManager->GetPhysicsFactory()->CreateSphereShape(4.0f);
	//		rbDef.collisionShape = sphereShape;
	//		rbDef.mass = 1.0f;
	//		Sphere0->AddComponent(eComponentTypes::RIGIDBODY_COMPONENT, rbDef);

	//		Sphere0->bIsActive = true;
	//		Sphere0->entityName = "PlaneBottom";
	//		Sphere0->entityTag = "Static";
	//		Sphere0->transform->position = glm::vec3(40.0f, 56.0f, 40.0f);

	//		meshC->meshName = "physicsSphere.ply";
	//		sModelDrawInfo* meshInfo = new sModelDrawInfo();
	//		meshInfo->meshFileName = meshC->meshName;
	//		cMeshManager::GetInstance()->FindMeshByName(*meshInfo);
	//		meshC->meshInfo = meshInfo;
	//		meshC->bIsAffectedByLights = true;
	//		meshC->nonUniformScale = glm::vec3(8.0f);
	//		meshC->materialDiffuse = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

	//		sTextureInfo groundTexture;
	//		groundTexture.texName = "WWPawprintIslandGrass.bmp";
	//		groundTexture.texStrength = 1.0f;
	//		meshC->vecTextures.push_back(groundTexture);
	//	}

	//	sEntity* Sphere1 = cEntityManager::GenerateEntity("PhysicsEntity");
	//	{
	//		sMeshComponent* meshC = static_cast<sMeshComponent*>(Sphere1->GetComponent(eComponentTypes::MESH_COMPONENT));

	//		nPhysics::sRigidbodyDef rbDef;
	//		rbDef.initialPosition = glm::vec3(40.0f, 48.0f, 40.0f);
	//		nPhysics::iSphereShape* sphereShape = pEntityManager->GetPhysicsFactory()->CreateSphereShape(4.0f);
	//		rbDef.collisionShape = sphereShape;
	//		rbDef.mass = 1.0f;
	//		Sphere1->AddComponent(eComponentTypes::RIGIDBODY_COMPONENT, rbDef);

	//		Sphere1->bIsActive = true;
	//		Sphere1->entityName = "PlaneBottom";
	//		Sphere1->entityTag = "Static";
	//		Sphere1->transform->position = glm::vec3(40.0f, 48.0f, 40.0f);

	//		meshC->meshName = "physicsSphere.ply";
	//		sModelDrawInfo* meshInfo = new sModelDrawInfo();
	//		meshInfo->meshFileName = meshC->meshName;
	//		cMeshManager::GetInstance()->FindMeshByName(*meshInfo);
	//		meshC->meshInfo = meshInfo;
	//		meshC->bIsAffectedByLights = true;
	//		meshC->nonUniformScale = glm::vec3(8.0f);
	//		meshC->materialDiffuse = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

	//		sTextureInfo groundTexture;
	//		groundTexture.texName = "WWPawprintIslandGrass.bmp";
	//		groundTexture.texStrength = 1.0f;
	//		meshC->vecTextures.push_back(groundTexture);
	//	}

	//	sEntity* Sphere2 = cEntityManager::GenerateEntity("PhysicsEntity");
	//	{
	//		
	//		sMeshComponent* meshC = static_cast<sMeshComponent*>(Sphere2->GetComponent(eComponentTypes::MESH_COMPONENT));

	//		nPhysics::sRigidbodyDef rbDef;
	//		rbDef.initialPosition = glm::vec3(40.0f, 40.0f, 40.0f);
	//		nPhysics::iSphereShape* sphereShape = pEntityManager->GetPhysicsFactory()->CreateSphereShape(4.0f);
	//		rbDef.collisionShape = sphereShape;
	//		rbDef.mass = 1.0f;
	//		Sphere2->AddComponent(eComponentTypes::RIGIDBODY_COMPONENT, rbDef);

	//		Sphere2->bIsActive = true;
	//		Sphere2->entityName = "PlaneBottom";
	//		Sphere2->entityTag = "Static";
	//		Sphere2->transform->position = glm::vec3(40.0f, 40.0f, 40.0f);

	//		meshC->meshName = "physicsSphere.ply";
	//		sModelDrawInfo* meshInfo = new sModelDrawInfo();
	//		meshInfo->meshFileName = meshC->meshName;
	//		cMeshManager::GetInstance()->FindMeshByName(*meshInfo);
	//		meshC->meshInfo = meshInfo;
	//		meshC->bIsAffectedByLights = true;
	//		meshC->nonUniformScale = glm::vec3(8.0f);
	//		meshC->materialDiffuse = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

	//		sTextureInfo groundTexture;
	//		groundTexture.texName = "WWPawprintIslandGrass.bmp";
	//		groundTexture.texStrength = 1.0f;
	//		meshC->vecTextures.push_back(groundTexture);
	//	}

	//	sEntity* ChainBall = cEntityManager::GenerateEntity("Line2LineEntity");
	//	{
	//		sMeshComponent* meshC = static_cast<sMeshComponent*>(ChainBall->GetComponent(eComponentTypes::MESH_COMPONENT));

	//		nPhysics::sRigidbodyDef rbDef;
	//		rbDef.initialPosition = glm::vec3(40.0f, 32.0f, 40.0f);
	//		nPhysics::iSphereShape* sphereShape = pEntityManager->GetPhysicsFactory()->CreateSphereShape(4.0f);
	//		rbDef.collisionShape = sphereShape;
	//		rbDef.mass = 1.0f;
	//		rbDef.bIsKinematic = true;
	//		rbDef.bUseGravity = false;
	//		ChainBall->AddComponent(eComponentTypes::RIGIDBODY_COMPONENT, rbDef);

	//		ChainBall->bIsActive = true;
	//		ChainBall->entityName = "PlaneBottom";
	//		ChainBall->entityTag = "Static";
	//		ChainBall->transform->position = glm::vec3(40.0f, 32.0f, 40.0f);

	//		meshC->meshName = "physicsSphere.ply";
	//		sModelDrawInfo* meshInfo = new sModelDrawInfo();
	//		meshInfo->meshFileName = meshC->meshName;
	//		cMeshManager::GetInstance()->FindMeshByName(*meshInfo);
	//		meshC->meshInfo = meshInfo;
	//		meshC->bIsAffectedByLights = true;
	//		meshC->nonUniformScale = glm::vec3(8.0f);
	//		meshC->materialDiffuse = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

	//		sTextureInfo groundTexture;
	//		groundTexture.texName = "WWPawprintIslandGrass.bmp";
	//		groundTexture.texStrength = 1.0f;
	//		meshC->vecTextures.push_back(groundTexture);
	//	}

	//	nPhysics::iRigidbody* rb0 = static_cast<nPhysics::iRigidbody*>(ChainBall->GetComponent(RIGIDBODY_COMPONENT));
	//	nPhysics::iRigidbody* rb1 = static_cast<nPhysics::iRigidbody*>(Sphere0->GetComponent(RIGIDBODY_COMPONENT));
	//	nPhysics::iRigidbody* rb2 = static_cast<nPhysics::iRigidbody*>(Sphere1->GetComponent(RIGIDBODY_COMPONENT));
	//	nPhysics::iRigidbody* rb3 = static_cast<nPhysics::iRigidbody*>(Sphere2->GetComponent(RIGIDBODY_COMPONENT));
	//	nPhysics::iCompoundBody* cb = static_cast<nPhysics::iCompoundBody*>(ChainBall->GetComponent(LINE2LINE_COMPOUND_COMPONENT));

	//	cb->AddCompoundBody(rb0);
	//	cb->AddCompoundBody(rb1);
	//	cb->AddCompoundBody(rb2);
	//	cb->AddCompoundBody(rb3);
	//}

	//void LoadCompoundBodyFixed(cEntityManager* pEntityManager)
	//{
	//	sEntity* Sphere0 = cEntityManager::GenerateEntity("PhysicsEntity");
	//	{
	//		sMeshComponent* meshC = static_cast<sMeshComponent*>(Sphere0->GetComponent(eComponentTypes::MESH_COMPONENT));

	//		nPhysics::sRigidbodyDef rbDef;
	//		rbDef.initialPosition = glm::vec3(0.0f, 40.0f, 40.0f);
	//		nPhysics::iBoxShape* sphereShape = pEntityManager->GetPhysicsFactory()->CreateBoxShape(glm::vec3(8.0f));
	//		rbDef.collisionShape = sphereShape;
	//		rbDef.mass = 1.0f;
	//		Sphere0->AddComponent(eComponentTypes::RIGIDBODY_COMPONENT, rbDef);

	//		Sphere0->bIsActive = true;
	//		Sphere0->entityName = "PlaneBottom";
	//		Sphere0->entityTag = "Static";
	//		Sphere0->transform->position = glm::vec3(0.0f, 40.0f, 40.0f);

	//		meshC->meshName = "physicsBox.ply";
	//		sModelDrawInfo* meshInfo = new sModelDrawInfo();
	//		meshInfo->meshFileName = meshC->meshName;
	//		cMeshManager::GetInstance()->FindMeshByName(*meshInfo);
	//		meshC->meshInfo = meshInfo;
	//		meshC->bIsAffectedByLights = true;
	//		meshC->nonUniformScale = glm::vec3(8.0f);
	//		meshC->materialDiffuse = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

	//		sTextureInfo groundTexture;
	//		groundTexture.texName = "WWPawprintIslandGrass.bmp";
	//		groundTexture.texStrength = 1.0f;
	//		meshC->vecTextures.push_back(groundTexture);
	//	}

	//	sEntity* Sphere1 = cEntityManager::GenerateEntity("PhysicsEntity");
	//	{

	//		sMeshComponent* meshC = static_cast<sMeshComponent*>(Sphere1->GetComponent(eComponentTypes::MESH_COMPONENT));

	//		nPhysics::sRigidbodyDef rbDef;
	//		rbDef.initialPosition = glm::vec3(0.0f, 32.0f, 40.0f);
	//		nPhysics::iSphereShape* sphereShape = pEntityManager->GetPhysicsFactory()->CreateSphereShape(4.0f);
	//		rbDef.collisionShape = sphereShape;
	//		rbDef.mass = 1.0f;
	//		Sphere1->AddComponent(eComponentTypes::RIGIDBODY_COMPONENT, rbDef);

	//		Sphere1->bIsActive = true;
	//		Sphere1->entityName = "PlaneBottom";
	//		Sphere1->entityTag = "Static";
	//		Sphere1->transform->position = glm::vec3(0.0f, 32.0f, 40.0f);

	//		meshC->meshName = "physicsSphere.ply";
	//		sModelDrawInfo* meshInfo = new sModelDrawInfo();
	//		meshInfo->meshFileName = meshC->meshName;
	//		cMeshManager::GetInstance()->FindMeshByName(*meshInfo);
	//		meshC->meshInfo = meshInfo;
	//		meshC->bIsAffectedByLights = true;
	//		meshC->nonUniformScale = glm::vec3(8.0f);
	//		meshC->materialDiffuse = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

	//		sTextureInfo groundTexture;
	//		groundTexture.texName = "WWPawprintIslandGrass.bmp";
	//		groundTexture.texStrength = 1.0f;
	//		meshC->vecTextures.push_back(groundTexture);
	//	}

	//	sEntity* FixedObject = cEntityManager::GenerateEntity("FixedEntity");
	//	{
	//		sMeshComponent* meshC = static_cast<sMeshComponent*>(FixedObject->GetComponent(eComponentTypes::MESH_COMPONENT));

	//		nPhysics::sRigidbodyDef rbDef;
	//		rbDef.initialPosition = glm::vec3(0.0f, 24.0f, 40.0f);
	//		glm::vec3 p0 = rbDef.initialPosition - glm::vec3(0.5f, 0.5f, 0.5f);
	//		glm::vec3 p1 = rbDef.initialPosition + glm::vec3(0.5f, -0.5f, -0.5f);
	//		glm::vec3 p2 = rbDef.initialPosition + glm::vec3(0.0f, -0.5f, 0.5f);
	//		glm::vec3 p3 = rbDef.initialPosition + glm::vec3(0.0f, 0.5f, 0.0f);
	//		nPhysics::iTetrahedronShape* tetrahedronShape = pEntityManager->GetPhysicsFactory()->CreateTetrahedronShape(p0, p1, p2, p3);
	//		rbDef.collisionShape = tetrahedronShape;
	//		rbDef.mass = 1.0f;
	//		rbDef.bIsKinematic = true;
	//		rbDef.bUseGravity = false;
	//		FixedObject->AddComponent(eComponentTypes::RIGIDBODY_COMPONENT, rbDef);

	//		FixedObject->bIsActive = true;
	//		FixedObject->entityName = "PlaneBottom";
	//		FixedObject->entityTag = "Static";
	//		FixedObject->transform->position = glm::vec3(0.0f, 24.0f, 40.0f);

	//		meshC->meshName = "physicsTetrahedron.ply";
	//		sModelDrawInfo* meshInfo = new sModelDrawInfo();
	//		meshInfo->meshFileName = meshC->meshName;
	//		cMeshManager::GetInstance()->FindMeshByName(*meshInfo);
	//		meshC->meshInfo = meshInfo;
	//		meshC->bIsAffectedByLights = true;
	//		meshC->nonUniformScale = glm::vec3(8.0f);
	//		meshC->materialDiffuse = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

	//		sTextureInfo groundTexture;
	//		groundTexture.texName = "WWPawprintIslandGrass.bmp";
	//		groundTexture.texStrength = 1.0f;
	//		meshC->vecTextures.push_back(groundTexture);
	//	}

	//	nPhysics::iRigidbody* rb0 = static_cast<nPhysics::iRigidbody*>(FixedObject->GetComponent(RIGIDBODY_COMPONENT));
	//	nPhysics::iRigidbody* rb1 = static_cast<nPhysics::iRigidbody*>(Sphere0->GetComponent(RIGIDBODY_COMPONENT));
	//	nPhysics::iRigidbody* rb2 = static_cast<nPhysics::iRigidbody*>(Sphere1->GetComponent(RIGIDBODY_COMPONENT));
	//	nPhysics::iCompoundBody* cb = static_cast<nPhysics::iCompoundBody*>(FixedObject->GetComponent(FIXED_COMPOUND_COMPONENT));

	//	cb->AddCompoundBody(rb0);
	//	cb->AddCompoundBody(rb1);
	//	cb->AddCompoundBody(rb2);
	//}

	//void LoadCompoundBodySlider(cEntityManager* pEntityManager)
	//{
	//	sEntity* box = cEntityManager::GenerateEntity("PhysicsEntity");
	//	{
	//		sMeshComponent* meshC = static_cast<sMeshComponent*>(box->GetComponent(eComponentTypes::MESH_COMPONENT));

	//		nPhysics::sRigidbodyDef rbDef;
	//		rbDef.initialPosition = glm::vec3(-40.0f, 40.0f, 40.0f);
	//		nPhysics::iBoxShape* boxShape = pEntityManager->GetPhysicsFactory()->CreateBoxShape(glm::vec3(8.0f));
	//		rbDef.collisionShape = boxShape;
	//		rbDef.mass = 1.0f;

	//		box->bIsActive = true;
	//		box->entityName = "PlaneBottom";
	//		box->entityTag = "Static";
	//		box->transform->position = glm::vec3(-40.0f, 40.0f, 40.0f);
	//		
	//		box->AddComponent(eComponentTypes::RIGIDBODY_COMPONENT, rbDef);

	//		meshC->meshName = "physicsBox.ply";
	//		sModelDrawInfo* meshInfo = new sModelDrawInfo();
	//		meshInfo->meshFileName = meshC->meshName;
	//		cMeshManager::GetInstance()->FindMeshByName(*meshInfo);
	//		meshC->meshInfo = meshInfo;
	//		meshC->bIsAffectedByLights = true;
	//		meshC->nonUniformScale = glm::vec3(8.0f);
	//		meshC->materialDiffuse = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);

	//		sTextureInfo groundTexture;
	//		groundTexture.texName = "WWPawprintIslandGrass.bmp";
	//		groundTexture.texStrength = 1.0f;
	//		meshC->vecTextures.push_back(groundTexture);
	//	}

	//	sEntity* sliderObject = cEntityManager::GenerateEntity("SliderEntity");
	//	{
	//		sMeshComponent* meshC = static_cast<sMeshComponent*>(sliderObject->GetComponent(eComponentTypes::MESH_COMPONENT));

	//		nPhysics::sRigidbodyDef rbDef;
	//		rbDef.initialPosition = glm::vec3(-40.0f, 24.0f, 40.0f);
	//		nPhysics::iCylinderShape* cylinderShape = pEntityManager->GetPhysicsFactory()->CreateCylinderShape(glm::vec3(8.0f, 24.0f, 8.0f));
	//		rbDef.collisionShape = cylinderShape;
	//		rbDef.mass = 1.0f;
	//		rbDef.bIsKinematic = true;
	//		rbDef.bUseGravity = false;

	//		sliderObject->bIsActive = true;
	//		sliderObject->entityName = "PlaneBottom";
	//		sliderObject->entityTag = "Static";
	//		sliderObject->transform->position = glm::vec3(-40.0f, 24.0f, 40.0f);
	//		//sliderObject->transform->qOrientation = nQuaternionHelper::SetOrientationEulerAngles(glm::vec3(0.0f, 0.0f, 90.0f), true);
	//		//rbDef.initialRotation = sliderObject->transform->qOrientation;
	//		sliderObject->AddComponent(eComponentTypes::RIGIDBODY_COMPONENT, rbDef);

	//		meshC->meshName = "physicsCylinder.ply";
	//		sModelDrawInfo* meshInfo = new sModelDrawInfo();
	//		meshInfo->meshFileName = meshC->meshName;
	//		cMeshManager::GetInstance()->FindMeshByName(*meshInfo);
	//		meshC->meshInfo = meshInfo;
	//		meshC->bIsAffectedByLights = true;
	//		meshC->nonUniformScale = glm::vec3(8.0f);
	//		meshC->materialDiffuse = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);

	//		sTextureInfo groundTexture;
	//		groundTexture.texName = "WWPawprintIslandGrass.bmp";
	//		groundTexture.texStrength = 1.0f;
	//		meshC->vecTextures.push_back(groundTexture);
	//	}

	//	nPhysics::iRigidbody* rb0 = static_cast<nPhysics::iRigidbody*>(sliderObject->GetComponent(RIGIDBODY_COMPONENT));
	//	nPhysics::iRigidbody* rb1 = static_cast<nPhysics::iRigidbody*>(box->GetComponent(RIGIDBODY_COMPONENT));
	//	nPhysics::iCompoundBody* cb = static_cast<nPhysics::iCompoundBody*>(sliderObject->GetComponent(SLIDER_COMPOUND_COMPONENT));

	//	cb->AddCompoundBody(rb0);
	//	cb->AddCompoundBody(rb1);
	//}

	//void LoadCompoundBodyHinge(cEntityManager* pEntityManager)
	//{
	//	sEntity* hingeObject = cEntityManager::GenerateEntity("HingeEntity");
	//	{
	//		sMeshComponent* meshC = static_cast<sMeshComponent*>(hingeObject->GetComponent(eComponentTypes::MESH_COMPONENT));

	//		nPhysics::sRigidbodyDef rbDef;
	//		rbDef.initialPosition = glm::vec3(0.0f, 8.0f, 60.0f);
	//		nPhysics::iBoxShape* cylinderShape = pEntityManager->GetPhysicsFactory()->CreateBoxShape(glm::vec3(8.0f));
	//		rbDef.collisionShape = cylinderShape;
	//		rbDef.mass = 1.0f;
	//		rbDef.bIsKinematic = true;
	//		rbDef.bUseGravity = false;

	//		hingeObject->bIsActive = true;
	//		hingeObject->entityName = "PlaneBottom";
	//		hingeObject->entityTag = "Static";
	//		hingeObject->transform->position = glm::vec3(0.0f, 8.0f, 60.0f);
	//		hingeObject->transform->qOrientation = nQuaternionHelper::SetOrientationEulerAngles(glm::vec3(0.0f, 0.0f, 90.0f), true);
	//		rbDef.initialRotation = hingeObject->transform->qOrientation;
	//		hingeObject->AddComponent(eComponentTypes::RIGIDBODY_COMPONENT, rbDef);

	//		meshC->meshName = "physicsBox.ply";
	//		sModelDrawInfo* meshInfo = new sModelDrawInfo();
	//		meshInfo->meshFileName = meshC->meshName;
	//		cMeshManager::GetInstance()->FindMeshByName(*meshInfo);
	//		meshC->meshInfo = meshInfo;
	//		meshC->bIsAffectedByLights = true;
	//		meshC->nonUniformScale = glm::vec3(8.0f);
	//		meshC->materialDiffuse = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);

	//		sTextureInfo groundTexture;
	//		groundTexture.texName = "WWPawprintIslandGrass.bmp";
	//		groundTexture.texStrength = 1.0f;
	//		meshC->vecTextures.push_back(groundTexture);
	//	}

	//	nPhysics::iRigidbody* rb0 = static_cast<nPhysics::iRigidbody*>(hingeObject->GetComponent(RIGIDBODY_COMPONENT));
	//	nPhysics::iCompoundBody* cb = static_cast<nPhysics::iCompoundBody*>(hingeObject->GetComponent(HINGE_COMPOUND_COMPONENT));

	//	cb->AddCompoundBody(rb0);
	//}

	void LoadModelsIntoScene(cEntityManager* pEntityManager, cVaoMeshManager* pVaoMeshManager, cAssimpMeshLoader* pAssimpLoader)
	{
		//{
		//	sEntity* skyboxSphere = pEntityManager->GenerateEntityOfType("StaticEntity");
		//	sMeshComponent* meshC = static_cast<sMeshComponent*>(skyboxSphere->GetComponent(eComponentTypes::MESH_COMPONENT));
		//	skyboxSphere->bIsActive = true;
		//	skyboxSphere->entityName = "SkyboxSphere";
		//	skyboxSphere->entityTag = "Static";
		//	meshC->meshName = "skyboxSphere.ply";
		//	sModelDrawInfo* meshInfo = new sModelDrawInfo();
		//	meshInfo->meshFileName = meshC->meshName;
		//	pVaoMeshManager->FindDrawInfoByName(*meshInfo);
		//	meshC->meshInfo = meshInfo;
		//	meshC->nonUniformScale = glm::vec3(10000.0f);
		//	meshC->materialDiffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		//	meshC->bIsAffectedByLights = false;
		//	meshC->bIsVisible = true;
		//	skyboxSphere->transform->position = glm::vec3(0.0f);
		//}

		//{
		//	sEntity* pCamera = pEntityManager->GenerateEntityOfType("CameraEntity");
		//	pCamera->entityName = "WorldCamera";
		//	pCamera->bIsActive = true;
		//	sCameraComponent* cameraC = static_cast<sCameraComponent*>(pCamera->GetComponent(eComponentTypes::CAMERA_COMPONENT));
		//	cameraC->eyePosition = glm::vec3(0.0f, 20.0f, -50.0f);
		//	cameraC->qOrientation = nQuaternionHelper::AdjOrientationEulerAngles(cameraC->qOrientation, glm::vec3(0.0f, 0.0f, 0.0f), true);
		//	cameraC->bFollowTarget = false;
		//	sAudioComponent* audioC = static_cast<sAudioComponent*>(pCamera->GetComponent(eComponentTypes::AUDIO_COMPONENT));
		//	audioC->bPlayOnStart = true;
		//	audioC->filename = "BGM_FFX-SightOfSpira.wav";
		//	audioC->volume = 1.0f;
		//	audioC->bLoop = true;
		//}

		//{
		//	sEntity* p2SidedQuad = pEntityManager->GenerateEntityOfType("StaticEntity");
		//	sMeshComponent* meshC = static_cast<sMeshComponent*>(p2SidedQuad->GetComponent(eComponentTypes::MESH_COMPONENT));

		//	p2SidedQuad->entityName = "2SidedQuad";
		//	p2SidedQuad->bIsActive = true;
		//	p2SidedQuad->entityTag = "Draw";
		//	p2SidedQuad->threatTag = "";

		//	meshC->meshName = "1x1_2Tri_Quad_2_Sided_xyz_n_uv.ply";
		//	sModelDrawInfo* meshInfo = new sModelDrawInfo();
		//	meshInfo->meshFileName = meshC->meshName;
		//	pVaoMeshManager->FindDrawInfoByName(*meshInfo);
		//	meshC->meshInfo = meshInfo;
		//	meshC->nonUniformScale = glm::vec3(10.0f);
		//	meshC->materialDiffuse = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		//	meshC->bIsAffectedByLights = false;
		//	meshC->bIsVisible = false;

		//	p2SidedQuad->transform->position = glm::vec3(0.0f, 0.0f, 0.0f);
		//	glm::quat newOrientation = nQuaternionHelper::SetOrientationEulerAngles(90.0f, 0.0f, 90.0f, true);
		//	p2SidedQuad->transform->qOrientation = newOrientation;
		//}
		//
		//{
		//	sEntity* RPGChan = pEntityManager->GenerateEntityOfType("StaticEntity");
		//	sMeshComponent* meshC = static_cast<sMeshComponent*>(RPGChan->GetComponent(eComponentTypes::MESH_COMPONENT));
		//	//nPhysics::iRigidbody* rigidbodyC = static_cast<nPhysics::iRigidbody*>(fahrenheit->GetComponent(eComponentTypes::BULLET_RIGIDBODY_COMPONENT));
		//	//rigidbodyC->SetUseGravity(false);
		//	RPGChan->bIsActive = true;
		//	RPGChan->entityName = "RPGChan";
		//	RPGChan->entityTag = "Player";
		//	RPGChan->threatTag = "Enemy";
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
		//	fahrenheitTexture.texName = "fahrenheit.bmp";
		//	fahrenheitTexture.texStrength = 1.0f;
		//	meshC->vecTextures.push_back(fahrenheitTexture);
		//	RPGChan->transform->position = glm::vec3(0.0f, 0.0f, 0.0f);
		//	RPGChan->transform->qOrientation = nQuaternionHelper::AdjOrientationEulerAngles(RPGChan->transform->qOrientation, glm::vec3(0.0f, 0.0f, 0.0f), true);
		//	RPGChan->transform->respawnPosition = glm::vec3(0.0f, 0.0f, 0.0f);

		//	// Animation
		//	meshC->SetAnimetor();
		//	cAssimpMeshLoader::sAnimationInfo animInfo;

		//	pAssimpLoader->FindAnimationByName("Idle", animInfo);
		//	meshC->animator->AddAnimation(animInfo); // Idle...
		//	pAssimpLoader->FindAnimationByName("Run", animInfo);
		//	meshC->animator->AddAnimation(animInfo); // Run...
		//	pAssimpLoader->FindAnimationByName("Attack", animInfo);
		//	meshC->animator->AddAnimation(animInfo); // Attack...
		//}

		///*{
		//	sEntity* ClothEntity = pEntityManager->GenerateEntityOfType("ClothEntity");
		//	sMeshComponent* meshC = static_cast<sMeshComponent*>(ClothEntity->GetComponent(eComponentTypes::MESH_COMPONENT));
		//	nPhysics::sSimpleSoftBodyComponent* softbodyC = static_cast<nPhysics::sSimpleSoftBodyComponent*>(ClothEntity->GetComponent(SOFTBODY_COMPONENT));

		//	ClothEntity->bIsActive = true;
		//	ClothEntity->entityName = "Cloth";
		//	ClothEntity->entityTag = "Cloth";
		//	ClothEntity->threatTag = "";
		//	ClothEntity->transform->position = glm::vec3(0.0f, 20.0f, 0.0f);
		//	ClothEntity->transform->qOrientation = nQuaternionHelper::AdjOrientationEulerAngles(ClothEntity->transform->qOrientation, glm::vec3(0.0f, 0.0f, 0.0f), true);
		//	ClothEntity->transform->respawnPosition = glm::vec3(0.0f, 0.0f, 0.0f);

		//	meshC->meshName = "debugSphere.ply";
		//	sModelDrawInfo* meshInfo = new sModelDrawInfo();
		//	meshInfo->meshFileName = meshC->meshName;
		//	pVaoMeshManager->FindDrawInfoByName(*meshInfo);
		//	meshC->meshInfo = meshInfo;
		//	meshC->nonUniformScale = glm::vec3(0.1f);
		//	meshC->materialDiffuse = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		//	meshC->bIsAffectedByLights = true;
		//	meshC->bIsVisible = true;

		//	sTextureInfo fahrenheitTexture;
		//	fahrenheitTexture.texName = "fahrenheit.bmp";
		//	fahrenheitTexture.texStrength = 1.0f;
		//	meshC->vecTextures.push_back(fahrenheitTexture);
		//}*/

		///*{
		//	sEntity* dalek = pEntityManager->GenerateEntityOfType("StaticEntity");
		//	sMeshComponent* meshC = static_cast<sMeshComponent*>(dalek->GetComponent(eComponentTypes::MESH_COMPONENT));
		//	sLocationComponent* locationC = static_cast<sLocationComponent*>(dalek->GetComponent(eComponentTypes::LOCATION_COMPONENT));

		//	dalek->bIsActive = false;
		//	dalek->entityName = "Dalek";
		//	dalek->entityTag = "Static";
		//	meshC->meshName = "dalek2005_xyz_uv_res_2.ply";

		//	sModelDrawInfo* meshInfo = new sModelDrawInfo();
		//	meshInfo->meshFileName = meshC->meshName;
		//	pVaoMeshManager->FindDrawInfoByName(*meshInfo);
		//	meshC->meshInfo = meshInfo;
		//	meshC->nonUniformScale = glm::vec3(1.0f);
		//	meshC->materialDiffuse = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		//	meshC->bIsAffectedByLights = true;
		//	meshC->bIsVisible = true;

		//	sTextureInfo castleTexture;
		//	castleTexture.texName = "fireFlan.bmp";
		//	castleTexture.texStrength = 1.0f;
		//	meshC->vecTextures.push_back(castleTexture);
		//	locationC->position = glm::vec3(0.0f);
		//	glm::quat newOrientation = nQuaternionHelper::SetOrientationEulerAngles(0.0f, 0.0f, 0.0f, true);
		//	dalek->transform->qOrientation = newOrientation;
		//}*/

		//

		////// Generate master Flock
		////sEntity* flock = pEntityManager->GenerateEntityOfType("StaticEntity");
		////{
		////	sFlockComponent* flockC = static_cast<sFlockComponent*>(flock->AddComponent(FLOCK_COMPONENT));
		////	sMeshComponent* meshC = static_cast<sMeshComponent*>(flock->AddComponent(MESH_COMPONENT));

		////	flock->bIsActive = true;
		////	flock->entityName = "Flock1";
		////	flock->entityTag = "Static";

		////	meshC->meshName = "debugSphere.ply";
		////	sModelDrawInfo* meshInfo = new sModelDrawInfo();
		////	meshInfo->meshFileName = meshC->meshName;
		////	pVaoMeshManager->FindDrawInfoByName(*meshInfo);
		////	meshC->meshInfo = meshInfo;
		////	meshC->nonUniformScale = glm::vec3(10.0f);
		////	meshC->materialDiffuse = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
		////	meshC->bIsAffectedByLights = false;
		////	meshC->bIsVisible = true;
		////	
		////	flockC->flockEntity = flock;
		////	flock->transform->position = glm::vec3(0.0f);
		////	glm::quat newOrientation = nQuaternionHelper::SetOrientationEulerAngles(0.0f, 0.0f, 0.0f, true);
		////	flock->transform->qOrientation = newOrientation;
		////	flockC->AddPathNode(glm::vec3(-20.0f, 0.0f, -20.0f));
		////	flockC->AddPathNode(glm::vec3(-20.0f, 0.0f, 20.0f));
		////	flockC->AddPathNode(glm::vec3(20.0f, 0.0f, 20.0f));
		////	flockC->AddPathNode(glm::vec3(20.0f, 0.0f, -20.0f));
		////}

		////// Skinned Mesh
		////for(unsigned int i = 0; i < 12; i++)
		////{
		////	sEntity* RPGChan = pEntityManager->GenerateEntityOfType("StaticEntity");
		////	sMeshComponent* meshC = static_cast<sMeshComponent*>(RPGChan->GetComponent(eComponentTypes::MESH_COMPONENT));
		////	sFlockComponent* flockC = static_cast<sFlockComponent*>(RPGChan->AddComponent(FLOCK_COMPONENT));
		////	//nPhysics::iRigidbody* rigidbodyC = static_cast<nPhysics::iRigidbody*>(fahrenheit->GetComponent(eComponentTypes::BULLET_RIGIDBODY_COMPONENT));
		////	//rigidbodyC->SetUseGravity(false);
		////	RPGChan->bIsActive = true;
		////	RPGChan->entityName = "RPGChan";
		////	RPGChan->entityTag = "Player";
		////	RPGChan->threatTag = "Enemy";
		////	meshC->meshName = "RPG-Character(FBX2013).FBX";
		////	sModelDrawInfo* meshInfo = new sModelDrawInfo();
		////	meshInfo->meshFileName = meshC->meshName;
		////	pVaoMeshManager->FindDrawInfoByName(*meshInfo);
		////	meshC->meshInfo = meshInfo;
		////	meshC->nonUniformScale = glm::vec3(0.1f);
		////	meshC->materialDiffuse = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		////	meshC->bIsAffectedByLights = true;
		////	meshC->bIsVisible = true;
		////	sTextureInfo fahrenheitTexture;
		////	fahrenheitTexture.texName = "fahrenheit.bmp";
		////	fahrenheitTexture.texStrength = 1.0f;
		////	meshC->vecTextures.push_back(fahrenheitTexture);
		////	RPGChan->transform->position = glm::vec3(20.0f * i, 0.0f, 20.0f * i * (-1.0f * (i % 2)));
		////	RPGChan->transform->qOrientation = nQuaternionHelper::AdjOrientationEulerAngles(RPGChan->transform->qOrientation, glm::vec3(0.0f, 0.0f, 0.0f), true);
		////	RPGChan->transform->respawnPosition = glm::vec3(0.0f, 0.0f, 0.0f);

		////	// Animation
		////	meshC->SetAnimetor();
		////	cAssimpMeshLoader::sAnimationInfo animInfo;

		////	pAssimpLoader->FindAnimationByName("Idle", animInfo);
		////	meshC->animator->AddAnimation(animInfo); // Idle...
		////	pAssimpLoader->FindAnimationByName("Run", animInfo);
		////	meshC->animator->AddAnimation(animInfo); // Run...
		////	pAssimpLoader->FindAnimationByName("Attack", animInfo);
		////	meshC->animator->AddAnimation(animInfo); // Attack...

		////	sFlockComponent* masterFlockC = static_cast<sFlockComponent*>(flock->GetComponent(FLOCK_COMPONENT));
		////	masterFlockC->AddBoid(RPGChan);
		////	flockC->masterFlock = masterFlockC;
		////	flockC->flockEntity = RPGChan;
		////}

		////sFlockComponent* masterFlockC = static_cast<sFlockComponent*>(flock->GetComponent(FLOCK_COMPONENT));
		////masterFlockC->SetFormation(sFlockComponent::LINE, glm::vec3(0.0f), glm::vec3(10.0f, 0.0f, 0.0f));
		////masterFlockC->MakeFormation();

		//CreateRigidbodies(pEntityManager, pVaoMeshManager);

		///*LoadCompoundBodyLine2Line(pEntityManager);
		//LoadCompoundBodyFixed(pEntityManager);
		//LoadCompoundBodySlider(pEntityManager);
		//LoadCompoundBodyHinge(pEntityManager);*/

		///*{
		//	sEntity* castle = pEntityManager->GenerateEntityOfType("StaticEntity");
		//	sMeshComponent* meshC = static_cast<sMeshComponent*>(castle->GetComponent(eComponentTypes::MESH_COMPONENT));
		//	sLocationComponent* locationC = static_cast<sLocationComponent*>(castle->GetComponent(eComponentTypes::LOCATION_COMPONENT));
		//	castle->bIsActive = true;
		//	castle->entityName = "Castle";
		//	castle->entityTag = "Static";
		//	meshC->meshName = "castle.ply";
		//	sModelDrawInfo* meshInfo = new sModelDrawInfo();
		//	meshInfo->meshFileName = meshC->meshName;
		//	pVaoMeshManager->FindDrawInfoByName(*meshInfo);
		//	meshC->meshInfo = meshInfo;
		//	meshC->nonUniformScale = glm::vec3(1.0f);
		//	meshC->materialDiffuse = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		//	meshC->bIsAffectedByLights = true;
		//	meshC->bIsVisible = true;
		//	sTextureInfo castleTexture;
		//	castleTexture.texName = "angkorWatTex.bmp";
		//	castleTexture.texStrength = 1.0f;
		//	meshC->vecTextures.push_back(castleTexture);
		//	locationC->position = glm::vec3(0.0f);
		//}*/

		//{
		//	sEntity* debugSphere = pEntityManager->GenerateEntityOfType("StaticEntity");
		//	sMeshComponent* meshC = static_cast<sMeshComponent*>(debugSphere->GetComponent(eComponentTypes::MESH_COMPONENT));
		//	debugSphere->bIsActive = true;
		//	debugSphere->entityName = "DebugSphere";
		//	debugSphere->entityTag = "Static";
		//	meshC->meshName = "debugSphere.ply";
		//	sModelDrawInfo* meshInfo = new sModelDrawInfo();
		//	meshInfo->meshFileName = meshC->meshName;
		//	pVaoMeshManager->FindDrawInfoByName(*meshInfo);
		//	meshC->meshInfo = meshInfo;
		//	meshC->nonUniformScale = glm::vec3(1.0f);
		//	meshC->materialDiffuse = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
		//	meshC->bIsAffectedByLights = true;
		//	meshC->bIsVisible = false;
		//	debugSphere->transform->position = glm::vec3(0.0f);
		//}

		//{
		//	sEntity* wireframeSphere = pEntityManager->GenerateEntityOfType("StaticEntity");
		//	sMeshComponent* meshC = static_cast<sMeshComponent*>(wireframeSphere->GetComponent(eComponentTypes::MESH_COMPONENT));
		//	wireframeSphere->bIsActive = true;
		//	wireframeSphere->entityName = "WireframeSphere";
		//	wireframeSphere->entityTag = "Static";
		//	meshC->meshName = "lowPolySphere.ply";
		//	sModelDrawInfo* meshInfo = new sModelDrawInfo();
		//	meshInfo->meshFileName = meshC->meshName;
		//	pVaoMeshManager->FindDrawInfoByName(*meshInfo);
		//	meshC->meshInfo = meshInfo;
		//	meshC->nonUniformScale = glm::vec3(1.0f);
		//	meshC->materialDiffuse = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		//	meshC->bIsAffectedByLights = false;
		//	meshC->bIsWireframe = true;
		//	meshC->bIsVisible = true;
		//	wireframeSphere->transform->position = glm::vec3(0.0f);
		//}

		//{
		//	sEntity* light1 = pEntityManager->GenerateEntityOfType("LightEntity");
		//	light1->bIsActive = true;
		//	light1->entityName = "Light";
		//	light1->entityTag = "Static";
		//	sLightComponent* lightC = static_cast<sLightComponent*>(light1->GetComponent(eComponentTypes::LIGHT_COMPONENT));
		//	lightC->SetLightType(lightC->DIRECTIONAL_LIGHT);
		//	lightC->SetDiffuseColour(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		//	lightC->SetPosition(glm::vec4(0.0f, 100.0f, 0.0f, 1.0f));
		//	lightC->bisWireframeOn = false;
		//}

		//{
		//	sEntity* light1 = pEntityManager->GenerateEntityOfType("LightEntity");
		//	light1->bIsActive = true;
		//	light1->entityName = "Light";
		//	light1->entityTag = "Static";
		//	sLightComponent* lightC = static_cast<sLightComponent*>(light1->GetComponent(eComponentTypes::LIGHT_COMPONENT));
		//	lightC->SetLightType(lightC->POINT_LIGHT);
		//	lightC->SetDiffuseColour(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		//	lightC->SetPosition(glm::vec4(0.0f, 100.0f, 0.0f, 1.0f));
		//	lightC->SetLinearAttenuation(0.1f);
		//	lightC->bisWireframeOn = false;
		//}

		//{
		//	sEntity* AABB = pEntityManager->GenerateEntityOfType("StaticEntity");
		//	sMeshComponent* meshC = static_cast<sMeshComponent*>(AABB->GetComponent(eComponentTypes::MESH_COMPONENT));
		//	AABB->bIsActive = true;
		//	AABB->entityName = "AABB";
		//	AABB->entityTag = "Static";
		//	meshC->meshName = "AABB.ply";
		//	sModelDrawInfo* meshInfo = new sModelDrawInfo();
		//	meshInfo->meshFileName = meshC->meshName;
		//	pVaoMeshManager->FindDrawInfoByName(*meshInfo);
		//	meshC->meshInfo = meshInfo;
		//	meshC->nonUniformScale = glm::vec3(1.0f);
		//	meshC->materialDiffuse = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		//	meshC->bIsAffectedByLights = false;
		//	meshC->bIsWireframe = true;
		//	meshC->bIsVisible = false;
		//	AABB->transform->position = glm::vec3(0.0f);
		//}
	}

	void CreateScene()
	{
		/*{
			sEntity* pCamera = cPoolManager::GetInstance()->GetEntityByName("WorldCamera");
			sEntity* Follow = cPoolManager::GetInstance()->GetEntityByName("PlayerSphere");
			sCameraComponent* cameraC = static_cast<sCameraComponent*>(pCamera->GetComponent(eComponentTypes::CAMERA_COMPONENT));
			cameraC->SetTarget(Follow, glm::vec3(80.0f, 50.0f, 0.0f), 10.0f, 50.0f);
		}*/

		//for (unsigned int i = 0; i != 500; i++)
		//{
		//	sEntity* playerBullet = pEntityManager->GenerateEntityOfType("BulletEntity");
		//	sMeshComponent* meshC = static_cast<sMeshComponent*>(playerBullet->GetComponent(eComponentTypes::MESH_COMPONENT));
		//	sLocationComponent* locationC = static_cast<sLocationComponent*>(playerBullet->GetComponent(eComponentTypes::LOCATION_COMPONENT));
		//	nPhysics::iRigidbody* rigidbodyC = static_cast<nPhysics::sBulletRigidbodyComponent*>(playerBullet->GetComponent(eComponentTypes::RIGIDBODY_COMPONENT));
		//	//rigidbodyC->_rigidbody->setIgnoreCollisionCheck(rigidbodyC->_rigidbody, true);
		//	playerBullet->bIsActive = false;
		//	playerBullet->entityName = "PlayerBullet";
		//	playerBullet->entityTag = "PlayerBullet";
		//	meshC->meshName = "physicsSphere.ply";
		//	meshC->nonUniformScale = glm::vec3(1.0f);
		//	meshC->materialDiffuse = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
		//	meshC->bIsAffectedByLights = true;
		//	meshC->bIsVisible = true;
		//	locationC->position = glm::vec3(0.0f);
		//}

		//for (unsigned int i = 0; i != 500; i++)
		//{
		//	sEntity* playerBullet = pEntityManager->GenerateEntityOfType("BulletEntity");
		//	sMeshComponent* meshC = static_cast<sMeshComponent*>(playerBullet->GetComponent(eComponentTypes::MESH_COMPONENT));
		//	sLocationComponent* locationC = static_cast<sLocationComponent*>(playerBullet->GetComponent(eComponentTypes::LOCATION_COMPONENT));
		//	nPhysics::iRigidbody* rigidbodyC = static_cast<nPhysics::sBulletRigidbodyComponent*>(playerBullet->GetComponent(eComponentTypes::RIGIDBODY_COMPONENT));
		//	//rigidbodyC->_rigidbody->setIgnoreCollisionCheck(rigidbodyC->_rigidbody, true);
		//	playerBullet->bIsActive = false;
		//	playerBullet->entityName = "EnemyBullet";
		//	playerBullet->entityTag = "EnemyBullet";
		//	meshC->meshName = "physicsSphere.ply";
		//	meshC->nonUniformScale = glm::vec3(1.0f);
		//	meshC->materialDiffuse = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		//	meshC->bIsAffectedByLights = true;
		//	meshC->bIsVisible = true;
		//	locationC->position = glm::vec3(0.0f);
		//}

		//// Generate waves
		//{
		//	std::pair<std::string, glm::vec3> pair1 = std::make_pair("EnemyTypeA", glm::vec3(30.0f, 20.0f, 0.0f));
		//	//std::pair<std::string, glm::vec3> pair2 = std::make_pair("EnemyTypeA", glm::vec3(30.0f, 20.0f, 15.0f));
		//	//std::pair<std::string, glm::vec3> pair3 = std::make_pair("EnemyTypeA", glm::vec3(30.0f, 20.0f, 30.0f));
		//	std::vector<std::pair<std::string, glm::vec3>> wave1;
		//	wave1.push_back(pair1);
		//	//wave1.push_back(pair2);
		//	//wave1.push_back(pair3);

		//	pair1 = std::make_pair("EnemyTypeB", glm::vec3(30.0f, 20.0f, 0.0f));
		//	//pair2 = std::make_pair("EnemyTypeB", glm::vec3(30.0f, 20.0f, 15.0f));
		//	//pair3 = std::make_pair("EnemyTypeB", glm::vec3(30.0f, 20.0f, 30.0f));
		//	std::vector<std::pair<std::string, glm::vec3>> wave2;
		//	wave2.push_back(pair1);
		//	//wave2.push_back(pair2);
		//	//wave2.push_back(pair3);

		//	pair1 = std::make_pair("EnemyTypeC", glm::vec3(0.0f, 20.0f, 30.0f));
		//	//pair2 = std::make_pair("EnemyTypeC", glm::vec3(15.0f, 20.0f, 30.0f));
		//	//pair3 = std::make_pair("EnemyTypeC", glm::vec3(30.0f, 20.0f, 30.0f));
		//	std::vector<std::pair<std::string, glm::vec3>> wave3;
		//	wave3.push_back(pair1);
		//	//wave3.push_back(pair2);
		//	//wave3.push_back(pair3);

		//	pair1 = std::make_pair("EnemyTypeD", glm::vec3(0.0f, 20.0f, 30.0f));
		//	//pair2 = std::make_pair("EnemyTypeD", glm::vec3(15.0f, 20.0f, 30.0f));
		//	//pair3 = std::make_pair("EnemyTypeD", glm::vec3(30.0f, 20.0f, 30.0f));
		//	std::vector<std::pair<std::string, glm::vec3>> wave4;
		//	wave4.push_back(pair1);
		//	//wave4.push_back(pair2);
		//	//wave4.push_back(pair3);

		//	pEntityDispatcher->SetWave(wave1);
		//	pEntityDispatcher->SetWave(wave2);
		//	pEntityDispatcher->SetWave(wave3);
		//	pEntityDispatcher->SetWave(wave4);
		//}

		//pEntityDispatcher->GetWave();
	}

	void CreateMaze()
	{
		cMazeMaker theMM;
		theMM.GenerateMaze(20, 20);
		theMM.PrintMaze();

		std::cout << std::endl;

		////	parseArgs(argc, argv);

		//maze_size[0] = 20;
		//maze_size[1] = 20;

		//// The width and height must be greater than or equal to 5 or it won't work
		//// The width and height must be odd or else we will have extra walls
		//for (int a = 0; a < 2; a++)
		//{
		//	if (maze_size[a] < 5)
		//	{
		//		maze_size[a] = 5;
		//	}
		//	else if (maze_size[a] % 2 == 0)
		//	{
		//		maze_size[a]--;
		//	}
		//}

		//initializeMaze();
		//randomPoint(false);
		//randomPoint(true);
		//generateMaze();

		//printMaze();
	}

	void CreateMazeBlockAt(cEntityManager* pEntityManager, cVaoMeshManager* pVaoMeshManager, glm::vec3 pos)
	{
		{
			sEntity* cube = pEntityManager->GenerateEntityOfType("StaticEntity");
			sMeshComponent* meshC = cube->GetComponent<sMeshComponent>();
			sLocationComponent* locationC = cube->GetComponent<sLocationComponent>();
			cube->SetIsActive(true);
			cube->SetName("Cube");
			cube->SetTag("NoSave");
			meshC->meshName = "cube.ply";
			sModelDrawInfo* meshInfo = new sModelDrawInfo();
			meshInfo->meshFileName = meshC->meshName;
			meshC->SetMesh(meshC->meshName);
			meshC->meshInfo = meshInfo;
			meshC->nonUniformScale = glm::vec3(2.0f);
			
			meshC->materialDiffuse = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			if (pos == glm::vec3(-196.0f, 2.0f, 200.0f))
			{
				meshC->materialDiffuse = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
			}
			meshC->bIsAffectedByLights = true;
			meshC->bIsVisible = true;
			sTextureInfo castleTexture;
			castleTexture.texName = "WWRock.bmp";
			castleTexture.texStrength = 1.0f;
			meshC->vecTextures.push_back(castleTexture);
			locationC->SetWorldPosition(pos);
		}
	}

	void Print3DMaze(cEntityManager* pEntityManager, cVaoMeshManager* pVaoMeshManager, cMazeMaker* pMaze)
	{
		float posX = -90.0f;
		float posZ = 90.0f;

		float cubeLength = 10.0f;

		for (unsigned int a = 0; a < pMaze->maze.size(); a++)
		{
			for (unsigned int b = 0; b < pMaze->maze[a].size(); b++)
			{
				if (pMaze->maze[a][b][0])
				{
					glm::vec3 pos = glm::vec3(posX, 5.0f, posZ);
					CreateMazeBlockAt(pEntityManager, pVaoMeshManager, pos);
				}

				posX += cubeLength;
			}

			posX = -90.0f;
			posZ -= cubeLength;
		}
	}

	void Print3DMaze(cEntityManager* pEntityManager, cVaoMeshManager* pVaoMeshManager, cMazeMaker* pMaze, std::vector<glm::vec3>& emptyPositions)
	{
		// 100x100 maze, so 4 in unit length each block
		// 100 * 4 = 400 each side of the maze

		float posX = -200.0f;
		float posZ = 200.0f;

		float cubeLength = 4.0f;

		for (unsigned int a = 0; a < pMaze->maze.size(); a++)
		{
			for (unsigned int b = 0; b < pMaze->maze[a].size(); b++)
			{
				if (pMaze->maze[a][b][0])
				{
					glm::vec3 pos = glm::vec3(posX, 2.0f, posZ);
					CreateMazeBlockAt(pEntityManager, pVaoMeshManager, pos);
				}
				else
				{
					emptyPositions.push_back(glm::vec3(posX, 2.0f, posZ));
				}

				posX += cubeLength;
			}

			posX = -200.0f;
			posZ -= cubeLength;
		}
	}

	void Print3DMaze(cEntityManager* pEntityManager, cVaoMeshManager* pVaoMeshManager, cMazeMaker* pMaze, std::vector<glm::vec3>& emptyPositions, std::vector<glm::vec2>& emptyBlocks)
	{
		// 100x100 maze, so 4 in unit length each block
		// 100 * 4 = 400 each side of the maze

		float posX = -200.0f;
		float posZ = 200.0f;

		float cubeLength = 4.0f;

		for (unsigned int a = 0; a < pMaze->maze.size(); a++)
		{
			for (unsigned int b = 0; b < pMaze->maze[a].size(); b++)
			{
				if (pMaze->maze[a][b][0])
				{
					glm::vec3 pos = glm::vec3(posX, 2.0f, posZ);
					CreateMazeBlockAt(pEntityManager, pVaoMeshManager, pos);
				}
				else
				{
					emptyPositions.push_back(glm::vec3(posX, 2.0f, posZ)); 
					emptyBlocks.push_back(glm::vec2(a, b));
				}

				posX += cubeLength;
			}

			posX = -200.0f;
			posZ -= cubeLength;
		}
	}

	void GetMazeEmptyPositions(cMazeMaker* pMaze, std::vector<glm::vec3>& emptyPositions)
	{
		float posX = -95.0f;
		float posZ = 95.0f;

		float cubeLength = 4.0f;

		for (unsigned int a = 0; a < pMaze->maze.size(); a++)
		{
			for (unsigned int b = 0; b < pMaze->maze[a].size(); b++)
			{
				if (!pMaze->maze[a][b][0])
				{
					emptyPositions.push_back(glm::vec3(posX, 2.0f, posZ));
				}

				posX += cubeLength;
			}

			posX = -95.0f;
			posZ -= cubeLength;
		}
	}
}
