// nAssetPack_0.cpp

#include "nAssetPack_0.h"
#include <Texture\cTextureManager.h>


cAssetManager::sAssetpack* nAssets::GenerateAssetPack_0(cAssetManager* pAssetManager)
{
	cAssetManager::sAssetpack* pack0 = pAssetManager->CreateAssetPack("StarterPack");
	pack0->AddMeshAsset("castle.ply");
	pack0->AddMeshAsset("lowres_castle.ply");
	pack0->AddMeshAsset("cube.ply");
	pack0->AddMeshAsset("TestIsland_0.ply");
	pack0->AddMeshAsset("lowres_TestIsland_0.ply");
	pack0->AddMeshAsset("TestIsland_2.ply");
	pack0->AddMeshAsset("lowres_TestIsland_1.ply");
	pack0->AddMeshAsset("shangai.ply");
	pack0->AddMeshAsset("lowres_shangai.ply");
	pack0->AddMeshAsset("lightIsland.ply");
	pack0->AddMeshAsset("lowres_lightIsland.ply");
	pack0->AddMeshAsset("masterSword.ply");
	pack0->AddMeshAsset("lowres_masterSword.ply");
	pack0->AddMeshAsset("debugSphere.ply");
	pack0->AddMeshAsset("lowres_debugSphere.ply");
	pack0->AddMeshAsset("physicsSphere.ply");
	pack0->AddMeshAsset("lowres_physicsSphere.ply");
	pack0->AddMeshAsset("physicsPlane.ply");
	pack0->AddMeshAsset("lowres_physicsPlane.ply");
	pack0->AddMeshAsset("physicsBox.ply");
	pack0->AddMeshAsset("lowres_physicsBox.ply");
	pack0->AddMeshAsset("physicsCylinder.ply");
	pack0->AddMeshAsset("lowres_physicsCylinder.ply");
	pack0->AddMeshAsset("lowPolySphere.ply");
	pack0->AddMeshAsset("lowres_lowPolySphere.ply");
	pack0->AddMeshAsset("skyboxSphere.ply");
	pack0->AddMeshAsset("lowres_skyboxSphere.ply");
	pack0->AddMeshAsset("AABB.ply");
	pack0->AddMeshAsset("lowres_AABB.ply");
	pack0->AddMeshAsset("1x1_2Tri_Quad_2_Sided_xyz_n_uv.ply");

	// Skinned Meshes
	pack0->AddSkinnedMeshAsset("ely.fbx");
	pack0->AddAnimationAsset("ely-Idle.fbx", "Idle");
	pack0->AddAnimationAsset("ely-Walk.fbx", "Walk");
	pack0->AddAnimationAsset("ely-Run.fbx", "Run");
	pack0->AddAnimationAsset("ely-Jump-Up.fbx", "JumpUp");
	pack0->AddAnimationAsset("ely-Jump-Air.fbx", "JumpAir");
	pack0->AddAnimationAsset("ely-Land1.fbx", "JumpLand1");
	pack0->AddAnimationAsset("ely-Land2.fbx", "JumpLand2");
	pack0->AddAnimationAsset("ely-Crouch.fbx", "Crouch");
	pack0->AddAnimationAsset("ely-Fly2.fbx", "Fly");
	pack0->AddAnimationAsset("ely-LandHard.fbx", "LandHard");
	pack0->AddAnimationAsset("ely-Outward-Slash.fbx", "Attack_1");

	return pack0;

	//unsigned int finalNumThreads = std::max<unsigned int>(1, numThreads);

	//// Load asset packs
	//pAssetManager->BeginLoadProcess(finalNumThreads);

	//while (!pAssetManager->ready)
	//{
	//	pAssetManager->Update(shaderProgramID);
	//}

	//if (numThreads > 0)
	//{
	//	pTextureManager->SetBasePath("assets/textures");
	//	pTextureManager->CreateTextureTripleBuffer();

	//	// Load low res textures
	//	//LoadTexture("angkorWatTex.bmp", cTextureManager::DEFAULT, pTextureManager);
	//	//LoadTexture("fahrenheit.bmp", cTextureManager::DEFAULT, pTextureManager);
	//	pTextureManager->LoadTexture("WWPawprintIslandGrass.bmp", cTextureManager::LOW_RES, pTextureManager);
	//	pTextureManager->LoadTexture("angkorWatTex.bmp", cTextureManager::LOW_RES, pTextureManager);
	//	pTextureManager->LoadTexture("shangai.bmp", cTextureManager::LOW_RES, pTextureManager);
	//	pTextureManager->LoadTexture("lightIsland.bmp", cTextureManager::LOW_RES, pTextureManager);
	//	pTextureManager->LoadTexture("WWRock.bmp", cTextureManager::LOW_RES, pTextureManager);
	//	pTextureManager->LoadTexture("stone.bmp", cTextureManager::LOW_RES, pTextureManager);
	//	pTextureManager->LoadTexture("goldFlan.bmp", cTextureManager::LOW_RES, pTextureManager);
	//	pTextureManager->LoadTexture("fireFlan.bmp", cTextureManager::LOW_RES, pTextureManager);
	//	pTextureManager->LoadTexture("ely.bmp", cTextureManager::LOW_RES, pTextureManager);
	//	pTextureManager->LoadTexture("WWWood.bmp", cTextureManager::LOW_RES, pTextureManager);
	//}
	//
	//// Load high res textures
	//pTextureManager->LoadTexture("WWPawprintIslandGrass.bmp", cTextureManager::HIGH_RES, pTextureManager);
	//pTextureManager->LoadTexture("angkorWatTex.bmp", cTextureManager::HIGH_RES, pTextureManager);
	//pTextureManager->LoadTexture("shangai.bmp", cTextureManager::HIGH_RES, pTextureManager);
	//pTextureManager->LoadTexture("lightIsland.bmp", cTextureManager::HIGH_RES, pTextureManager);
	//pTextureManager->LoadTexture("WWRock.bmp", cTextureManager::HIGH_RES, pTextureManager);
	//pTextureManager->LoadTexture("stone.bmp", cTextureManager::HIGH_RES, pTextureManager);
	//pTextureManager->LoadTexture("goldFlan.bmp", cTextureManager::HIGH_RES, pTextureManager);
	//pTextureManager->LoadTexture("fireFlan.bmp", cTextureManager::HIGH_RES, pTextureManager);
	//pTextureManager->LoadTexture("ely.bmp", cTextureManager::HIGH_RES, pTextureManager);
	//pTextureManager->LoadTexture("WWWood.bmp", cTextureManager::HIGH_RES, pTextureManager);
}
