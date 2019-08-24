// cScene.cpp

#include <Scene\cScene.h>
#include <Scene\nGenerateMap.h>
#include <Entity\Systems\cDrawSystem.h>
#include <AI\cDijkstra.h>
#include <AI\cAStar.h>

#include <Entity\Components\sMeshComponent.h>
#include <Entity\Components\AI\sNeuralNetworkComponent.h>


cScene::cScene(cVaoMeshManager* pVaoMeshManager, cEntityManager* pEntityManager)
{
	this->_pVaoMeshManager = pVaoMeshManager;
	this->_pEntityManager = pEntityManager;
	this->agentNodeIndex = 0;
	this->gatherTime = 7.0f;
	this->baseTime = 2.0f;

	return;
}

cScene::~cScene()
{
	this->EndScene();

	return;
}

void cScene::_BeginDijkstraAStarScene()
{
	cBitmapLoader* map_0 = new cBitmapLoader();
	map_0->OpenBinaryFile("map_0.bmp");
	map_0->ReadData(54);

	std::vector<glm::vec3> vecColour;
	nGenerateMap::GenerateMapFromBMP(map_0->bmpInfo, glm::vec3(0.0f), vecColour);

	this->_pDijkstra = new cDijkstra();
	this->_pAStar = new cAStar();

	long width = 0;
	long height = 0;

	// Generate all nodes and objects
	for (unsigned long i = 0; i < vecColour.size(); i++)
	{
		float nodeSize = 2.0f;
		glm::vec3 colour = vecColour[i];
		glm::vec3 position = glm::vec3(-width * nodeSize, 0.0f, height * nodeSize);

		// Generate all nodes and objects
		if (colour == glm::vec3(0, 0, 0)) // Blacks are walls
		{
			this->_CreateBlockAt(position, nodeSize);
			this->_pDijkstra->CreateNode("Wall", position, 5);
			this->_pAStar->CreateNode("Wall", position, 5);
		}
		else if (colour == glm::vec3(0, 255, 0)) // Green is the gatherer
		{
			this->_CreateAgentAt(position, nodeSize);
			sPathNode* nodeA = this->_pAStar->CreateNode("Empty", position, 0, true);
			sPathNode* node = this->_pDijkstra->CreateNode("Start", position, 0);
			this->_pDijkstra->SetStartNode(node);
		}
		else if (colour == glm::vec3(0, 0, 255)) // Blue is the base
		{
			this->_CreateBaseAt(position, nodeSize);
			sPathNode* nodeA = this->_pAStar->CreateNode("Base", position, 0);
			sPathNode* node = this->_pDijkstra->CreateNode("Empty", position, 0);
		}
		else if (colour == glm::vec3(255, 255, 0)) // Yellows are resources
		{
			sEntity* resource = this->_CreateResourceAt(position, nodeSize);
			sPathNode* nodeA = this->_pAStar->CreateNode("Empty", position, 0);
			sPathNode* node = this->_pDijkstra->CreateNode("Resource", position, 0);
			nodeA->nodeEntity = resource;
			node->nodeEntity = resource;
		}
		else if (colour == glm::vec3(255, 255, 255)) // Whites are empty spaces
		{
			this->_pAStar->CreateNode("Empty", position, 0);
			this->_pDijkstra->CreateNode("Empty", position, 0);
		}

		width--;
		if (width <= -map_0->bmpInfo->imageWidth)
		{
			width = 0;
			height--;
		}
	}

	this->_height = map_0->bmpInfo->imageHeight;
	this->_width = map_0->bmpInfo->imageWidth;
	this->_pAStar->InitializeNodes(map_0->bmpInfo->imageHeight, map_0->bmpInfo->imageWidth, true);
	this->_pDijkstra->InitializeNodes(map_0->bmpInfo->imageHeight, map_0->bmpInfo->imageWidth, true);
}

