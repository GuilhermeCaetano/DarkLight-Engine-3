#ifndef _sModelDrawInfo_HG_
#define _sModelDrawInfo_HG_

#include <Mesh\sOriginalMeshInfo.h>

#include <string>
#include <array>
#include <vector>
#include <map>

#include <Scene\eModelTypes.h>
#include <glm\mat4x4.hpp>
#include <Animation\Assimp\postprocess.h>
#include <Animation\Assimp\scene.h>
#include <Animation\Assimp\Importer.hpp>


// Usually we want models that have vertices
// looking like this: xxxx, xxxxx, xxxxx
// a set of x, y, z coordinates for the vertices, normals and UVs

// Max number of bones per vertex
static const unsigned int NUMBEROFBONES = 4;

//// Information about the vertex
//struct sModelVertex
//{
//	float x, y, z; // Position
//	float nx, ny, nz; // Normals
//	float u, v;
//};
//
//// Information about the triangle
//struct sModelTriangle
//{
//	int vertex1;
//	int vertex2;
//	int vertex3;
//};

struct sVertexBoneData
{
	//std::array<unsigned int, MAX_BONES_PER_VERTEX> ids;
	std::array<float, NUMBEROFBONES> ids;
	std::array<float, NUMBEROFBONES> weights;

	void AddBoneData(unsigned int BoneID, float Weight);
};

struct sBoneInfo
{
	glm::mat4 BoneOffset;
	glm::mat4 FinalTransformation;
	glm::mat4 ObjectBoneTransformation;
};

// Information containing the position, rgb colours, normals and uvs.
// This is the final format expected by the VAO.
struct sModel_xyz_rgb_n_uv_bones
{
	sModel_xyz_rgb_n_uv_bones();

	float x, y, z; // Position
	float r, g, b, a; // Colour
	float nx, ny, nz; // Normals
	float u0, v0, u1, v1; // UVs (note that UVs go from 0 to 1 on texture mapping)
	float tx, ty, tz; // Tangent
	float bx, by, bz; // Bi-normals

	// Bones
	float boneIDs[NUMBEROFBONES];
	float boneWeights[NUMBEROFBONES];
};

//struct sAnimationInfo
//{
//	std::string friendlyName;
//	std::string fileName;
//	const aiScene* pAIScene;
//};

struct sSkinnedModelInfo
{
	sSkinnedModelInfo();

	//HACK: Bones... we should really rethink on where to put this when we have time... 
	Assimp::Importer importer;
	const aiScene* pScene;

	unsigned int numBones;
	glm::mat4 mGlobalInverseTransformation;

	std::vector<sBoneInfo> vecBoneInfo;
	std::vector<sVertexBoneData> vecVertexBoneData;	//Bones;

	std::map<std::string /*BoneName*/, unsigned int /*BoneIndex*/> mapBoneNameToBoneIndex;
	
	// Extent Values for skinned mesh
	// These can be updated per frame, from the "update skinned mesh" call
	glm::vec3 minXYZ_from_SM_Bones;
	glm::vec3 maxXYZ_from_SM_Bones;

	// Store all the bones for this model, being updated per frame
	std::vector< glm::mat4x4 > vecBoneGlobals;
	std::vector< glm::mat4x4 > vecFinalTransformation;
	std::vector< glm::mat4x4 > vecOffsets;

	glm::mat4x4 GetFinalBoneByName(std::string boneName);
	glm::mat4x4 GetBoneByName(std::string boneName);

	void GetListOfBoneIDandNames(std::vector<std::string> &vecBoneNames);
};

// The struct that will hold information about the model mesh in order to draw it
struct sModelDrawInfo
{
	sModelDrawInfo();
	~sModelDrawInfo();

	void CalculateExtents();
	void CalculateNormals();

	// Dynamic arrays
	//sMeshVertex* pVerticesFromFile;
	//sMeshTriangle* pTrianglesFromFile;
	unsigned int* pIndicesToVBO;
	sModel_xyz_rgb_n_uv_bones* pVerticesToVBO;

	// Vertex Buffer
	unsigned int vertexBufferID;
	unsigned int numberOfVertices;

	// Index Buffer
	unsigned int indexBufferID;
	unsigned int numberOfIndices;
	unsigned int numberOfTriangles;

	// Common info
	std::string meshFileName;
	std::string friendlyName;
	eModelTypes modelType;
	unsigned int meshIndex;
	unsigned int vaoID;

	// Extents
	float maxX, maxY, maxZ;
	float minX, minY, minZ;
	float extentX, extentY, extentZ;
	float maxExtent;
	float scaleForUnitBox;
	float* heightMap;
	
	// Bones
	sSkinnedModelInfo* skinnedModelInfo;
	sOriginalMeshInfo* originalMeshInfo;
};


#endif // !_cModelDrawInfo_HG_
