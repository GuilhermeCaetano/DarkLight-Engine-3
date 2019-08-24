// cAssimpBasic.cpp

#include <Animation\cAssimpBasic.h>
#include <Animation\cAssimpBasic_Imp.h>

cAssimpBasic::cAssimpBasic()
{
	this->_pImp = new cAssimpBasic_Imp();

	return;
}

cAssimpBasic::~cAssimpBasic()
{
	delete this->_pImp;

	return;
}

bool cAssimpBasic::LoadSkinnedMeshFromFile(std::string filename, std::string friendlyName)
{
	return this->_pImp->LoadSkinnedMeshFromFile(filename, friendlyName);
}

// Loads a model and returns a mesh
bool cAssimpBasic::loadModelA(std::string filename, sModelDrawInfo& mesh, std::string& error)
{
	return this->_pImp->loadModelA( filename, mesh, error );
}
