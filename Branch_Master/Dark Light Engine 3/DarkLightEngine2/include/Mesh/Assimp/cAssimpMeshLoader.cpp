// cAssimpMeshLoader.cpp

#include <Mesh\Assimp\cAssimpMeshLoader.h>
#include <Animation\Assimp\postprocess.h>
#include <Animation\Assimp\Importer.hpp>
#include <Animation\nAssimpHelper.h>


cAssimpMeshLoader::cAssimpMeshLoader()
{
	this->_baseModelPath = "assets/models/skinnedModels/";
	this->_baseAnimationPath = "assets/animations/";

	return;
}

cAssimpMeshLoader::~cAssimpMeshLoader()
{
	this->_Cleanup();

	return;
}

glm::mat4 cAssimpMeshLoader::AIMatrixToGLMMatrix(const aiMatrix4x4 &mat)
{
	return glm::mat4(mat.a1, mat.b1, mat.c1, mat.d1,
		mat.a2, mat.b2, mat.c2, mat.d2,
		mat.a3, mat.b3, mat.c3, mat.d3,
		mat.a4, mat.b4, mat.c4, mat.d4);
}


bool cAssimpMeshLoader::SetBasePath(std::string basePath, std::string& pathToSet)
{
	struct stat info;
	if (stat(basePath.c_str(), &info) != 0)
	{
		printf("Path %s couldn't be accessed!\n", basePath.c_str());
		return false;
	}
	else if (info.st_mode& S_IFDIR)
	{
		//printf("Model path set.\n");
		pathToSet = basePath;
		return true;
	}
}

bool cAssimpMeshLoader::LoadMeshFromFile(const std::string& filename, sModelDrawInfo& meshInfo)
{
	unsigned int Flags = aiProcess_Triangulate | aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph |
						 aiProcess_JoinIdenticalVertices | aiProcess_GenNormals | aiProcess_CalcTangentSpace;

	// Create original mesh info and skinned mesh info
	sOriginalMeshInfo* originalMesh = new sOriginalMeshInfo();
	sSkinnedModelInfo* skinnedMesh = new sSkinnedModelInfo();

	std::string fullPathName = this->_baseModelPath;
	fullPathName.append(filename);
	skinnedMesh->pScene = skinnedMesh->importer.ReadFile(fullPathName.c_str(), Flags);

	//aiMesh* pM0 = this->pScene->mMeshes[0];
	////aiMesh* pM1 = this->mpScene->mMeshes[1]; // if we had more than 1 mesh on the scene...
	////aiMesh* pM2 = this->mpScene->mMeshes[2];
	////aiMesh* pM3 = this->mpScene->mMeshes[3];
	if (skinnedMesh->pScene)
	{
		// Set names
		meshInfo.meshFileName = filename;

		// Assume the friendlyName is the same as the file, for now
		meshInfo.friendlyName = filename;

		skinnedMesh->mGlobalInverseTransformation = AIMatrixToGLMMatrix(skinnedMesh->pScene->mRootNode->mTransformation);
		skinnedMesh->mGlobalInverseTransformation = glm::inverse(skinnedMesh->mGlobalInverseTransformation);

		// Calcualte all the bone things
		if (!this->_InitializeBones(skinnedMesh->pScene, meshInfo, skinnedMesh))
		{	// There was an issue doing this calculation
			printf("Error: %s\n", skinnedMesh->importer.GetErrorString());
			return false;
		}
	}

	meshInfo.originalMeshInfo = originalMesh;
	meshInfo.skinnedModelInfo = skinnedMesh;

	return true;
}

bool cAssimpMeshLoader::_InitializeBones(const aiScene* pScene, sModelDrawInfo& meshInfo, sSkinnedModelInfo* skinnedMesh)
{
	meshInfo.numberOfVertices = skinnedMesh->pScene->mMeshes[0]->mNumVertices;

	// This is the vertex information for JUST the bone stuff
	skinnedMesh->vecVertexBoneData.resize(meshInfo.numberOfVertices);

	this->LoadBones(pScene->mMeshes[0], skinnedMesh);

	return true;
}

