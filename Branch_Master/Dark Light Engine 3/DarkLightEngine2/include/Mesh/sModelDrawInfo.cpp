// sModelDrawInfo.cpp

#include <Mesh\sModelDrawInfo.h>

sModel_xyz_rgb_n_uv_bones::sModel_xyz_rgb_n_uv_bones()
{
	// Initializing values
	this->x = 0.0f; this->y = 0.0f; this->z = 0.0f;
	this->r = 1.0f; this->g = 0.0f; this->b = 0.0f;
	this->nx = 0.0f; this->ny = 0.0f; this->nz = 0.0f;
	this->u0 = 0.0f; this->v0 = 0.0f; this->u1 = 0.0f; this->v1 = 0.0f;
	this->tx = 0.0f; this->ty = 0.0f; this->tz = 0.0f;
	this->bx = 0.0f; this->by = 0.0f; this->bz = 0.0f;

	// Bones
	memset(this->boneIDs, 0, sizeof(unsigned int) * NUMBEROFBONES);
	memset(this->boneWeights, 0, sizeof(unsigned int) * NUMBEROFBONES);
}

sModelDrawInfo::sModelDrawInfo()
{
	// Initializing values

	// Dynamic array pointers
	this->pVerticesToVBO = nullptr;
	this->pIndicesToVBO = nullptr;
	this->heightMap = nullptr;

	// Pointers to other infos
	this->originalMeshInfo = nullptr;
	this->skinnedModelInfo = nullptr;

	// Vertex buffer
	this->vertexBufferID = 0;
	this->numberOfVertices = 0;

	// Index buffer
	this->indexBufferID = 0;
	this->numberOfIndices = 0;
	this->numberOfTriangles = 0;

	// Common values
	this->meshFileName = "";
	this->friendlyName = "";
	this->meshIndex = 0;
	this->vaoID = 0;

	// Extents
	this->maxX = this->maxY = this->maxZ = 0.0f;
	this->minX = this->minY = this->minZ = 0.0f;
	this->extentX = this->extentY = this->extentZ = 0.0f;
	this->maxExtent = 0.0f;
	this->scaleForUnitBox = 1.0f;

	return;
}

sModelDrawInfo::~sModelDrawInfo()
{
	if (this->pVerticesToVBO != nullptr)
	{
		delete[] this->pVerticesToVBO;
		this->pVerticesToVBO = nullptr;
	}
	if (this->pIndicesToVBO != nullptr)
	{
		delete[] this->pIndicesToVBO;
		this->pIndicesToVBO = nullptr;
	}
	if (this->heightMap != nullptr)
	{
		delete[] this->heightMap;
		this->heightMap = nullptr;
	}

	if (this->originalMeshInfo != nullptr)
	{
		if (this->originalMeshInfo->pTrianglesFromFile != nullptr)
		{
			delete[] this->originalMeshInfo->pTrianglesFromFile;
			this->originalMeshInfo->pTrianglesFromFile = nullptr;
		}
		if (this->originalMeshInfo->pVerticesFromFile != nullptr)
		{
			delete[] this->originalMeshInfo->pVerticesFromFile;
			this->originalMeshInfo->pVerticesFromFile = nullptr;
		}
		delete this->originalMeshInfo;
		this->originalMeshInfo = nullptr;
	}

	if (this->skinnedModelInfo != nullptr)
	{
		delete this->skinnedModelInfo;
		this->skinnedModelInfo = nullptr;
	}
}

sSkinnedModelInfo::sSkinnedModelInfo()
{
	// Initializing
	this->pScene = nullptr;
	this->numBones = 0;
}

void sModelDrawInfo::CalculateExtents()
{
	// Calculating extents, the farthest points of the mesh
	// Begin assuming that all are 0
	this->minX = this->pVerticesToVBO[0].x;
	this->minY = this->pVerticesToVBO[0].y;
	this->minZ = this->pVerticesToVBO[0].z;
	this->maxX = this->pVerticesToVBO[0].x;
	this->maxY = this->pVerticesToVBO[0].y;
	this->maxZ = this->pVerticesToVBO[0].z;

	for (unsigned int extIndex = 0; extIndex != this->numberOfVertices; extIndex++)
	{
		if (this->pVerticesToVBO[extIndex].x < this->minX) { this->minX = this->pVerticesToVBO[extIndex].x; }
		if (this->pVerticesToVBO[extIndex].y < this->minY) { this->minY = this->pVerticesToVBO[extIndex].y; }
		if (this->pVerticesToVBO[extIndex].z < this->minZ) { this->minZ = this->pVerticesToVBO[extIndex].z; }

		if (this->pVerticesToVBO[extIndex].x > this->maxX) { this->maxX = this->pVerticesToVBO[extIndex].x; }
		if (this->pVerticesToVBO[extIndex].y > this->maxY) { this->maxY = this->pVerticesToVBO[extIndex].y; }
		if (this->pVerticesToVBO[extIndex].z > this->maxZ) { this->maxZ = this->pVerticesToVBO[extIndex].z; }
	}

	this->extentX = this->maxX - this->minX;
	this->extentY = this->maxY - this->minY;
	this->extentZ = this->maxZ - this->minZ;

	this->maxExtent = this->extentX;
	if (this->maxExtent < this->extentY) { this->maxExtent = this->extentY; }
	if (this->maxExtent < this->extentZ) { this->maxExtent = this->extentZ; }

	this->scaleForUnitBox = 1.0f / this->maxExtent;
}

