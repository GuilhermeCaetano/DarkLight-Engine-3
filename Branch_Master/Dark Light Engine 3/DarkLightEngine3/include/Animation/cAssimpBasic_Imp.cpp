// cAssimpBasic_Imp.cpp

#include <Animation\cAssimpBasic_Imp.h>

cAssimpBasic_Imp::cAssimpBasic_Imp()
{
	return;
}

cAssimpBasic_Imp::~cAssimpBasic_Imp()
{
	return;
}

bool cAssimpBasic_Imp::LoadSkinnedMeshFromFile(std::string filename, std::string friendlyName)
{
	//// Create scene to load the model from
	//const struct aiScene* pScene = nullptr;

	//// Create the importer and set flags
	//Assimp::Importer mImporter;
	//unsigned int Flags = aiProcess_Triangulate | aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph | 
	//			aiProcess_JoinIdenticalVertices | aiProcess_GenNormals | aiProcess_CalcTangentSpace;

	//// Read model from the file
	//pScene = mImporter.ReadFile(filename.c_str(), Flags);

	//if (pScene != nullptr)
	//{

	//}
	return true;
}

// Loads a model and returns a mesh
bool cAssimpBasic_Imp::loadModelA(std::string filename, sModelDrawInfo& mesh, std::string& error)
{
//	// TODO: Assimp stuff
//	const struct aiScene* scene = NULL;
//
//	Assimp::Importer mImporter;
//	unsigned int Flags = aiProcess_Triangulate | aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph | 
//		                 aiProcess_JoinIdenticalVertices | aiProcess_GenNormals |aiProcess_CalcTangentSpace;
//
//	scene = mImporter.ReadFile(filename.c_str(), Flags);
//
//	if ( scene->HasMeshes() )			// or ( scene->mNumMeshes > 0 )
//	{
//		// assume it's the 1st mesh only
//		mesh.numberOfVertices = scene->mMeshes[0]->mNumVertices;
//
//		// Create the number of vertices that this mesh has.
//		mesh.pVerticesToVBO = new sModel_xyz_rgb_n_uv_bones[mesh.numberOfVertices];
//
//		mesh.minX = scene->mMeshes[0]->mVertices[0].x;
//		mesh.minY = scene->mMeshes[0]->mVertices[0].y;
//		mesh.minZ = scene->mMeshes[0]->mVertices[0].z;
//		
//		mesh.maxX = mesh.minX;
//		mesh.maxY = mesh.minY;
//		mesh.maxZ = mesh.minZ;
//
//		for ( unsigned int index = 0; index != mesh.numberOfVertices; index++ )
//		{
//			mesh.pVerticesToVBO[index].x = scene->mMeshes[0]->mVertices[index].x;
//			mesh.pVerticesToVBO[index].y = scene->mMeshes[0]->mVertices[index].y;
//			mesh.pVerticesToVBO[index].z = scene->mMeshes[0]->mVertices[index].z;
//
//			if (mesh.pVerticesToVBO[index].x < mesh.minX )	{ mesh.minX = mesh.pVerticesToVBO[index].x; }
//			if (mesh.pVerticesToVBO[index].y < mesh.minY )	{ mesh.minY = mesh.pVerticesToVBO[index].y; }
//			if (mesh.pVerticesToVBO[index].z < mesh.minZ)	{ mesh.minZ = mesh.pVerticesToVBO[index].z; }
//
//			if (mesh.pVerticesToVBO[index].x > mesh.maxX )	{ mesh.maxX = mesh.pVerticesToVBO[index].x; }
//			if (mesh.pVerticesToVBO[index].y > mesh.maxY )	{ mesh.maxY = mesh.pVerticesToVBO[index].y; }
//			if (mesh.pVerticesToVBO[index].z > mesh.maxZ )	{ mesh.maxZ = mesh.pVerticesToVBO[index].z; }
//
//
//			mesh.pVerticesToVBO[index].nx = scene->mMeshes[0]->mNormals[index].x;
//			mesh.pVerticesToVBO[index].ny = scene->mMeshes[0]->mNormals[index].y;
//			mesh.pVerticesToVBO[index].nz = scene->mMeshes[0]->mNormals[index].z;
//
//			// A little crazy... 
//			// - There can be up to 8 UVs PER vertex
//			// - [0] is the 1st UV for that vertex
//			// - 2 is that there are 2 UV COORDINATES at THAT vertex (u & v ..... or s&t)
//			if ( scene->mMeshes[0]->mNumUVComponents[0] == 2 )
//			{
//				mesh.pVerticesToVBO[index].u1 = scene->mMeshes[0]->mTextureCoords[0][index].x;
//				mesh.pVerticesToVBO[index].v1 = scene->mMeshes[0]->mTextureCoords[0][index].y;
//
////				theMesh.pVertices[index].u2 = scene->mMeshes[0]->mTextureCoords[1][index].x;
////				theMesh.pVertices[index].v2 = scene->mMeshes[0]->mTextureCoords[1][index].y;
//			}
//		}
//
//		mesh.numberOfTriangles = scene->mMeshes[0]->mNumFaces;
//		mesh.pTrianglesFromFile = new sModelTriangle[mesh.numberOfTriangles];
//
//		// Now copy the triangle vertex information (indices)
//		for ( int triIndex = 0; triIndex != mesh.numberOfTriangles; triIndex++ )
//		{
//			mesh.pTrianglesFromFile[triIndex].vertex1 = scene->mMeshes[0]->mFaces[triIndex].mIndices[0];
//			mesh.pTrianglesFromFile[triIndex].vertex2 = scene->mMeshes[0]->mFaces[triIndex].mIndices[1];
//			mesh.pTrianglesFromFile[triIndex].vertex3 = scene->mMeshes[0]->mFaces[triIndex].mIndices[2];
//
//		}//for ( int triIndex...
//
//		mesh.meshFileName = filename;
//		mesh.CalculateExtents();
//	}

	return true;
}