void cAssimpMeshLoader::_Cleanup()
{
	// Release all scenes created
	std::map<std::string, sAnimationInfo>::iterator mapIter = this->mapAnimationsByName.begin();
	for (mapIter; mapIter != this->mapAnimationsByName.end(); mapIter++)
	{
		// Deleting the assimp importer will clean all data created by it.
		delete mapIter->second.pImporter;
	}
}

bool cAssimpMeshLoader::LoadBones(const aiMesh* mesh, sSkinnedModelInfo* skinnedMesh)
{
	for (unsigned int boneIndex = 0; boneIndex != mesh->mNumBones; boneIndex++)
	{
		unsigned int BoneIndex = 0;
		std::string BoneName(mesh->mBones[boneIndex]->mName.data);

		//	std::map<std::string /*BoneName*/, unsigned int /*BoneIndex*/> mMapping;
		// 	std::vector<sBoneInfo> mInfo;

		std::map<std::string, unsigned int>::iterator it = skinnedMesh->mapBoneNameToBoneIndex.find(BoneName);
		if (it == skinnedMesh->mapBoneNameToBoneIndex.end())
		{
			BoneIndex = skinnedMesh->numBones;
			skinnedMesh->numBones++;
			sBoneInfo bi;
			skinnedMesh->vecBoneInfo.push_back(bi);

			skinnedMesh->vecBoneInfo[BoneIndex].BoneOffset = nAssimpHelper::AIMatrixToGLMMatrix(mesh->mBones[boneIndex]->mOffsetMatrix);
			skinnedMesh->mapBoneNameToBoneIndex[BoneName] = BoneIndex;
		}
		else
		{
			BoneIndex = it->second;
		}

		for (unsigned int WeightIndex = 0; WeightIndex != mesh->mBones[boneIndex]->mNumWeights; WeightIndex++)
		{
			unsigned int VertexID = /*mMeshEntries[MeshIndex].BaseVertex +*/ mesh->mBones[boneIndex]->mWeights[WeightIndex].mVertexId;
			float Weight = mesh->mBones[boneIndex]->mWeights[WeightIndex].mWeight;
			skinnedMesh->vecVertexBoneData[VertexID].AddBoneData(BoneIndex, Weight);
		}
	}

	return true;
}

bool cAssimpMeshLoader::LoadMeshAnimation(const std::string& filename, const std::string& friendlyName)
{
	// Check if animation is already there
	std::map<std::string/*Friendly name*/, sAnimationInfo/*Animation*/>::iterator mapIter = this->mapAnimationsByName.find(friendlyName);
	if (mapIter != this->mapAnimationsByName.end())
	{
		// Found it
		return false;
	}

	unsigned int Flags = aiProcess_Triangulate | aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph | aiProcess_JoinIdenticalVertices;

	// Set anim info
	Assimp::Importer* pImporter = new Assimp::Importer();
	sAnimationInfo animInfo;
	animInfo.friendlyName = friendlyName;
	animInfo.fileName = filename;

	// Load animation
	std::string fullPathName = this->_baseAnimationPath;
	fullPathName.append(filename);
	animInfo.pImporter = pImporter;
	animInfo.pAIScene = pImporter->ReadFile(fullPathName.c_str(), Flags);

	if (!animInfo.pAIScene)
	{
		return false;
	}

	// Animation scene is loaded (we assume)
	// Add it to the map
	this->mapAnimationsByName[animInfo.friendlyName] = animInfo;

	return true;
}