void cScene::_BeginNeuralNetworkScene()
{
	cBitmapLoader* map_0 = new cBitmapLoader();
	map_0->OpenBinaryFile("map_1.bmp");
	map_0->ReadData(54);

	std::vector<glm::vec3> vecColour;
	nGenerateMap::GenerateMapFromBMP(map_0->bmpInfo, glm::vec3(0.0f), vecColour);

	long width = 0;
	long height = 0;

	// Generate all nodes and objects
	for (unsigned long i = 0; i < vecColour.size(); i++)
	{
		float nodeSize = 2.0f;
		glm::vec3 colour = vecColour[i];
		glm::vec3 position = glm::vec3(-width * nodeSize, 0.0f, height * nodeSize);

		// Generate all nodes and objects
		if (colour == glm::vec3(0, 0, 0)) // Blacks are walls
		{
			this->_CreateBlockAt(position, nodeSize);
		}
		else if (colour == glm::vec3(0, 255, 0)) // Green is the car
		{
			this->_CreateNeuralCarAt(position, nodeSize);
		}
		else if (colour == glm::vec3(0, 0, 255)) // Blue is the base
		{
			
		}
		else if (colour == glm::vec3(255, 255, 0)) // Yellows are resources
		{
			
		}
		else if (colour == glm::vec3(255, 255, 255)) // Whites are empty spaces
		{

		}

		width--;
		if (width <= -map_0->bmpInfo->imageWidth)
		{
			width = 0;
			height--;
		}
	}

	this->_height = map_0->bmpInfo->imageHeight;
	this->_width = map_0->bmpInfo->imageWidth;
}

void cScene::_CreateAgentAt(glm::vec3 pos, float nodeSize)
{
	sEntity* cube = this->_pEntityManager->GenerateEntityOfType("StaticEntity");
	sMeshComponent* meshC = cube->GetComponent<sMeshComponent>();
	cube->SetIsActive(true);
	cube->SetName("Cube");
	cube->SetTag("NoSave");
	meshC->meshName = "cube.ply";
	sModelDrawInfo* meshInfo = new sModelDrawInfo();
	meshInfo->meshFileName = meshC->meshName;
	meshC->SetMesh(meshC->meshName);
	meshC->meshInfo = meshInfo;
	meshC->nonUniformScale = glm::vec3(nodeSize * 0.5f);

	meshC->materialDiffuse = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	meshC->bIsAffectedByLights = true;
	meshC->bIsVisible = true;
	sTextureInfo castleTexture;
	castleTexture.texName = "WWRock.bmp";
	castleTexture.texStrength = 1.0f;
	meshC->vecTextures.push_back(castleTexture);
	cube->transform->SetWorldPosition(glm::vec3(pos.x, 1.0f, pos.z));

	this->agent = cube;
}

void cScene::_CreateBlockAt(glm::vec3 pos, float nodeSize)
{
	sEntity* cube = this->_pEntityManager->GenerateEntityOfType("StaticPrefab");
	sMeshComponent* meshC = cube->GetComponent<sMeshComponent>();
	cube->SetIsActive(true);
	cube->SetName("Cube");
	cube->SetTag("Obstacle");
	meshC->meshName = "cube.ply";
	sModelDrawInfo* meshInfo = new sModelDrawInfo();
	meshInfo->meshFileName = meshC->meshName;
	meshC->SetMesh(meshC->meshName);
	meshC->meshInfo = meshInfo;
	meshC->nonUniformScale = glm::vec3(nodeSize * 0.5f);

	meshC->materialDiffuse = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	meshC->bIsAffectedByLights = true;
	meshC->bIsVisible = true;
	sTextureInfo castleTexture;
	castleTexture.texName = "WWRock.bmp";
	castleTexture.texStrength = 1.0f;
	meshC->vecTextures.push_back(castleTexture);
	cube->transform->SetWorldPosition(glm::vec3(pos.x, 1.0f, pos.z));
}

