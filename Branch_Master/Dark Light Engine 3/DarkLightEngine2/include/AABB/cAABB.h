#ifndef _cAABB_HG_
#define _cAABB_HG_

#include <OpenGL\OpenGLHeaders.h>
#include <vector>
#include <string>


// My version of the AABB
// Using center and side length
class cAABB
{
public:
	// Struct for each triangle vertice
	struct sAABBTriangle
	{
		glm::vec3 vertices[3];
	};

	// Constructor will take the center and side length params on creation.
	// This will prevent anyone from re-setting these values later. 
	// Do we need to re-set them?
	cAABB(glm::vec3 centerXYZ, float sideLength, std::string meshName);
	~cAABB();

	// Getters for the center, side length and ID
	glm::vec3 GetCenterXYZ();
	float GetSideLength();
	std::string GetMeshName();

	// Generate ID automatically from private variables
	static unsigned long long GenerateID(glm::vec3 centerXYZ);
	// Generate ID from custom centerXYZ
	static unsigned long long GenerateID(glm::vec3 centerXYZ, float sideLength);
	unsigned long long GetID();
	// Pointer for all the triangles inside the AABBs
	std::vector<sAABBTriangle> _pVecAABBTriangles;

	bool bShouldDraw;

private:
	// Struct for each triangle based on indexes
	struct sAABBTriangle2
	{
		unsigned int vertIndexes[3];
	};
	// Pointer for all the triangles inside the AABBs
	std::vector<sAABBTriangle*> _pVecAABBTriangles2;

	// This AABB side length and center
	std::string _meshName;
	float _sideLength;
	glm::vec3 _centerXYZ;


	unsigned long long _ID;
};



#endif // !_cMyAABB_HG_

