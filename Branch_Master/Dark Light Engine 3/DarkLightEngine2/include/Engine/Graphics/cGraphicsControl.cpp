// cGraphicsControl.cpp

#include "cGraphicsControl.h"
#include <Shader\cShaderManager.h>
#include <Mesh\cVaoMeshManager.h>
#include <Mesh\Assimp\cAssimpMeshLoader.h>
#include <Texture\cTextureManager.h>
#include <Engine\Assets\cAssetManager.h>


cGraphicsControl::cGraphicsControl()
{
	return;
}

cGraphicsControl::~cGraphicsControl()
{
	this->DestroyGraphics();

	return;
}

void cGraphicsControl::CreateGraphics()
{
	this->_pShaderManager = new cShaderManager();
	this->_pVaoMeshManager = new cVaoMeshManager();
	this->_pAssimpMeshLoader = new cAssimpMeshLoader();
	this->_pTextureManager = new cTextureManager();
	this->_pAssetManager = new cAssetManager();
}

void cGraphicsControl::DestroyGraphics()
{
	if (this->_pShaderManager)
	{
		delete this->_pShaderManager;
		this->_pShaderManager = nullptr;
	}
	if (this->_pVaoMeshManager)
	{
		delete this->_pVaoMeshManager;
		this->_pVaoMeshManager = nullptr;
	}
	if (this->_pAssimpMeshLoader)
	{
		delete this->_pAssimpMeshLoader;
		this->_pAssimpMeshLoader = nullptr;
	}
	if (this->_pTextureManager)
	{
		delete this->_pTextureManager;
		this->_pTextureManager = nullptr;
	}
	if (this->_pAssetManager)
	{
		delete this->_pAssetManager;
		this->_pAssetManager = nullptr;
	}
}

cVaoMeshManager* cGraphicsControl::GetVaoMeshManager()
{
	return this->_pVaoMeshManager;
}

cShaderManager* cGraphicsControl::GetShaderManager()
{
	return this->_pShaderManager;
}

cAssimpMeshLoader* cGraphicsControl::GetAssimpMeshLoader()
{
	return this->_pAssimpMeshLoader;
}

cTextureManager* cGraphicsControl::GetTextureManager()
{
	return this->_pTextureManager;
}

cAssetManager* cGraphicsControl::GetAssetManager()
{
	return this->_pAssetManager;
}
