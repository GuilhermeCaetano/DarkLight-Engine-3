#ifndef _cAssimpBasic_Imp_HG_
#define _cAssimpBasic_Imp_HG_

#include <Animation\Assimp\Importer.hpp>
#include <Animation\Assimp\scene.h>
#include <Animation\Assimp\postprocess.h>
#include <Mesh\sModelDrawInfo.h>

class cAssimpBasic_Imp
{
public:
	cAssimpBasic_Imp();
	~cAssimpBasic_Imp();

	bool LoadSkinnedMeshFromFile(std::string filename, std::string friendlyName);

	// Loads a model and returns a mesh
	bool loadModelA(std::string filename, sModelDrawInfo& mesh, std::string& error);
};

#endif 