void cScene::_CreateBaseAt(glm::vec3 pos, float nodeSize)
{
	sEntity* resource = this->_pEntityManager->GenerateEntityOfType("StaticEntity");
	sMeshComponent* meshC = resource->GetComponent<sMeshComponent>();
	resource->SetIsActive(true);
	resource->SetName("Cube");
	resource->SetTag("NoSave");
	meshC->meshName = "cube.ply";
	sModelDrawInfo* meshInfo = new sModelDrawInfo();
	meshInfo->meshFileName = meshC->meshName;
	meshC->SetMesh(meshC->meshName);
	meshC->meshInfo = meshInfo;
	meshC->nonUniformScale = glm::vec3(1.0f);

	meshC->materialDiffuse = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	meshC->bIsAffectedByLights = true;
	meshC->bIsVisible = true;
	sTextureInfo castleTexture;
	castleTexture.texName = "WWRock.bmp";
	castleTexture.texStrength = 1.0f;
	meshC->vecTextures.push_back(castleTexture);
	resource->transform->SetWorldPosition(glm::vec3(pos.x, 1.0f, pos.z));
}

sEntity* cScene::_CreateResourceAt(glm::vec3 pos, float nodeSize)
{
	sEntity* resource = this->_pEntityManager->GenerateEntityOfType("StaticEntity");
	sMeshComponent* meshC = resource->GetComponent<sMeshComponent>();
	resource->SetIsActive(true);
	resource->SetName("Cube");
	resource->SetTag("NoSave");
	meshC->meshName = "physicsTetrahedron.ply";
	sModelDrawInfo* meshInfo = new sModelDrawInfo();
	meshInfo->meshFileName = meshC->meshName;
	meshC->SetMesh(meshC->meshName);
	meshC->meshInfo = meshInfo;
	meshC->nonUniformScale = glm::vec3(1.0f);

	meshC->materialDiffuse = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	meshC->bIsAffectedByLights = true;
	meshC->bIsVisible = true;
	sTextureInfo castleTexture;
	castleTexture.texName = "goldFlan.bmp";
	castleTexture.texStrength = 1.0f;
	meshC->vecTextures.push_back(castleTexture);
	resource->transform->SetWorldPosition(glm::vec3(pos.x, 1.0f, pos.z));

	return resource;
}

void cScene::_CreateNeuralCarAt(glm::vec3 pos, float nodeSize)
{
	sEntity* car = this->_pEntityManager->GenerateEntityOfType("StaticPrefab");
	sMeshComponent* meshC = car->GetComponent<sMeshComponent>();
	car->SetIsActive(true);
	car->SetName("Dalek");
	car->SetTag("Obstacle");
	meshC->meshName = "dalek2005_xyz_uv_res_2.ply";
	sModelDrawInfo* meshInfo = new sModelDrawInfo();
	meshInfo->meshFileName = meshC->meshName;
	meshC->SetMesh(meshC->meshName);
	meshC->meshInfo = meshInfo;
	meshC->nonUniformScale = glm::vec3(1.0f);

	meshC->materialDiffuse = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	meshC->bIsAffectedByLights = true;
	meshC->bIsVisible = true;
	sTextureInfo castleTexture;
	castleTexture.texName = "goldFlan.bmp";
	castleTexture.texStrength = 1.0f;
	meshC->vecTextures.push_back(castleTexture);
	car->transform->SetWorldPosition(glm::vec3(pos.x, 1.0f, pos.z));

	car->AddComponent<sNeuralNetworkComponent>();
}

void cScene::_DrawPath(cDrawSystem* pDrawSystem, GLuint shaderPorgramID)
{
	if (this->_pathType == DIJKSTRA)
	{
		this->_pDijkstra->DrawPath(pDrawSystem, shaderPorgramID);
	}
	else if (this->_pathType == ASTAR)
	{
		this->_pAStar->DrawPath(pDrawSystem, shaderPorgramID);
	}
}

