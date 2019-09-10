// cAABBManager.cpp

#include <AABB\cAABBManager.h>
#include <Entity\sEntity.h>
#include <Pools\cPoolManager.h>
#include <Mesh\cVaoMeshManager.h>
#include <Entity\Components\sMeshComponent.h>
#include <iostream>


cAABBManager::cAABBManager(cVaoMeshManager* pVaoMeshManager)
{
	this->_pVaoMeshManager = pVaoMeshManager;

	return;
}

cAABBManager::~cAABBManager()
{
	return;
}

void cAABBManager::GenerateAABBs(float distanceBetween)
{
	// Get all the triangles from the terrain and load
	// the information into the vector.
	// If the object is moving, we might need to save the 
	// object start position and offset the triangle position by it,
	// since the triangle information should not change!

	sEntity* pTerrain = cPoolManager::GetInstance()->GetEntityByName("terrain");
	sMeshComponent* mesh = pTerrain->GetComponent<sMeshComponent>();
	sModelDrawInfo terrainMeshInfo = *mesh->meshInfo;

	// How big the AABB is. The bigger it is, the more triangles it will contain
	// The smaller, the more AABBs we will have, with fewer triangles inside
	float sideLength = distanceBetween;

	for (unsigned int triIndex = 0; triIndex != terrainMeshInfo.numberOfTriangles; triIndex++)
	{
		// for each triangle, for each vertex, determine which AABB the triangle should be in
		// (if your mesh has been transformed, then you need to transform the tirangles 
		//  BEFORE you do this... or just keep the terrain UNTRANSFORMED)

		sMeshTriangle currentTri = terrainMeshInfo.originalMeshInfo->pTrianglesFromFile[triIndex];

		sMeshVertex currentVerts[3];
		currentVerts[0] = terrainMeshInfo.originalMeshInfo->pVerticesFromFile[currentTri.vertex1];
		currentVerts[1] = terrainMeshInfo.originalMeshInfo->pVerticesFromFile[currentTri.vertex2];
		currentVerts[2] = terrainMeshInfo.originalMeshInfo->pVerticesFromFile[currentTri.vertex3];

		// Save the file triangle vertices into the manager struct triangle
		cAABB::sAABBTriangle AABBTriangle;
		for (int i = 0; i != 3; i++)
		{
			AABBTriangle.vertices[i].x = currentVerts[i].x;
			AABBTriangle.vertices[i].y = currentVerts[i].y;
			AABBTriangle.vertices[i].z = currentVerts[i].z;
		}

		// Is the triangle "too big", and if so, split it (take centre and make 3 more)
		// (Pro Tip: "too big" is the SMALLEST side is greater than HALF the AABB length)
		// Use THOSE triangles as the test (and recursively do this if needed),
		// +++BUT+++ store the ORIGINAL triangle info NOT the subdivided one
		// 
		// For the student to complete... 
		// 

		cAABB* tempAABB = nullptr; // We will use this to create a control variable to store the triangles in the end!
		cAABB* newTempAABB = nullptr; // Control variable for newly created AABBs
		for (unsigned int vertIndex = 0; vertIndex != 3; vertIndex++)
		{
			// Generate the ID for this vertex
			//AABBTriangle.vertices[0] = glm::vec3(100.0f, 0.0f, 0.0f); // TEST
			unsigned long long AABB_ID = cAABB::GenerateID(AABBTriangle.vertices[vertIndex], sideLength);

			// Check if the ID already exists. If it does, this vertex is inside an AABB that already exists
			std::map< unsigned long long/*ID AABB*/, cAABB* >::iterator itAABB = this->_pMapAABB.find(AABB_ID);
			

			// Reached the end, didn't find an AABB for this vertex
			if (itAABB == this->_pMapAABB.end())
			{
				// Determine the AABB location for this point
				// (like the generateID() method...)
				glm::vec3 centerXYZ = AABBTriangle.vertices[vertIndex];
				// Test: 2,5 normally == 0, so x == 0. in 7,5 x == 1, but that's not good for positioning, since the
				// AABBs would be inside each other. They need to be offset by their whole length. This is why we multiply in the end.
				// So, 2,5 x == 0, 7,5 x == 1 * length, which means it will be 1 length farther away.
				centerXYZ.x = ((floor((centerXYZ.x - ((float)sideLength / 2.0f)) / sideLength)) * sideLength) + sideLength;
				centerXYZ.y = ((floor((centerXYZ.y - ((float)sideLength / 2.0f)) / sideLength)) * sideLength) + sideLength;
				centerXYZ.z = ((floor((centerXYZ.z - ((float)sideLength / 2.0f)) / sideLength)) * sideLength) + sideLength;

				// Create AABB and set center and side length values
				cAABB* pAABB = new cAABB(centerXYZ, sideLength, "AABB.ply");
				
				/*pAABB->meshName = "AABB.ply";
				pAABB->position = centerXYZ;
				pAABB->nonUniformScale = glm::vec3(sideLength);
				pAABB->bIsWireframe = true;
				pAABB->bIsVisible = false;*/

				// The ID's should be the same. Otherwise, we have a bug
				unsigned long long the_AABB_ID = pAABB->GetID();
				if (AABB_ID != the_AABB_ID)
				{
					std::cerr << "AABB ID's didn't match!!" << std::endl;
				}
				// Set AABB on the map
				this->_pMapAABB[the_AABB_ID] = pAABB;

				// Then set the iterator to the AABB, by running find again and add the triangle to it
				itAABB = this->_pMapAABB.find(the_AABB_ID);
				itAABB->second->_pVecAABBTriangles.push_back(AABBTriangle);
				newTempAABB = itAABB->second;
			}
			else
			{
				if (tempAABB == nullptr)
				{
					// The vertex was on an already existing AABB, but have we already set the
					// triangle on it? We won't if the control variable is still null.
					// The control variable will exist until the vertex loop lasts
					tempAABB = itAABB->second;
					itAABB->second->_pVecAABBTriangles.push_back(AABBTriangle);
				}
				else if (itAABB->second != tempAABB && itAABB->second != newTempAABB)
				{
					// This AABB stil hasn't received this triangle
					itAABB->second->_pVecAABBTriangles.push_back(AABBTriangle);
				}
			}
			// Here, the map has finished checking for the AABB and either triggered the end above and created a new one,
			// or we already had an AABB at that point. We add the new triangle to whatever AABB this is in, BUT we can't repeat!
			//itAABB->second->_pVecAABBTriangles.push_back(AABBTriangle);
		}
	}

	return;
}

