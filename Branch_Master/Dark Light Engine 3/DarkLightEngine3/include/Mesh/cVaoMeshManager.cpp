// cVaoMeshManager.cpp

#include <Mesh\cVaoMeshManager.h>
#include <fstream>
#include <sstream>

cVaoMeshManager::cVaoMeshManager()
{
	this->_basePath = "assets/models/";
	this->_lastError = "";

	return;
}

cVaoMeshManager::~cVaoMeshManager()
{
	this->Shutdown();

	return;
}

bool cVaoMeshManager::SetBasePath(const std::string& basePath)
{
	struct stat info;
	if (stat(basePath.c_str(), &info) != 0)
	{
		printf("Path %s couldn't be accessed!\n", basePath.c_str());
		return false;
	}
	else if (info.st_mode& S_IFDIR)
	{
		//printf("Model path set.\n", basePath.c_str());
		this->_basePath = basePath;
		return true;
	}
}

bool cVaoMeshManager::LoadThreadModelIntoCPU(sModelDrawInfo& modelInfo)
{
	return this->_LoadModelFromFile(modelInfo);
}

bool cVaoMeshManager::LoadThreadModelIntoGPU(sModelDrawInfo& meshInfo, GLuint shaderProgramID)
{
	if (!this->_LoadDrawInfoIntoVAO(meshInfo, shaderProgramID))
	{
		this->_AppendTextToLastError("Model ", false);
		this->_AppendTextToLastError(meshInfo.meshFileName, false);
		this->_AppendTextToLastError(" wasn't loaded into the VAO!", true);
		return false;
	}

	// Add to the map and return
	this->_mapModelInfoByName[meshInfo.meshFileName] = &meshInfo;
	return true;
}