bool cAssimpMeshLoader::LoadAssimpDrawInfo(sModelDrawInfo& meshInfo, unsigned int modelIndexInScene)
{
	sOriginalMeshInfo* originalMesh = meshInfo.originalMeshInfo;
	sSkinnedModelInfo* skinnedMesh = meshInfo.skinnedModelInfo;

	// Each aiScene has indexes for each model inside the scene.
	// It could have multiple, so send the index of the one we want
	// and if there are less meshes then what we asked, it's not there.
	if (skinnedMesh->pScene->mNumMeshes < modelIndexInScene)
	{
		return false;
	}

	aiMesh* pAIMesh = skinnedMesh->pScene->mMeshes[modelIndexInScene];

	meshInfo.numberOfVertices = pAIMesh->mNumVertices;

	meshInfo.pVerticesToVBO = new sModel_xyz_rgb_n_uv_bones[meshInfo.numberOfVertices];

	for (int vertIndex = 0; vertIndex != meshInfo.numberOfVertices; vertIndex++)
	{
		sModel_xyz_rgb_n_uv_bones pCurVert = meshInfo.pVerticesToVBO[vertIndex];

		aiVector3D* pAIVert = &(pAIMesh->mVertices[vertIndex]);

		pCurVert.x = pAIVert->x;
		pCurVert.y = pAIVert->y;
		pCurVert.z = pAIVert->z;

		// Colours
		if (pAIMesh->GetNumColorChannels() > 0)
		{
			pCurVert.r = skinnedMesh->pScene->mMeshes[0]->mColors[vertIndex]->r;
			pCurVert.g = skinnedMesh->pScene->mMeshes[0]->mColors[vertIndex]->g;
			pCurVert.b = skinnedMesh->pScene->mMeshes[0]->mColors[vertIndex]->b;
			pCurVert.a = skinnedMesh->pScene->mMeshes[0]->mColors[vertIndex]->a;
		}
		else
		{
			pCurVert.r = pCurVert.g = pCurVert.b = pCurVert.a = 1.0f;
		}

		// Normals
		if (pAIMesh->HasNormals())
		{
			pCurVert.nx = pAIMesh->mNormals[vertIndex].x;
			pCurVert.ny = pAIMesh->mNormals[vertIndex].y;
			pCurVert.nz = pAIMesh->mNormals[vertIndex].z;
		}

		// UVs
		if (pAIMesh->GetNumUVChannels() > 0)
		{	// Assume 1st channel is the 2D UV coordinates
			pCurVert.u0 = pAIMesh->mTextureCoords[0][vertIndex].x;
			pCurVert.v0 = pAIMesh->mTextureCoords[0][vertIndex].y;
			if (pAIMesh->GetNumUVChannels() > 1)
			{
				pCurVert.u1 = pAIMesh->mTextureCoords[1][vertIndex].x;
				pCurVert.v1 = pAIMesh->mTextureCoords[1][vertIndex].y;
			}
		}

		// Tangents and Bitangents (bi-normals)
		if (pAIMesh->HasTangentsAndBitangents())
		{
			pCurVert.tx = pAIMesh->mTangents[vertIndex].x;
			pCurVert.ty = pAIMesh->mTangents[vertIndex].y;
			pCurVert.tz = pAIMesh->mTangents[vertIndex].z;

			pCurVert.bx = pAIMesh->mBitangents[vertIndex].x;
			pCurVert.by = pAIMesh->mBitangents[vertIndex].y;
			pCurVert.bz = pAIMesh->mBitangents[vertIndex].z;
		}

		// Bone IDs are being passed OK
		pCurVert.boneIDs[0] = skinnedMesh->vecVertexBoneData[vertIndex].ids[0];
		pCurVert.boneIDs[1] = skinnedMesh->vecVertexBoneData[vertIndex].ids[1];
		pCurVert.boneIDs[2] = skinnedMesh->vecVertexBoneData[vertIndex].ids[2];
		pCurVert.boneIDs[3] = skinnedMesh->vecVertexBoneData[vertIndex].ids[3];

		// Weights are being passed OK
		pCurVert.boneWeights[0] = skinnedMesh->vecVertexBoneData[vertIndex].weights[0];
		pCurVert.boneWeights[1] = skinnedMesh->vecVertexBoneData[vertIndex].weights[1];
		pCurVert.boneWeights[2] = skinnedMesh->vecVertexBoneData[vertIndex].weights[2];
		pCurVert.boneWeights[3] = skinnedMesh->vecVertexBoneData[vertIndex].weights[3];


	}//for ( int vertIndex

	// Triangles
	meshInfo.numberOfTriangles = pAIMesh->mNumFaces;

	originalMesh->pTrianglesFromFile = new sMeshTriangle[meshInfo.numberOfTriangles];

	for (unsigned int triIndex = 0; triIndex != meshInfo.numberOfTriangles; triIndex++)
	{
		aiFace* pAIFace = &(pAIMesh->mFaces[triIndex]);

		originalMesh->pTrianglesFromFile[triIndex].vertex1 = pAIFace->mIndices[0];
		originalMesh->pTrianglesFromFile[triIndex].vertex2 = pAIFace->mIndices[1];
		originalMesh->pTrianglesFromFile[triIndex].vertex3 = pAIFace->mIndices[2];

	}//for ( unsigned int triIndex...

	//meshInfo->name = this->friendlyName;

	meshInfo.CalculateExtents();

	return true;
}