void cAABBManager::GenerateAABBs(std::string meshName, float distanceBetween)
{
	// Get all the triangles from the terrain and load
	// the information into the vector.
	// If the object is moving, we might need to save the 
	// object start position and offset the triangle position by it,
	// since the triangle information should not change!

	sEntity* pTerrain = cPoolManager::GetInstance()->GetEntityByName(meshName);
	sMeshComponent* mesh = pTerrain->GetComponent<sMeshComponent>();
	sModelDrawInfo terrainMeshInfo = *mesh->meshInfo;

	// How big the AABB is. The bigger it is, the more triangles it will contain
	// The smaller, the more AABBs we will have, with fewer triangles inside
	float sideLength = distanceBetween;

	for (unsigned int triIndex = 0; triIndex != terrainMeshInfo.numberOfTriangles; triIndex++)
	{
		// for each triangle, for each vertex, determine which AABB the triangle should be in
		// (if your mesh has been transformed, then you need to transform the tirangles 
		//  BEFORE you do this... or just keep the terrain UNTRANSFORMED)

		sMeshTriangle currentTri = terrainMeshInfo.originalMeshInfo->pTrianglesFromFile[triIndex];

		sMeshVertex currentVerts[3];
		currentVerts[0] = terrainMeshInfo.originalMeshInfo->pVerticesFromFile[currentTri.vertex1];
		currentVerts[1] = terrainMeshInfo.originalMeshInfo->pVerticesFromFile[currentTri.vertex2];
		currentVerts[2] = terrainMeshInfo.originalMeshInfo->pVerticesFromFile[currentTri.vertex3];

		// Save the file triangle vertices into the manager struct triangle
		cAABB::sAABBTriangle AABBTriangle;
		for (int i = 0; i != 3; i++)
		{
			AABBTriangle.vertices[i].x = currentVerts[i].x;
			AABBTriangle.vertices[i].y = currentVerts[i].y;
			AABBTriangle.vertices[i].z = currentVerts[i].z;

			AABBTriangle.vertices[i].x *= mesh->nonUniformScale.x;
			AABBTriangle.vertices[i].y *= mesh->nonUniformScale.y;
			AABBTriangle.vertices[i].z *= mesh->nonUniformScale.z;

			AABBTriangle.vertices[i].x += pTerrain->transform->GetWorldPosition().x;
			AABBTriangle.vertices[i].y += pTerrain->transform->GetWorldPosition().y;
			AABBTriangle.vertices[i].z += pTerrain->transform->GetWorldPosition().z;
		}

		// Is the triangle "too big", and if so, split it (take centre and make 3 more)
		// (Pro Tip: "too big" is the SMALLEST side is greater than HALF the AABB length)
		// Use THOSE triangles as the test (and recursively do this if needed),
		// +++BUT+++ store the ORIGINAL triangle info NOT the subdivided one
		// 
		// For the student to complete... 
		// 

		cAABB* tempAABB = nullptr; // We will use this to create a control variable to store the triangles in the end!
		cAABB* newTempAABB = nullptr; // Control variable for newly created AABBs
		for (unsigned int vertIndex = 0; vertIndex != 3; vertIndex++)
		{
			// Generate the ID for this vertex
			//AABBTriangle.vertices[0] = glm::vec3(100.0f, 0.0f, 0.0f); // TEST
			unsigned long long AABB_ID = cAABB::GenerateID(AABBTriangle.vertices[vertIndex], sideLength);

			// Check if the ID already exists. If it does, this vertex is inside an AABB that already exists
			std::map< unsigned long long/*ID AABB*/, cAABB* >::iterator itAABB = this->_pMapAABB.find(AABB_ID);


			// Reached the end, didn't find an AABB for this vertex
			if (itAABB == this->_pMapAABB.end())
			{
				// Determine the AABB location for this point
				// (like the generateID() method...)
				glm::vec3 centerXYZ = AABBTriangle.vertices[vertIndex];
				// Test: 2,5 normally == 0, so x == 0. in 7,5 x == 1, but that's not good for positioning, since the
				// AABBs would be inside each other. They need to be offset by their whole length. This is why we multiply in the end.
				// So, 2,5 x == 0, 7,5 x == 1 * length, which means it will be 1 length farther away.
				centerXYZ.x = ((floor((centerXYZ.x - ((float)sideLength / 2.0f)) / sideLength)) * sideLength) + sideLength;
				centerXYZ.y = ((floor((centerXYZ.y - ((float)sideLength / 2.0f)) / sideLength)) * sideLength) + sideLength;
				centerXYZ.z = ((floor((centerXYZ.z - ((float)sideLength / 2.0f)) / sideLength)) * sideLength) + sideLength;

				// Create AABB and set center and side length values
				cAABB* pAABB = new cAABB(centerXYZ, sideLength, "AABB.ply");
				//pAABB->setMeshOrientationEulerAngles(pAABB->GetMeshQuatOrientation(), glm::vec3(0.0f, 0.0f, 0.0f), true);
				/*pAABB->meshName = "AABB.ply";
				pAABB->position = centerXYZ;
				pAABB->nonUniformScale = glm::vec3(sideLength);
				pAABB->bIsWireframe = true;
				pAABB->bIsVisible = false;*/

				// The ID's should be the same. Otherwise, we have a bug
				unsigned long long the_AABB_ID = pAABB->GetID();
				if (AABB_ID != the_AABB_ID)
				{
					std::cerr << "AABB ID's didn't match!!" << std::endl;
				}
				// Set AABB on the map
				this->_pMapAABB[the_AABB_ID] = pAABB;

				// Then set the iterator to the AABB, by running find again and add the triangle to it
				itAABB = this->_pMapAABB.find(the_AABB_ID);
				itAABB->second->_pVecAABBTriangles.push_back(AABBTriangle);
				newTempAABB = itAABB->second;
			}
			else
			{
				if (tempAABB == nullptr)
				{
					// The vertex was on an already existing AABB, but have we already set the
					// triangle on it? We won't if the control variable is still null.
					// The control variable will exist until the vertex loop lasts
					tempAABB = itAABB->second;
					itAABB->second->_pVecAABBTriangles.push_back(AABBTriangle);
				}
				else if (itAABB->second != tempAABB && itAABB->second != newTempAABB)
				{
					// This AABB stil hasn't received this triangle
					itAABB->second->_pVecAABBTriangles.push_back(AABBTriangle);
				}
			}
			// Here, the map has finished checking for the AABB and either triggered the end above and created a new one,
			// or we already had an AABB at that point. We add the new triangle to whatever AABB this is in, BUT we can't repeat!
			//itAABB->second->_pVecAABBTriangles.push_back(AABBTriangle);
		}
	}

	return;
}