bool cVaoMeshManager::_LoadModelFromFile(sModelDrawInfo& modelInfo)
{
	// Open file
	std::string fullPathName = this->_basePath + modelInfo.meshFileName;
	std::ifstream meshFile(fullPathName.c_str());

	// Check if we could load the file
	if (!meshFile.is_open())
	{
		this->_AppendTextToLastError("Could not load ", false);
		this->_AppendTextToLastError(modelInfo.meshFileName, false);
		this->_AppendTextToLastError(" file!", true);
		return false;
	}

	// Look for the number of vertices on the file
	std::string nextData;
	while (meshFile >> nextData)
	{
		if (nextData == "vertex") // Number is right after this word
		{
			break;
		}
	}
	meshFile >> modelInfo.numberOfVertices; // Save the number of vertices

	// Look for the number of triangles
	while (meshFile >> nextData)
	{
		if (nextData == "face") // Number is right after this word
		{
			break;
		}
	}
	meshFile >> modelInfo.numberOfTriangles;

	// Look for the beggining of the vertex listings
	while (meshFile >> nextData)
	{
		if (nextData == "end_header") // Listings are right after this word
		{
			break;
		}
	}

	// Create an array to store the vertices and set all to 0
	sOriginalMeshInfo* originalInfo = new sOriginalMeshInfo();
	originalInfo->pVerticesFromFile = new sMeshVertex[modelInfo.numberOfVertices];
	memset(originalInfo->pVerticesFromFile, 0, sizeof(sMeshVertex) * modelInfo.numberOfVertices);

	// Allocate all the vertex numbers from the file into the array
	// Also allocate the height map
	modelInfo.heightMap = new float[modelInfo.numberOfVertices];
	memset(modelInfo.heightMap, 0, sizeof(float) * modelInfo.numberOfVertices);
	for (unsigned int vertIndex = 0; vertIndex != modelInfo.numberOfVertices; vertIndex++)
	{
		// The data had only pos x, y, z in sequence, but now we have the normals too
		meshFile >> originalInfo->pVerticesFromFile[vertIndex].x;
		meshFile >> originalInfo->pVerticesFromFile[vertIndex].y;
		meshFile >> originalInfo->pVerticesFromFile[vertIndex].z;
					
		meshFile >> originalInfo->pVerticesFromFile[vertIndex].nx;
		meshFile >> originalInfo->pVerticesFromFile[vertIndex].ny;
		meshFile >> originalInfo->pVerticesFromFile[vertIndex].nz;

		// load UV values here!
		meshFile >> originalInfo->pVerticesFromFile[vertIndex].u;
		meshFile >> originalInfo->pVerticesFromFile[vertIndex].v;

		// Load height map!
		modelInfo.heightMap[vertIndex] = originalInfo->pVerticesFromFile[vertIndex].y;
	}

	// Create an array to store the triangles and set all to 0
	originalInfo->pTrianglesFromFile = new sMeshTriangle[modelInfo.numberOfTriangles];
	memset(originalInfo->pTrianglesFromFile, 0, sizeof(sMeshTriangle) * modelInfo.numberOfTriangles);

	// Allocate all the triangles from file into the array. Don't forget to discard the unneeded value
	unsigned int notWanted = 0;
	for (unsigned int triIndex = 0; triIndex != modelInfo.numberOfTriangles; triIndex++)
	{
		meshFile >> notWanted;
		meshFile >> originalInfo->pTrianglesFromFile[triIndex].vertex1;
		meshFile >> originalInfo->pTrianglesFromFile[triIndex].vertex2;
		meshFile >> originalInfo->pTrianglesFromFile[triIndex].vertex3;
	}

	//printf("Read from the file OK!\n");

	// Calculating extents, the farthest points of the mesh
	// Begin assuming that all are 0
	modelInfo.minX = originalInfo->pVerticesFromFile[0].x;
	modelInfo.minY = originalInfo->pVerticesFromFile[0].y;
	modelInfo.minZ = originalInfo->pVerticesFromFile[0].z;
	
	modelInfo.maxX = originalInfo->pVerticesFromFile[0].x;
	modelInfo.maxY = originalInfo->pVerticesFromFile[0].y;
	modelInfo.maxZ = originalInfo->pVerticesFromFile[0].z;

	for (unsigned int extIndex = 0; extIndex != modelInfo.numberOfVertices; extIndex++)
	{
		if (originalInfo->pVerticesFromFile[extIndex].x < modelInfo.minX) { modelInfo.minX = originalInfo->pVerticesFromFile[extIndex].x; }
		if (originalInfo->pVerticesFromFile[extIndex].y < modelInfo.minY) { modelInfo.minY = originalInfo->pVerticesFromFile[extIndex].y; }
		if (originalInfo->pVerticesFromFile[extIndex].z < modelInfo.minZ) { modelInfo.minZ = originalInfo->pVerticesFromFile[extIndex].z; }
																   			
		if (originalInfo->pVerticesFromFile[extIndex].x > modelInfo.maxX) { modelInfo.maxX = originalInfo->pVerticesFromFile[extIndex].x; }
		if (originalInfo->pVerticesFromFile[extIndex].y > modelInfo.maxY) { modelInfo.maxY = originalInfo->pVerticesFromFile[extIndex].y; }
		if (originalInfo->pVerticesFromFile[extIndex].z > modelInfo.maxZ) { modelInfo.maxZ = originalInfo->pVerticesFromFile[extIndex].z; }
	}

	modelInfo.extentX = modelInfo.maxX - modelInfo.minX;
	modelInfo.extentY = modelInfo.maxY - modelInfo.minY;
	modelInfo.extentZ = modelInfo.maxZ - modelInfo.minZ;

	modelInfo.maxExtent = modelInfo.extentX;
	if (modelInfo.maxExtent < modelInfo.extentY) { modelInfo.maxExtent = modelInfo.extentY; }
	if (modelInfo.maxExtent < modelInfo.extentZ) { modelInfo.maxExtent = modelInfo.extentZ; }

	modelInfo.originalMeshInfo = originalInfo;

	// Remember to close the file
	if (meshFile.is_open())
	{
		meshFile.close();
	}

	this->_mapModelInfoByName[modelInfo.meshFileName] = &modelInfo;
	return true;
}