//bool cAssimpMeshLoader::LoadMeshAnimation(const std::string& filename, const std::string& friendlyName, sModelDrawInfo& meshInfo)
//{
//	// Check if animation is already there
//	std::map<std::string/*Friendly name*/, sAnimationInfo/*Animation*/>::iterator mapIter = meshInfo.mapAnimationsByName.find(friendlyName);
//	if (mapIter != meshInfo.mapAnimationsByName.end())
//	{
//		// Found it
//		return false;
//	}
//
//	unsigned int Flags = aiProcess_Triangulate | aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph | aiProcess_JoinIdenticalVertices;
//
//	// Set anim info
//	Assimp::Importer* pImporter = new Assimp::Importer();
//	sAnimationInfo animInfo;
//	animInfo.friendlyName = friendlyName;
//	animInfo.fileName = filename;
//
//	// Load animation
//	std::string fullPathName = this->_baseAnimationPath;
//	fullPathName.append(filename);
//	animInfo.pAIScene = pImporter->ReadFile(fullPathName.c_str(), Flags);
//
//	if (!animInfo.pAIScene)
//	{
//		return false;
//	}
//
//	// Animation scene is loaded (we assume)
//	// Add it to the map
//	meshInfo.mapAnimationsByName[animInfo.friendlyName] = animInfo;
//
//	return true;
//}
//
//bool cAssimpMeshLoader::LoadAssimpDrawInfo(sModelDrawInfo& meshInfo, unsigned int modelIndexInScene)
//{
//	// Each aiScene has indexes for each model inside the scene.
//	// It could have multiple, so send the index of the one we want
//	// and if there are less meshes then what we asked, it's not there.
//	if (meshInfo.pScene->mNumMeshes < modelIndexInScene)
//	{	
//		return false;
//	}
//
//	aiMesh* pAIMesh = meshInfo.pScene->mMeshes[modelIndexInScene];
//
//	meshInfo.numberOfVertices = pAIMesh->mNumVertices;
//
//	meshInfo.pVerticesToVBO = new sModel_xyz_rgb_n_uv_bones[meshInfo.numberOfVertices];
//
//	for (int vertIndex = 0; vertIndex != meshInfo.numberOfVertices; vertIndex++)
//	{
//		sModel_xyz_rgb_n_uv_bones pCurVert = meshInfo.pVerticesToVBO[vertIndex];
//
//		aiVector3D* pAIVert = &(pAIMesh->mVertices[vertIndex]);
//
//		pCurVert.x = pAIVert->x;
//		pCurVert.y = pAIVert->y;
//		pCurVert.z = pAIVert->z;
//
//		// Colours
//		if (pAIMesh->GetNumColorChannels() > 0)
//		{
//			pCurVert.r = meshInfo.pScene->mMeshes[0]->mColors[vertIndex]->r;
//			pCurVert.g = meshInfo.pScene->mMeshes[0]->mColors[vertIndex]->g;
//			pCurVert.b = meshInfo.pScene->mMeshes[0]->mColors[vertIndex]->b;
//			pCurVert.a = meshInfo.pScene->mMeshes[0]->mColors[vertIndex]->a;
//		}
//		else
//		{
//			pCurVert.r = pCurVert.g = pCurVert.b = pCurVert.a = 1.0f;
//		}
//
//		// Normals
//		if (pAIMesh->HasNormals())
//		{
//			pCurVert.nx = pAIMesh->mNormals[vertIndex].x;
//			pCurVert.ny = pAIMesh->mNormals[vertIndex].y;
//			pCurVert.nz = pAIMesh->mNormals[vertIndex].z;
//		}
//
//		// UVs
//		if (pAIMesh->GetNumUVChannels() > 0)
//		{	// Assume 1st channel is the 2D UV coordinates
//			pCurVert.u0 = pAIMesh->mTextureCoords[0][vertIndex].x;
//			pCurVert.v0 = pAIMesh->mTextureCoords[0][vertIndex].y;
//			if (pAIMesh->GetNumUVChannels() > 1)
//			{
//				pCurVert.u1 = pAIMesh->mTextureCoords[1][vertIndex].x;
//				pCurVert.v1 = pAIMesh->mTextureCoords[1][vertIndex].y;
//			}
//		}
//
//		// Tangents and Bitangents (bi-normals)
//		if (pAIMesh->HasTangentsAndBitangents())
//		{
//			pCurVert.tx = pAIMesh->mTangents[vertIndex].x;
//			pCurVert.ty = pAIMesh->mTangents[vertIndex].y;
//			pCurVert.tz = pAIMesh->mTangents[vertIndex].z;
//
//			pCurVert.bx = pAIMesh->mBitangents[vertIndex].x;
//			pCurVert.by = pAIMesh->mBitangents[vertIndex].y;
//			pCurVert.bz = pAIMesh->mBitangents[vertIndex].z;
//		}
//
//		// Bone IDs are being passed OK
//		pCurVert.boneIDs[0] = meshInfo.vecVertexBoneData[vertIndex].ids[0];
//		pCurVert.boneIDs[1] = meshInfo.vecVertexBoneData[vertIndex].ids[1];
//		pCurVert.boneIDs[2] = meshInfo.vecVertexBoneData[vertIndex].ids[2];
//		pCurVert.boneIDs[3] = meshInfo.vecVertexBoneData[vertIndex].ids[3];
//
//		// Weights are being passed OK
//		pCurVert.boneWeights[0] = meshInfo.vecVertexBoneData[vertIndex].weights[0];
//		pCurVert.boneWeights[1] = meshInfo.vecVertexBoneData[vertIndex].weights[1];
//		pCurVert.boneWeights[2] = meshInfo.vecVertexBoneData[vertIndex].weights[2];
//		pCurVert.boneWeights[3] = meshInfo.vecVertexBoneData[vertIndex].weights[3];
//
//
//	}//for ( int vertIndex
//
//	// Triangles
//	meshInfo.numberOfTriangles = pAIMesh->mNumFaces;
//
//	meshInfo.pTrianglesFromFile = new sModelTriangle[meshInfo.numberOfTriangles];
//
//	for (unsigned int triIndex = 0; triIndex != meshInfo.numberOfTriangles; triIndex++)
//	{
//		aiFace* pAIFace = &(pAIMesh->mFaces[triIndex]);
//
//		meshInfo.pTrianglesFromFile[triIndex].vertex1 = pAIFace->mIndices[0];
//		meshInfo.pTrianglesFromFile[triIndex].vertex2 = pAIFace->mIndices[1];
//		meshInfo.pTrianglesFromFile[triIndex].vertex3 = pAIFace->mIndices[2];
//
//	}//for ( unsigned int triIndex...
//
//	//meshInfo->name = this->friendlyName;
//
//	meshInfo.CalculateExtents();
//
//	return true;
//}

