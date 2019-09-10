#ifndef _cAABBManager_HG_
#define _cAABBManager_HG_

#include <AABB\cAABB.h>
#include <OpenGL\OpenGLHeaders.h>
#include <map>
#include <string>

class cVaoMeshManager;


class cAABBManager
{
public:
	cAABBManager(cVaoMeshManager* pVaoMeshManager);
	~cAABBManager();

	void GenerateAABBs(float distanceBetween);
	void GenerateAABBs(std::string meshName, float distanceBetween);

	// Returns an AABB by it's ID
	cAABB* GetAABBByID(unsigned long long ID);

	// Should we draw all AABBs? Maybe for now to test, later do only when player is close
	void DrawAllAABBs(GLuint shaderProgram);

	// Map of all AABBs by ID
	std::map<unsigned long long /*AABB ID*/, cAABB* /*ID AABB*/> _pMapAABB;

private:
	cVaoMeshManager* _pVaoMeshManager;

};


#endif // !_cAABBManager_HG_