//void cAABBManager::DrawAllAABBs(GLuint program)
//{
//	for (std::map<unsigned long long, cAABB*>::iterator mapIter = _pMapAABB.begin(); mapIter != _pMapAABB.end(); mapIter++)
//	{
//		if (mapIter->second->bShouldDraw || cSceneUtils::GetInstance()->bShowAllBoundingBoxes)
//		{
//			mapIter->second->bShouldDraw = true;
//			DrawCall(program, mapIter->second);
//		}
//	}
//}

// Returns an AABB by it's ID
cAABB* cAABBManager::GetAABBByID(unsigned long long ID)
{
	cAABB* returnAABB = nullptr;
	for (std::map<unsigned long long, cAABB*>::iterator mapIter = _pMapAABB.begin(); mapIter != _pMapAABB.end(); mapIter++)
	{
		if (mapIter->first == ID)
		{
			// Found it
			mapIter->second->bShouldDraw = true;
			printf("Found AABB\n");
			//std::cout << "Position: " << mapIter->second->position.x << ", " << mapIter->second->position.y << ", " << mapIter->second->position.z << std::endl;
			returnAABB = mapIter->second;
		}
		else
		{
			mapIter->second->bShouldDraw = false;
		}
	}

	// Result will be the AABB or a nullptr
	return returnAABB;
}