bool cVaoMeshManager::_LoadDrawInfoIntoVAO(sModelDrawInfo& drawInfo, GLuint shaderProgramID)
{
	sOriginalMeshInfo* originalMesh = drawInfo.originalMeshInfo;

	if (drawInfo.skinnedModelInfo == nullptr)
	{
		// Create the file the way that the buffer will understand
		drawInfo.pVerticesToVBO = new sModel_xyz_rgb_n_uv_bones[drawInfo.numberOfVertices];
		memset(drawInfo.pVerticesToVBO, 0, sizeof(sModel_xyz_rgb_n_uv_bones) * drawInfo.numberOfVertices);

		// Copy the data from the model format to the vertex buffer format
		for (unsigned int i = 0; i != drawInfo.numberOfVertices; i++)
		{
			drawInfo.pVerticesToVBO[i].x = originalMesh->pVerticesFromFile[i].x;
			drawInfo.pVerticesToVBO[i].y = originalMesh->pVerticesFromFile[i].y;
			drawInfo.pVerticesToVBO[i].z = originalMesh->pVerticesFromFile[i].z;

			// Need to pass the normals to the VBO too, but we want them to be normalized
			glm::vec3 normal = glm::vec3(originalMesh->pVerticesFromFile[i].nx, originalMesh->pVerticesFromFile[i].ny, originalMesh->pVerticesFromFile[i].nz);
			glm::normalize(normal);

			drawInfo.pVerticesToVBO[i].nx = normal.x;
			drawInfo.pVerticesToVBO[i].ny = normal.y;
			drawInfo.pVerticesToVBO[i].nz = normal.z;

			// Add the colours
			drawInfo.pVerticesToVBO[i].r = 1.0f;
			drawInfo.pVerticesToVBO[i].g = 1.0f;
			drawInfo.pVerticesToVBO[i].b = 1.0f;

			// Add the textures
			drawInfo.pVerticesToVBO[i].u0 = originalMesh->pVerticesFromFile[i].u;
			drawInfo.pVerticesToVBO[i].v0 = originalMesh->pVerticesFromFile[i].v;
			drawInfo.pVerticesToVBO[i].u1 = 0.0f;
			drawInfo.pVerticesToVBO[i].v1 = 0.0f;
		}
	}

	// The "VAO" or Vertex Array Object needs to be
	// created and bound first. It will remember the
	// order of the rest of the buffers automatically.
	// In essence, it will use the indices array to
	// control the vertex array, so we don't need to
	// re organize the vertex array.
	// It can be set to 0 and re-remembered afterwards
	glGenVertexArrays(1, &(drawInfo.vaoID));
	glBindVertexArray(drawInfo.vaoID);

	// From this point, the VAO will remember
	// the order of things called first!
	// Vertex buffer
	glGenBuffers(1, &(drawInfo.vertexBufferID));
	glBindBuffer(GL_ARRAY_BUFFER, drawInfo.vertexBufferID); // - Sets the "type" of buffer - Makes it the 'current' buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(sModel_xyz_rgb_n_uv_bones) * drawInfo.numberOfVertices, drawInfo.pVerticesToVBO, GL_STATIC_DRAW); // Send to the GPU

	// Create the indices
	drawInfo.numberOfIndices = drawInfo.numberOfTriangles * 3; // 3 vertices for each triangle
	drawInfo.pIndicesToVBO = new unsigned int[drawInfo.numberOfIndices]; // This is a 1D array of unsigned ints
	memset(drawInfo.pIndicesToVBO, 0, sizeof(unsigned int) * drawInfo.numberOfIndices);

	// Load the indices
	unsigned int indicesIndex = 0;
	for (unsigned int trisIndex = 0; trisIndex != drawInfo.numberOfTriangles; trisIndex++, indicesIndex += 3)
	{
		drawInfo.pIndicesToVBO[indicesIndex] = originalMesh->pTrianglesFromFile[trisIndex].vertex1;
		drawInfo.pIndicesToVBO[indicesIndex + 1] = originalMesh->pTrianglesFromFile[trisIndex].vertex2;
		drawInfo.pIndicesToVBO[indicesIndex + 2] = originalMesh->pTrianglesFromFile[trisIndex].vertex3;
	}

	// Indices buffer
	glGenBuffers(1, &(drawInfo.indexBufferID));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawInfo.indexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * drawInfo.numberOfIndices, drawInfo.pIndicesToVBO, GL_STATIC_DRAW);

	//// Get set vertex layout
	GLuint vPosLocation = glGetAttribLocation(shaderProgramID, "vPosition");			// Position
	GLuint vColLocation = glGetAttribLocation(shaderProgramID, "vColour");				// Color
	GLuint vNormLocation = glGetAttribLocation(shaderProgramID, "vNormal");				// Normal
	GLuint vUVx2Location = glGetAttribLocation(shaderProgramID, "vUV_x2");				// UV

	GLint vTanXYZLocation = glGetAttribLocation(shaderProgramID, "vTanXYZ");			// Tangent
	GLint vBiNormXYZLocation = glGetAttribLocation(shaderProgramID, "vBiNormXYZ");		// BiNormal
	GLint vBoneIDLocation = glGetAttribLocation(shaderProgramID, "vBoneID");			// BoneID
	GLint vBoneWeightLocation = glGetAttribLocation(shaderProgramID, "vBoneWeight");	// Bone Weight

	// Position
	glEnableVertexAttribArray(vPosLocation);
	glVertexAttribPointer(vPosLocation, 3, GL_FLOAT, GL_FALSE, sizeof(sModel_xyz_rgb_n_uv_bones), (void*)offsetof(sModel_xyz_rgb_n_uv_bones, x)); // Location, how many "points" (xy, or xyz), data type, normalize, number where the next line should start, beggining

	// Normals
	glEnableVertexAttribArray(vNormLocation);
	glVertexAttribPointer(vNormLocation, 3, GL_FLOAT, GL_FALSE, sizeof(sModel_xyz_rgb_n_uv_bones), (void*)offsetof(sModel_xyz_rgb_n_uv_bones, nx));

	// Color
	glEnableVertexAttribArray(vColLocation);
	glVertexAttribPointer(vColLocation, 3, GL_FLOAT, GL_FALSE, sizeof(sModel_xyz_rgb_n_uv_bones), (void*)offsetof(sModel_xyz_rgb_n_uv_bones, r));

	// UV
	glEnableVertexAttribArray(vUVx2Location);
	glVertexAttribPointer(vUVx2Location, 4, GL_FLOAT, GL_FALSE, sizeof(sModel_xyz_rgb_n_uv_bones), (void*)offsetof(sModel_xyz_rgb_n_uv_bones, u0));

	//HACK: Still need to do this here!!
	// Tangent

	// BiNormal

	// Bone ID
	glEnableVertexAttribArray(vBoneIDLocation);
	glVertexAttribPointer(vBoneIDLocation, 4,	GL_FLOAT, GL_FALSE, sizeof(sModel_xyz_rgb_n_uv_bones), (void*)offsetof(sModel_xyz_rgb_n_uv_bones, boneIDs[0]));

	// Bone Weight
	glEnableVertexAttribArray(vBoneWeightLocation);
	glVertexAttribPointer(vBoneWeightLocation, 4, GL_FLOAT, GL_FALSE, sizeof(sModel_xyz_rgb_n_uv_bones),	(void*)offsetof(sModel_xyz_rgb_n_uv_bones, boneWeights[0]));


	// No current VAO. We can call the ID when needed, since it will remember
	// the changes saved on it.
	glBindVertexArray(0);

	glDisableVertexAttribArray(vPosLocation);
	glDisableVertexAttribArray(vColLocation);
	glDisableVertexAttribArray(vNormLocation);
	glDisableVertexAttribArray(vUVx2Location);

	glDisableVertexAttribArray(vTanXYZLocation);
	glDisableVertexAttribArray(vBiNormXYZLocation);
	glDisableVertexAttribArray(vBoneIDLocation);
	glDisableVertexAttribArray(vBoneWeightLocation);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return true;
}

