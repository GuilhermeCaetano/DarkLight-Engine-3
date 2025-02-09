#ifndef _cGraphicsControl_HG_
#define _cGraphicsControl_HG_

#include <Mesh\sModelDrawInfo.h>
#include <Mesh\cVaoMeshManager.h>

class cShaderManager;
class cAssimpMeshLoader;
class cTextureManager;
//class cAssetManager;
class cDebugRenderer;


class cGraphicsControl
{
public:
	cGraphicsControl();
	~cGraphicsControl();

	// Create and destroy all utils needed for graphics
	void CreateGraphics();
	void DestroyGraphics();

	// Accessors
	cVaoMeshManager* GetVaoMeshManager();
	cShaderManager* GetShaderManager();
	cAssimpMeshLoader* GetAssimpMeshLoader();
	cTextureManager* GetTextureManager();
	//cAssetManager* GetAssetManager();
	cDebugRenderer* GetDebugRederer();

private:
	// Graphics utils
	cVaoMeshManager* _pVaoMeshManager;
	cShaderManager* _pShaderManager;
	cAssimpMeshLoader* _pAssimpMeshLoader;
	cTextureManager* _pTextureManager;
	//cAssetManager* _pAssetManager;
	cDebugRenderer* _pDebugRenderer;

};


#endif // !_cGraphicsControl_HG_