void sModelDrawInfo::CalculateNormals()
{
	//// Go through each triangle...
	//// ...calculate normal per vertex (3 vertices)
	//// ...ADD this normal to the corresponding vertex

	//// 2nd pass, 
	//// go through all the vertices and "normalize" them
	//for (int vertIndex = 0; vertIndex != this->numberOfVertices; vertIndex++)
	//{
	//	this->originalMeshInfo->pVerticesFromFile[vertIndex].nx = 0.0f;
	//	this->originalMeshInfo->pVerticesFromFile[vertIndex].ny = 0.0f;
	//	this->originalMeshInfo->pVerticesFromFile[vertIndex].nz = 0.0f;
	//}

	//for (int triIndex = 0; triIndex != this->numberOfTriangles; triIndex++)
	//{
	//	sMeshTriangle curTri = this->pTrianglesFromFile[triIndex];

	//	// Calculate normal for each vertex
	//	glm::vec3 vertA = glm::vec3(this->pVerticesFromFile[curTri.vertex1].x,
	//		this->pVerticesFromFile[curTri.vertex1].y,
	//		this->pVerticesFromFile[curTri.vertex1].z);

	//	glm::vec3 vertB = glm::vec3(this->pVerticesFromFile[curTri.vertex2].x,
	//		this->pVerticesFromFile[curTri.vertex2].y,
	//		this->pVerticesFromFile[curTri.vertex2].z);

	//	glm::vec3 vertC = glm::vec3(this->pVerticesFromFile[curTri.vertex3].x,
	//		this->pVerticesFromFile[curTri.vertex3].y,
	//		this->pVerticesFromFile[curTri.vertex3].z);

	//	// Cross of A-B and A-C (normal at vertex A)
	//	glm::vec3 normVec0 = glm::normalize(glm::cross(vertB - vertA, vertC - vertA));

	//	// Cross of B-A and B-C (normal at vertex B)
	//	glm::vec3 normVec1 = glm::normalize(glm::cross(vertA - vertB, vertC - vertB));

	//	// Cross of C-A and C-B (normal at vertex C)
	//	glm::vec3 normVec2 = glm::normalize(glm::cross(vertA - vertC, vertB - vertC));

	//	// Add the values to the current normals (vert A)
	//	this->pVerticesFromFile[curTri.vertex1].nx += normVec0.x;
	//	this->pVerticesFromFile[curTri.vertex1].ny += normVec0.y;
	//	this->pVerticesFromFile[curTri.vertex1].nz += normVec0.z;

	//	// Add the values to the current normals (vert B)
	//	this->pVerticesFromFile[curTri.vertex2].nx += normVec1.x;
	//	this->pVerticesFromFile[curTri.vertex2].ny += normVec1.y;
	//	this->pVerticesFromFile[curTri.vertex2].nz += normVec1.z;

	//	// Add the values to the current normals (vert C)
	//	this->pVerticesFromFile[curTri.vertex3].nx += normVec2.x;
	//	this->pVerticesFromFile[curTri.vertex3].ny += normVec2.y;
	//	this->pVerticesFromFile[curTri.vertex3].nz += normVec2.z;
	//}//

	//// 2nd pass: normalize the normals
	//for (int vertIndex = 0; vertIndex != this->oNumberOfVertices; vertIndex++)
	//{
	//	glm::vec3 norm = glm::vec3(this->pVerticesFromFile[vertIndex].nx,
	//		this->pVerticesFromFile[vertIndex].ny,
	//		this->pVerticesFromFile[vertIndex].nz);
	//	// It's value DIV length
	//	glm::normalize(norm);
	//	// divide the xyz by the length of the vector

	//	this->pVerticesFromFile[vertIndex].nx = norm.x;
	//	this->pVerticesFromFile[vertIndex].ny = norm.y;
	//	this->pVerticesFromFile[vertIndex].nz = norm.z;
	//}
}

void sVertexBoneData::AddBoneData(unsigned int BoneID, float Weight)
{
	for (unsigned int Index = 0; Index < sizeof(this->ids) / sizeof(this->ids[0]); Index++)
	{
		if (this->weights[Index] == 0.0f)
		{
			this->ids[Index] = (float)BoneID;
			this->weights[Index] = Weight;
			return;
		}
	}
}

glm::mat4x4 sSkinnedModelInfo::GetFinalBoneByName(std::string boneName)
{
	std::map<std::string/*Bone name*/, unsigned int /*index*/>::iterator mapIter;
	mapIter = this->mapBoneNameToBoneIndex.find(boneName);

	glm::mat4x4 mat = glm::mat4x4(1.0f);
	if (mapIter != this->mapBoneNameToBoneIndex.end())
	{
		// Found it
		return this->vecFinalTransformation[mapIter->second];
	}

	return mat;
}

glm::mat4x4 sSkinnedModelInfo::GetBoneByName(std::string boneName)
{
	std::map<std::string/*Bone name*/, unsigned int /*index*/>::iterator mapIter;
	mapIter = this->mapBoneNameToBoneIndex.find(boneName);

	glm::mat4x4 mat = glm::mat4x4(1.0f);
	if (mapIter != this->mapBoneNameToBoneIndex.end())
	{
		// Found it
		return this->vecBoneGlobals[mapIter->second];
	}

	return mat;
}

// Added to assist with the forward kinematics (id and bone names)
void sSkinnedModelInfo::GetListOfBoneIDandNames(std::vector<std::string> &vecBoneNames)
{
	// Go through the list of channels and return the id and name
	for (unsigned int chanID = 0; chanID != this->pScene->mAnimations[0]->mNumChannels; chanID++)
	{
		std::string boneName;
		boneName.append(this->pScene->mAnimations[0]->mChannels[chanID]->mNodeName.data);
		vecBoneNames.push_back(boneName);
	}
	return;
}