void cScene::BeginScene(unsigned int index) 
{
	if (index == 0)
	{
		this->_BeginDijkstraAStarScene();
	}
	else if (index == 1)
	{
		this->_BeginNeuralNetworkScene();
	}
}

//void cScene::UpdateScene(class cDrawSystem* pDrawSystem, GLuint shaderPorgramID, double deltaTime)
//{
//	float agentDistance = std::numeric_limits<float>::max();
//	glm::vec3 direction = glm::vec3(0.0f);
//
//	switch (this->_agentState)
//	{
//	case IDLE:
//		// Imediately go into search
//		this->_agentState = SEARCH;
//		break;
//	case SEARCH:
//		// Search for a resource
//		this->_pathType = DIJKSTRA;
//		this->_pDijkstra->goalName = "Resource";
//
//		this->_vecDjNodes.clear();
//		// Found a resource, go gather it
//		if (this->_pDijkstra->MakePath(this->_vecDjNodes))
//		{
//			this->_agentState = GATHER;
//		}
//		else
//		{
//			// No resources left, done
//			this->_agentState = DONE;
//		}
//		break;
//	case GATHER:
//		// Move to resource and gather
//		agentDistance = fabs(glm::distance(this->agent->transform->position, this->_vecDjNodes[this->agentNodeIndex]->_position));
//		if (agentDistance > 1.0f)
//		{
//			// Get direction and move
//			direction = this->_vecDjNodes[this->agentNodeIndex]->_position - this->agent->transform->position;
//			direction = glm::normalize(direction);
//			this->agent->transform->position += direction * (float)deltaTime;
//		}
//		else if (agentDistance <= 1.0f)
//		{
//			if (this->agentNodeIndex != this->_vecDjNodes.size() - 1)
//			{
//				this->agentNodeIndex++;
//			}
//			else
//			{
//				// Check gather timer
//				this->gatherTime -= 1.0f * (float)deltaTime;
//				if (this->gatherTime <= 0.0f)
//				{
//					// Go back when finished
//					this->gatherTime = 7.0f;
//					this->_pathType = ASTAR;
//					this->_pAStar->goalName = "Base";
//					this->_vecDjNodes[this->agentNodeIndex]->nodeEntity->bIsActive = false;
//					this->_vecDjNodes[this->agentNodeIndex]->bIsActive = false;
//					this->agentNodeIndex = 0;
//
//					this->_vecAsNodes.clear();
//					if (this->_pAStar->MakePath(this->_vecAsNodes))
//					{
//						// Go back to base
//						this->_agentState = RETURN;
//					}
//				}
//			}
//		}
//
//		break;
//	case RETURN:
//		// Move back to base
//		agentDistance = fabs(glm::distance(this->agent->transform->position, this->_vecAsNodes[this->agentNodeIndex]->_position));
//
//		direction = this->_vecAsNodes[this->agentNodeIndex]->_position - this->agent->transform->position;
//		direction = glm::normalize(direction);
//
//		this->agent->transform->position += direction * (float)deltaTime;
//
//		if (agentDistance <= 1.0f)
//		{
//			if (this->agentNodeIndex != this->_vecAsNodes.size())
//			{
//				this->agentNodeIndex++;
//			}
//			else
//			{
//				// Check gather timer
//				this->gatherTime -= 1.0f * deltaTime;
//				if (this->gatherTime <= 0.0f)
//				{
//					// Go back when finished
//					this->gatherTime = 7.0f;
//					this->agentNodeIndex = 0;
//					this->_vecAsNodes.clear();
//					this->_agentState = SEARCH;
//				}
//			}
//		}
//		break;
//	case DONE:
//		// Don't do anything here for now
//
//		break;
//	}
//
//	this->_DrawPath(pDrawSystem, shaderPorgramID);
//}

