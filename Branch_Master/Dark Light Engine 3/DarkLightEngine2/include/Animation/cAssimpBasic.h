#ifndef _cAssimpBasic_HG_
#define _cAssimpBasic_HG_

class cAssimpBasic_Imp;
#include <Mesh\sModelDrawInfo.h>

class cAssimpBasic
{
public:
	cAssimpBasic();
	~cAssimpBasic();

	// Load a skinned mesh from file
	bool LoadSkinnedMeshFromFile(std::string filename, std::string friendlyName);

	// Loads a model and returns a mesh
	bool loadModelA( std::string filename, sModelDrawInfo& mesh, std::string& error );

private:
	cAssimpBasic_Imp* _pImp;

};


#endif
