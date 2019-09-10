// nGenerateMap.cpp

#include <Scene\nGenerateMap.h>
#include <Entity\sEntity.h>
#include <Entity\Components\sMeshComponent.h>
#include <Entity\Components\AI\sNeuralNetworkComponent.h>


namespace nGenerateMap
{
	void CreateBlockAt(cEntityManager* pEntityManager, cVaoMeshManager* pVaoMeshManager, glm::vec3 pos)
	{
		sEntity* cube = pEntityManager->GenerateEntityOfType("StaticEntity");
		sMeshComponent* meshC = cube->GetComponent<sMeshComponent>();
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
		meshC->bIsAffectedByLights = true;
		meshC->bIsVisible = true;
		sTextureInfo castleTexture;
		castleTexture.texName = "WWRock.bmp";
		castleTexture.texStrength = 1.0f;
		meshC->vecTextures.push_back(castleTexture);
		cube->transform->SetWorldPosition(glm::vec3(pos.x * meshC->nonUniformScale.x * 2.0f, 1.0f, pos.z * meshC->nonUniformScale.z * 2.0f));
	}

	void CreateNeuralCarAt(cEntityManager* pEntityManager, cVaoMeshManager* pVaoMeshManager, glm::vec3 pos)
	{
		sEntity* car = pEntityManager->GenerateEntityOfType("StaticEntity");
		sMeshComponent* meshC = car->GetComponent<sMeshComponent>();
		car->SetIsActive(true);
		car->SetName("Cube");
		car->SetTag("NoSave");
		meshC->meshName = "dalek2005_xyz_uv_res_2.ply";
		sModelDrawInfo* meshInfo = new sModelDrawInfo();
		meshInfo->meshFileName = meshC->meshName;
		meshC->SetMesh(meshC->meshName);
		meshC->nonUniformScale = glm::vec3(2.0f);

		meshC->materialDiffuse = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
		meshC->bIsAffectedByLights = true;
		meshC->bIsVisible = true;
		sTextureInfo castleTexture;
		castleTexture.texName = "goldFlan.bmp";
		castleTexture.texStrength = 1.0f;
		meshC->vecTextures.push_back(castleTexture);
		car->transform->SetWorldPosition(glm::vec3(pos.x * meshC->nonUniformScale.x * 2.0f, 1.0f, pos.z * meshC->nonUniformScale.z * 2.0f));

		car->AddComponent<sNeuralNetworkComponent>();
	}

	bool GenerateMapFromBMP(cBitmapLoader::sBMPInfoHeader* bitmapInfo, glm::vec3 initialPosition, std::vector<glm::vec3>& vecColour)
	{
		long width = 0;
		long height = 0;
		for (unsigned long i = 0; i < bitmapInfo->vecBitmap.size(); i += 3)
		{
			glm::vec3 colour = glm::vec3(bitmapInfo->vecBitmap[i], bitmapInfo->vecBitmap[i + 1], bitmapInfo->vecBitmap[i + 2]);

			// Read RGB for next 3 bytes
			vecColour.push_back(colour);

			width--;
			if (width <= -bitmapInfo->imageWidth)
			{
				width = 0;
				height--;
			}
		}

		return false;
	}

	bool GenerateMap0(cBitmapLoader::sBMPInfoHeader* bitmapInfo, cEntityManager* pEntityManager, cVaoMeshManager* pVaoMeshManager, glm::vec3 initialPos)
	{
		long width = 0;
		long height = 0;
		for (unsigned long i = 0; i < bitmapInfo->vecBitmap.size(); i += 3)
		{
			glm::vec3 colour = glm::vec3(bitmapInfo->vecBitmap[i], bitmapInfo->vecBitmap[i + 1], bitmapInfo->vecBitmap[i + 2]);

			// Read RGB for next 3 bytes
			if (colour == glm::vec3(0, 0, 0)) // Is red
			{
				CreateBlockAt(pEntityManager, pVaoMeshManager, glm::vec3(-width, 1.0f, height));
			}
			else if (colour == glm::vec3(0, 255, 0)) // Is green
			{

			}
			else if (colour == glm::vec3(0, 0, 255)) // Is blue
			{

			}

			width++;
			if (width >= bitmapInfo->imageWidth)
			{
				width = 0;
				height--;
			}
		}

		return false;
	}

	bool GenerateMap1(cBitmapLoader::sBMPInfoHeader* bitmapInfo, cEntityManager* pEntityManager, cVaoMeshManager* pVaoMeshManager, glm::vec3 initialPos)
	{
		long width = 0;
		long height = 0;
		for (unsigned long i = 0; i < bitmapInfo->vecBitmap.size(); i += 3)
		{
			glm::vec3 colour = glm::vec3(bitmapInfo->vecBitmap[i], bitmapInfo->vecBitmap[i + 1], bitmapInfo->vecBitmap[i + 2]);

			// Read RGB for next 3 bytes
			if (colour == glm::vec3(0, 0, 0)) // Is red
			{
				CreateBlockAt(pEntityManager, pVaoMeshManager, glm::vec3(-width, 1.0f, height));
			}
			else if (colour == glm::vec3(0, 255, 0)) // Is green
			{

			}
			else if (colour == glm::vec3(0, 0, 255)) // Is blue
			{

			}

			width++;
			if (width >= bitmapInfo->imageWidth)
			{
				width = 0;
				height--;
			}
		}

		return false;
	}

	bool GenerateMapFromBMP(cBitmapLoader::sBMPInfoHeader* bitmapInfo, cEntityManager* pEntityManager, cVaoMeshManager* pVaoMeshManager, glm::vec3 initialPos, unsigned int level)
	{
		if (level == 0)
		{
			GenerateMap0(bitmapInfo, pEntityManager, pVaoMeshManager, initialPos);
		}
		else if (level == 1)
		{
			GenerateMap1(bitmapInfo, pEntityManager, pVaoMeshManager, initialPos);
		}

		return false;
	}
}