void cScene::UpdateScene(class cDrawSystem* pDrawSystem, GLuint shaderPorgramID, double deltaTime)
{
	float agentDistance = std::numeric_limits<float>::max();
	glm::vec3 direction = glm::vec3(0.0f);

	switch (this->_agentState)
	{
	case IDLE:
		// Imediately go into search
		this->_agentState = SEARCH;
		break;
	case SEARCH:
		// Search for a resource
		this->_pathType = DIJKSTRA;
		this->_deqDjNodes.clear();

		this->_pDijkstra->goalName = "Resource";
		// Found a resource, go gather it
		if (this->_pDijkstra->MakePath(this->_deqDjNodes))
		{
			this->_agentState = GATHER;
		}
		else
		{
			// No resources left, done
			this->_agentState = DONE;
		}
		break;
	case GATHER:
		// Move to resource and gather
		agentDistance = fabs(glm::distance(this->agent->transform->GetWorldPosition(), this->_deqDjNodes[this->agentNodeIndex]->_position));
		if (agentDistance > 1.0f)
		{
			// Get direction and move
			direction = this->_deqDjNodes[this->agentNodeIndex]->_position - this->agent->transform->GetWorldPosition();
			direction = glm::normalize(direction);
			this->agent->transform->SetWorldPosition(this->agent->transform->GetWorldPosition() + direction * 15.0f * (float)deltaTime);
		}
		else if (agentDistance <= 1.0f)
		{
			if (this->agentNodeIndex != this->_deqDjNodes.size() - 1)
			{
				this->agentNodeIndex++;
			}
			else
			{
				// Check gather timer
				this->gatherTime -= 1.0f * (float)deltaTime;
				if (this->gatherTime <= 0.0f)
				{
					// Go back when finished
					this->gatherTime = 7.0f;
					
					this->_deqDjNodes[this->agentNodeIndex]->nodeEntity->SetIsActive(false);
					this->_deqDjNodes[this->agentNodeIndex]->bIsActive = false;

					this->_pathType = ASTAR;
					this->_pAStar->SetStartNode(this->agent->transform->GetWorldPosition());
					this->_pAStar->SetGoalNode("Base");
					this->_pAStar->InitializeNodes(this->_height, this->_width, false);
					this->_deqAsNodes.clear();
					this->agentNodeIndex = 0;
					
					if (this->_pAStar->MakePath(this->_deqAsNodes))
					{
						// Go back to base
						this->_agentState = RETURN;
					}
				}
			}
		}

		break;
	case RETURN:
		// Move back to base
		agentDistance = fabs(glm::distance(this->agent->transform->GetWorldPosition(), this->_deqAsNodes[this->agentNodeIndex]->_position));

		if(agentDistance > 1.0f)
		{
			direction = this->_deqAsNodes[this->agentNodeIndex]->_position - this->agent->transform->GetWorldPosition();
			direction = glm::normalize(direction);

			this->agent->transform->SetWorldPosition(this->agent->transform->GetWorldPosition() + direction * 15.0f * (float)deltaTime);
		}
		else if (agentDistance <= 1.0f)
		{
			if (this->agentNodeIndex != this->_deqAsNodes.size() - 1)
			{
				this->agentNodeIndex++;
			}
			else
			{
				// Check base timer
				this->baseTime -= 1.0f * (float)deltaTime;
				if (this->baseTime <= 0.0f)
				{
					// Go back when finished
					this->baseTime = 2.0f;
					this->agentNodeIndex = 0;

					this->_pDijkstra->SetStartNode(this->agent->transform->GetWorldPosition());
					this->_pDijkstra->InitializeNodes(this->_height, this->_width, false);
					this->_agentState = SEARCH;
				}
			}
		}
		break;
	case DONE:
		// Don't do anything here for now
		printf("Done.\n");

		break;
	}

	this->_DrawPath(pDrawSystem, shaderPorgramID);
}

void cScene::EndScene()
{
	delete this->_pDijkstra;
	delete this->_pAStar;
}