bool cVaoMeshManager::LoadModelIntoVAO(sModelDrawInfo& drawInfo, GLuint shaderProgramID)
{
	// Check to see if the model has already been loaded
	std::map<std::string /*model name*/, sModelDrawInfo* /*draw info*/>::iterator mapIter;
	mapIter = this->_mapModelInfoByName.find(drawInfo.meshFileName);

	if (mapIter != this->_mapModelInfoByName.end())
	{
		// Found it
		this->_AppendTextToLastError("Model ", false);
		this->_AppendTextToLastError(drawInfo.meshFileName, false);
		this->_AppendTextToLastError(" was already loaded!", true);
		return false;
	}

	// Load the model from the file to get
	// the vertices, triangles and bounding boxes
	// It might already have been loaded by assimp
	if (drawInfo.numberOfVertices == 0)
	{
		if (!this->_LoadModelFromFile(drawInfo))
		{
			this->_AppendTextToLastError("Model ", false);
			this->_AppendTextToLastError(drawInfo.meshFileName, false);
			this->_AppendTextToLastError(" wasn't read from the file!", true);
			return false;
		}
	}

	// Load the vertices and indices to the VAO
	// and send the data into the GPU
	if (!this->_LoadDrawInfoIntoVAO(drawInfo, shaderProgramID))
	{
		this->_AppendTextToLastError("Model ", false);
		this->_AppendTextToLastError(drawInfo.meshFileName, false);
		this->_AppendTextToLastError(" wasn't loaded into the VAO!", true);
		return false;
	}

	// Add to the map and return
	//this->_mapModelInfoByName[drawInfo.meshFileName] = &drawInfo;
	return true;
}

