#ifndef _nGenerateMap_HG_
#define _nGenerateMap_HG_

#include <string>
#include <Texture\cBitmapLoader.h>
#include <OpenGL\OpenGLHeaders.h>
#include <Engine\cEntityManager.h>
#include <Mesh\cVaoMeshManager.h>


namespace nGenerateMap
{
	bool GenerateMapFromBMP(cBitmapLoader::sBMPInfoHeader* bitmapInfo, cEntityManager* pEntityManager, cVaoMeshManager* pVaoMeshManager, glm::vec3 initialPos, unsigned int level);
	bool GenerateMapFromBMP(cBitmapLoader::sBMPInfoHeader* bitmapInfo, glm::vec3 initialPosition, std::vector<glm::vec3>& vecColour);
	//bool GenerateMapFromBMP(cBitmapLoader::sBMPInfoHeader* bitmapInfo, cEntityManager* pEntityManager, cVaoMeshManager* pVaoMeshManager, glm::vec3 initialPos);
}




#endif // !_nGenerateMap_HG_