bool cAssimpMeshLoader::ConvertAssimpToVAO(cVaoMeshManager* pVaoMeshManager, sModelDrawInfo& newDrawInfo, GLuint shaderProgramID)
{
	//sSkinnedModelInfo* skinnedMesh = newDrawInfo.skinnedModelInfo;

	////newDrawInfo.meshFileName = meshInfo.meshFileName;
	////newDrawInfo.friendlyName = meshInfo.friendlyName;

	//// Copy the data from assimp format into the sModelDrawInfo format...

	//newDrawInfo.numberOfVertices = skinnedMesh->pScene->mMeshes[0]->mNumVertices;
	//newDrawInfo.numberOfTriangles = skinnedMesh->pScene->mMeshes[0]->mNumFaces;

	//// We used the "triangulate" option when loading so all the primitives
	////	will be triangles, but BEWARE!
	//newDrawInfo.numberOfIndices = newDrawInfo.numberOfTriangles * 3;


	//// Allocate the vertex array (it's a c-style array)
	////newDrawInfo.originalMeshInfo = meshInfo.originalMeshInfo;

	//newDrawInfo.pVerticesToVBO = new sModel_xyz_rgb_n_uv_bones[newDrawInfo.numberOfVertices];

	//// Danger Will Robinson! 
	//// You don't really need to do this, but at least it will clear it to zero.
	//// (compiler will complain that it's 'not safe', etc.)
	//memset(newDrawInfo.pVerticesToVBO, 0, sizeof(sModel_xyz_rgb_n_uv_bones) * newDrawInfo.numberOfVertices);


	//for (unsigned int vertIndex = 0; vertIndex != newDrawInfo.numberOfVertices; vertIndex++)
	//{
	//	newDrawInfo.pVerticesToVBO[vertIndex].x = skinnedMesh->pScene->mMeshes[0]->mVertices[vertIndex].x;
	//	newDrawInfo.pVerticesToVBO[vertIndex].y = skinnedMesh->pScene->mMeshes[0]->mVertices[vertIndex].y;
	//	newDrawInfo.pVerticesToVBO[vertIndex].z = skinnedMesh->pScene->mMeshes[0]->mVertices[vertIndex].z;

	//	// Normals... 
	//	newDrawInfo.pVerticesToVBO[vertIndex].nx = skinnedMesh->pScene->mMeshes[0]->mNormals[vertIndex].x;
	//	newDrawInfo.pVerticesToVBO[vertIndex].ny = skinnedMesh->pScene->mMeshes[0]->mNormals[vertIndex].y;
	//	newDrawInfo.pVerticesToVBO[vertIndex].nz = skinnedMesh->pScene->mMeshes[0]->mNormals[vertIndex].z;

	//	// Colours... 
	//	// (If there are no colours, make it hit pink)
	//	// Note: the method is because you could have more than one set of 
	//	//	vertex colours in the model (Why? Who the heck knows?) 
	//	if (skinnedMesh->pScene->mMeshes[0]->HasVertexColors(0))
	//	{
	//		newDrawInfo.pVerticesToVBO[vertIndex].r = skinnedMesh->pScene->mMeshes[0]->mColors[vertIndex]->r;
	//		newDrawInfo.pVerticesToVBO[vertIndex].g = skinnedMesh->pScene->mMeshes[0]->mColors[vertIndex]->g;
	//		newDrawInfo.pVerticesToVBO[vertIndex].b = skinnedMesh->pScene->mMeshes[0]->mColors[vertIndex]->b;
	//		newDrawInfo.pVerticesToVBO[vertIndex].a = skinnedMesh->pScene->mMeshes[0]->mColors[vertIndex]->a;
	//	}
	//	else
	//	{	// hotpink	#FF69B4	rgb(255,105,180)
	//		newDrawInfo.pVerticesToVBO[vertIndex].r = 1.0f;
	//		newDrawInfo.pVerticesToVBO[vertIndex].g = 105.0f / 255.0f;
	//		newDrawInfo.pVerticesToVBO[vertIndex].b = 180.0f / 255.0f;
	//		newDrawInfo.pVerticesToVBO[vertIndex].a = 1.0f;
	//	}

	//	//bi-normal  (or bi-tangent)
	//	newDrawInfo.pVerticesToVBO[vertIndex].bx = skinnedMesh->pScene->mMeshes[0]->mBitangents[vertIndex].x;
	//	newDrawInfo.pVerticesToVBO[vertIndex].by = skinnedMesh->pScene->mMeshes[0]->mBitangents[vertIndex].y;
	//	newDrawInfo.pVerticesToVBO[vertIndex].bz = skinnedMesh->pScene->mMeshes[0]->mBitangents[vertIndex].z;

	//	// Tangent
	//	newDrawInfo.pVerticesToVBO[vertIndex].tx = skinnedMesh->pScene->mMeshes[0]->mTangents[vertIndex].x;
	//	newDrawInfo.pVerticesToVBO[vertIndex].ty = skinnedMesh->pScene->mMeshes[0]->mTangents[vertIndex].y;
	//	newDrawInfo.pVerticesToVBO[vertIndex].tz = skinnedMesh->pScene->mMeshes[0]->mTangents[vertIndex].z;

	//	// uv2 (which are odd in assimp)
	//	// Note that there is an array of texture coordinates, 
	//	// up to 8 (actually). Usually, there's only 1
	//	//if (skinnedMesh->pScene->mMeshes[0]->HasTextureCoords(0))	// 1st UV coords
	//	//{
	//	//	// Assume there's 1... (at least)
	//	//	newDrawInfo.pVerticesToVBO[vertIndex].u0 = skinnedMesh->pScene->mMeshes[0]->mTextureCoords[0]->x;
	//	//	newDrawInfo.pVerticesToVBO[vertIndex].v0 = skinnedMesh->pScene->mMeshes[0]->mTextureCoords[0]->y;
	//	//}
	//	//if (skinnedMesh->pScene->mMeshes[0]->HasTextureCoords(1))	// 2nd UV coords
	//	//{
	//	//	newDrawInfo.pVerticesToVBO[vertIndex].u1 = skinnedMesh->pScene->mMeshes[0]->mTextureCoords[1]->x;
	//	//	newDrawInfo.pVerticesToVBO[vertIndex].v1 = skinnedMesh->pScene->mMeshes[0]->mTextureCoords[1]->y;
	//	//}
	//	// TODO: add additional texture coordinates (mTextureCoords[1], etc.)
	//	if (skinnedMesh->pScene->mMeshes[0]->mNumUVComponents[0] == 2)
	//	{
	//		newDrawInfo.pVerticesToVBO[vertIndex].u0 = skinnedMesh->pScene->mMeshes[0]->mTextureCoords[0][vertIndex].x;
	//		newDrawInfo.pVerticesToVBO[vertIndex].v0 = skinnedMesh->pScene->mMeshes[0]->mTextureCoords[0][vertIndex].y;

	//		//newDrawInfo.pVerticesToVBO[vertIndex].u1 = skinnedMesh->pScene->mMeshes[0]->mTextureCoords[1][vertIndex].x;
	//		//newDrawInfo.pVerticesToVBO[vertIndex].v1 = skinnedMesh->pScene->mMeshes[0]->mTextureCoords[1][vertIndex].y;
	//	}

	//	// 4Bones: ids and weights
	//	newDrawInfo.pVerticesToVBO[vertIndex].boneIDs[0] = skinnedMesh->vecVertexBoneData[vertIndex].ids[0];
	//	newDrawInfo.pVerticesToVBO[vertIndex].boneIDs[1] = skinnedMesh->vecVertexBoneData[vertIndex].ids[1];
	//	newDrawInfo.pVerticesToVBO[vertIndex].boneIDs[2] = skinnedMesh->vecVertexBoneData[vertIndex].ids[2];
	//	newDrawInfo.pVerticesToVBO[vertIndex].boneIDs[3] = skinnedMesh->vecVertexBoneData[vertIndex].ids[3];
	//	
	//	newDrawInfo.pVerticesToVBO[vertIndex].boneWeights[0] = skinnedMesh->vecVertexBoneData[vertIndex].weights[0];
	//	newDrawInfo.pVerticesToVBO[vertIndex].boneWeights[1] = skinnedMesh->vecVertexBoneData[vertIndex].weights[1];
	//	newDrawInfo.pVerticesToVBO[vertIndex].boneWeights[2] = skinnedMesh->vecVertexBoneData[vertIndex].weights[2];
	//	newDrawInfo.pVerticesToVBO[vertIndex].boneWeights[3] = skinnedMesh->vecVertexBoneData[vertIndex].weights[3];


	//}//for ( unsigned int vertIndex = 0;

	//// And the triangles

	//// Allocate the array to hold the indices (triangle) info


	//// Allocate the array for that (indices NOT triangles)
	//newDrawInfo.pIndicesToVBO = new unsigned int[newDrawInfo.numberOfIndices];

	//// Danger Will Robinson! 
	//// You don't really need to do this, but at least it will clear it to zero.
	//// (compiler will complain that it's 'not safe', etc.)
	//memset(newDrawInfo.pIndicesToVBO, 0, sizeof(unsigned int) * newDrawInfo.numberOfIndices);


	//unsigned int numTriangles = skinnedMesh->pScene->mMeshes[0]->mNumFaces;
	//unsigned int triIndex = 0;		// Steps through the triangles.
	//unsigned int indexIndex = 0;	// Setps through the indices (index buffer)
	//for (; triIndex != newDrawInfo.numberOfTriangles;
	//	triIndex++, indexIndex += 3)	// Note, every 1 triangle = 3 index steps
	//{
	//	// Get the triangle at this triangle index...
	//	aiFace* pAIFace = &(skinnedMesh->pScene->mMeshes[0]->mFaces[triIndex]);

	//	newDrawInfo.pIndicesToVBO[indexIndex + 0]				// Offset by 0 (zero)
	//		= pAIFace->mIndices[0];	// vertex 0

	//	newDrawInfo.pIndicesToVBO[indexIndex + 1]				// Offset by 1
	//		= pAIFace->mIndices[1];	// vertex 1

	//	newDrawInfo.pIndicesToVBO[indexIndex + 2]				// Offset by 2
	//		= pAIFace->mIndices[2];	// vertex 1
	//}//for ( ; triIndex != numVertices; 


	//// Calculate the extents on the mesh
	//// (Note, because I'm a bone head, this is dupicated...)
	//newDrawInfo.skinnedModelInfo = skinnedMesh;
	//newDrawInfo.CalculateExtents();

	//meshInfo = newDrawInfo;

	// ...then pass it to the mesh manager

	pVaoMeshManager->LoadModelIntoVAO(newDrawInfo, shaderProgramID);

	return true;
}

bool cAssimpMeshLoader::FindAnimationByName(const std::string& animationName, sAnimationInfo& animInfo)
{
	std::map<std::string, sAnimationInfo>::iterator mapIter = this->mapAnimationsByName.find(animationName);
	if (mapIter == this->mapAnimationsByName.end())
	{
		return false;
	}

	animInfo = mapIter->second;
	return true;
}


