// sOriginalMeshInfo

// This is the original mesh from the file
// unnaltered for the VAO. Can be updated if needed

#include <Mesh\sOriginalMeshInfo.h>
#include <OpenGL\OpenGLHeaders.h>

sOriginalMeshInfo::sOriginalMeshInfo()
{
	this->pVerticesFromFile = nullptr;
	this->pTrianglesFromFile = nullptr;

	this->meshFileName = "";

	this->oMaxX = this->oMaxY = this->oMaxZ = 0.0f;
	this->oMinX = this->oMinY = this->oMinZ = 0.0f;
	this->oExtentX = this->oExtentY = this->oExtentZ = 0.0f;
	this->oMaxExtent = 0.0f;
	this->oScaleForUnitBox = 0.0f;

	return;
}

sOriginalMeshInfo::~sOriginalMeshInfo()
{
	return;
}