bool cVaoMeshManager::FindDrawInfoByName(const std::string& meshName, sModelDrawInfo** drawInfo)
{
	// Check to see if the model exists on the map
	std::map<std::string /*model name*/, sModelDrawInfo* /*draw info*/>::iterator mapIter;
	mapIter = this->_mapModelInfoByName.find(meshName);

	if (mapIter == this->_mapModelInfoByName.end())
	{
		this->_AppendTextToLastError("Model ", false);
		this->_AppendTextToLastError(meshName, false);
		this->_AppendTextToLastError(" wasn't found on the map!", true);
		return false;
	}

	// Return the draw info
	drawInfo = &mapIter->second;
	return true;
}

sModelDrawInfo* cVaoMeshManager::FindDrawInfoByName(const std::string & meshName)
{
	// Check to see if the model exists on the map
	std::map<std::string /*model name*/, sModelDrawInfo* /*draw info*/>::iterator mapIter;
	mapIter = this->_mapModelInfoByName.find(meshName);

	if (mapIter == this->_mapModelInfoByName.end())
	{
		this->_AppendTextToLastError("Model ", false);
		this->_AppendTextToLastError(meshName, false);
		this->_AppendTextToLastError(" wasn't found on the map!", true);
		return nullptr;
	}

	// Return the draw info
	return mapIter->second;
}

void cVaoMeshManager::UpdateModelVertexInformation(sModelDrawInfo& drawInfo)
{
	glBindBuffer(GL_ARRAY_BUFFER, drawInfo.vertexBufferID);

	unsigned int vertexBufferSizeInBytes = sizeof(sModel_xyz_rgb_n_uv_bones) * drawInfo.numberOfVertices;

	// ASSUME it's a dynmaic buffer
	// CAN update the vertex information (later)
	glBufferData(GL_ARRAY_BUFFER, vertexBufferSizeInBytes, drawInfo.pVerticesToVBO, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);	// "un-bind" the buffer

	return;
}

bool cVaoMeshManager::UnloadModelFromVAO(sModelDrawInfo* modelInfo)
{
	this->_mapModelInfoByName.erase(modelInfo->friendlyName);

	glBindBuffer(GL_ARRAY_BUFFER, modelInfo->vertexBufferID);
	glDeleteBuffers(1, &modelInfo->vertexBufferID);

	glBindBuffer(GL_ARRAY_BUFFER, modelInfo->indexBufferID);
	glDeleteBuffers(1, &modelInfo->indexBufferID);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return true;
}

void cVaoMeshManager::_AppendTextToLastError(std::string text, bool addNewLineBefore)
{
	std::stringstream ssError;
	ssError.str() = this->_lastError;

	if (addNewLineBefore)
	{
		ssError << std::endl;
	}
	ssError << text;

	this->_lastError = ssError.str();

	return;
}

std::string cVaoMeshManager::GetLastError(bool bClear)
{
	std::string errorText = this->_lastError;
	if (bClear)
	{
		this->_lastError = "";
	}
	return errorText;
}

void cVaoMeshManager::Shutdown()
{
	//for (std::map<std::string, sModelDrawInfo>::iterator mapiter = this->_mapModelInfoByName.begin(); mapiter != this->_mapModelInfoByName.end(); mapiter++)
	//{
	//	// Delete vertices and indices
	//	delete[] mapiter->second.pVerticesToVBO;
	//	delete[] mapiter->second.pIndicesToVBO;

	//	if (mapiter->second.originalMeshInfo != nullptr)
	//	{
	//		delete[] mapiter->second.originalMeshInfo->pTrianglesFromFile;
	//		delete[] mapiter->second.originalMeshInfo->pVerticesFromFile;
	//		delete mapiter->second.originalMeshInfo;
	//	}

	//	if (mapiter->second.skinnedModelInfo != nullptr)
	//	{
	//		delete mapiter->second.skinnedModelInfo;
	//	}
	//}

	//this->_mapModelInfoByName.clear();
}