#include <Texture\cTextureManager.h>
#include <Shader\cShaderManager.h>
#include <Threading\cTextureTripleBuffer.h>

#include <sstream>


/*------------------------- TEXTURE THREAD TRIPLE BUFFERING -----------------------------------------------------
1 - Textures will normally blocking load low res textures on the main thread, 
	so that the game can't start without textures
2 - After all low res texture have been loaded, a thread will be spawned for each texture.
3 - Each texture thread will (this will block the specific thread, but not the main thread)
	- Load the high res texture into the CPU
	- Load the high res texture into the GPU
	- Update the triple buffer so that the map info CAN'T be read and written to at the same time
4 - Check for pending texture loads and update the high res map so the next read can read the high res texture
----------------------------------------------------------------------------------------------------------------*/

unsigned long cTextureManager::_uniqueID = 0;

cTextureManager::cTextureManager()
{
	this->_totalNumTextures = 0;
	this->_bAllLowResLoaded = false;

	return;
}

cTextureManager::~cTextureManager()
{
	this->_Cleanup();

	return;
}

bool cTextureManager::CreateTextureTripleBuffer()
{
	// Error if all lower res textures haven't been loaded
	/*if (!this->_bAllLowResLoaded)
	{
		printf("Error! Not all low resolution textures have been added!\n");
		return false;
	}*/

	if (this->_pTexTripleThread == nullptr)
	{
		this->_pTexTripleThread = new cTextureTripleBuffer();
	}
	
	this->_pTexTripleThread->InitBuffers(this->_uniqueID);

	// Reset unique_ID so the high res textures can be mapped correctly
	this->_uniqueID = 0;

	return false;
}

void cTextureManager::SetBasePath(std::string basepath)
{
	this->_basePath = basepath;
	return;
}


bool cTextureManager::Create2DTexture_BMP(std::string textureFileName, std::string texturePrepend, bool bGenerateMipMap)
{
	std::string fileToLoadFullPath = this->_basePath + "/" + texturePrepend + textureFileName;

	cTextureBMP* pTempTexture = new cTextureBMP(textureFileName, fileToLoadFullPath, 0, bGenerateMipMap);
	if (!pTempTexture->CreateNewTextureFromBMPFile2(textureFileName, fileToLoadFullPath, /*textureUnit,*/ bGenerateMipMap))
	{
		this->_AppendErrorString("Can't load ");
		this->_AppendErrorString(fileToLoadFullPath);
		this->_AppendErrorString("\n");
		return false;
	}

	this->_mapNonOptTexByName[textureFileName] = pTempTexture;
	this->_uniqueID++;

	return true;
}

bool cTextureManager::Create2DLowResTexture_BMP(std::string textureFileName, bool bGenerateMipMap)
{
	std::string fullPath = this->_basePath + "/" + "lowRes_" + textureFileName;


	cTextureBMP* pTempTexture = new cTextureBMP(textureFileName, fullPath, this->_uniqueID, bGenerateMipMap);
	if (!pTempTexture->CreateNewTextureFromBMPFile2(textureFileName, fullPath, bGenerateMipMap))
	{
		this->_AppendErrorString("Can't load ");
		this->_AppendErrorString(fullPath);
		this->_AppendErrorString("\n");
		return false;
	}

	// Add to low res map and update uniqueID
	this->_mapLowResTexByName[textureFileName] = pTempTexture;
	this->_uniqueID++;

	return true;
}

bool cTextureManager::CreateThreaded2DHighResTexture_BMP(const std::string& textureFileName, bool bGenerateMipMap)
{
	std::string fullPath = this->_basePath + "/" + "highRes_" + textureFileName;

	if (this->_pTexTripleThread == nullptr)
	{
		return this->Create2DTexture_BMP(textureFileName, "highRes_", bGenerateMipMap);
	}

	// Create texture
	cTextureBMP* pTempTexture = new cTextureBMP(textureFileName, fullPath, this->_uniqueID, bGenerateMipMap);

	// We don't need to use Critical Sections here,
	// since the triple buffer will be safe.

	// Just create the thread for the texture.
	bool result = pTempTexture->CreateTextureThread(this->_pTexTripleThread);
	if (!result)
	{
		printf("Error creating thread for %s texture!\n", fullPath.c_str());
		return false;
	}

	// Now let the thread take care of loading the high res texture...
	this->_mapPendingTexByID[this->_uniqueID] = pTempTexture;
	this->_uniqueID++;

	return true;
}

void cTextureManager::_AppendErrorString(std::string nextErrorText)
{
	std::stringstream ss;
	ss << this->_lastError << nextErrorText;
	this->_lastError = ss.str();
	return;
}

GLuint cTextureManager::getTextureIDFromName(std::string textureFileName)
{
	std::map< std::string, cTextureBMP* >::iterator itTexture
		= this->_mapNonOptTexByName.find(textureFileName);
	// Found it?
	if (itTexture == this->_mapNonOptTexByName.end())
	{
		return 0;
	}
	// Reutrn texture number (from OpenGL genTexture)
	return itTexture->second->getTextureNumber();
}

GLuint cTextureManager::GetThreadedTextureIDFromName(std::string textureFileName)
{
	std::string texName = "highRes_" + textureFileName;
	std::map< std::string, cTextureBMP* >::iterator mapHighIter = this->_mapHighResTexByName.find(textureFileName);

	// Try to find a high res version of this texture
	if (mapHighIter != this->_mapHighResTexByName.end())
	{
		if (mapHighIter->second->GetTextureInfo()->_texState == cTextureBMP::READY_TO_USE)
		{
			return mapHighIter->second->getTextureNumber();
		}
	}

	// No high res. Do we have a low res then?
	texName = "lowRes_" + textureFileName;
	std::map< std::string, cTextureBMP* >::iterator mapLowIter = this->_mapLowResTexByName.find(textureFileName);

	// Try to find a low res version of this texture
	if (mapLowIter != this->_mapLowResTexByName.end())
	{
		return mapLowIter->second->getTextureNumber();
	}

	// Try to find a non-optimized texture
	std::map< std::string, cTextureBMP* >::iterator mapNonOptIter = this->_mapNonOptTexByName.find(textureFileName);

	// Try to find a low res version of this texture
	if (mapNonOptIter != this->_mapNonOptTexByName.end())
	{
		return mapNonOptIter->second->getTextureNumber();
	}

	// No textures were found...
	return 0;
}


void cTextureManager::_AppendErrorStringLine(std::string nextErrorTextLine)
{
	std::stringstream ss;
	ss << this->_lastError << std::endl;
	ss << nextErrorTextLine << std::endl;
	this->_lastError = ss.str();
	return;
}

bool cTextureManager::_LoadPendingTextureIntoGPU(cTextureBMP* pTexture)
{
	if (!pTexture->LoadTextureIntoGPU(true))
	{
		printf("Error! Texture not loaded into GPU.\n");
		return false;
	}

	return true;
}

void cTextureManager::_Cleanup()
{
	// Delete all textures from the maps
	std::map<std::string, cTextureBMP*>::iterator mapLowIter = this->_mapLowResTexByName.begin();
	std::map<std::string, cTextureBMP*>::iterator mapHighIter = this->_mapHighResTexByName.begin();
	std::map<std::string, cTextureBMP*>::iterator mapNonOptIter = this->_mapNonOptTexByName.begin();
	std::map<unsigned long, cTextureBMP*>::iterator mapPendingIter = this->_mapPendingTexByID.begin();

	for (mapLowIter; mapLowIter != this->_mapLowResTexByName.end(); mapLowIter++)
	{
		delete mapLowIter->second;
		mapLowIter->second = nullptr;
	}

	//TODO: For now, all the high def textures will be deleted from the triple buffer.
	// Since this map only contains fully loaded textures, it might not contain all
	// created textures depending on program exit time, while the triple buffer will contain all of them.
	// It might be better in the future to not have the high map, and only read from the read buffer.
	for (mapHighIter; mapHighIter != this->_mapHighResTexByName.end(); mapHighIter++)
	{
		delete mapHighIter->second;
		mapHighIter->second = nullptr;
	}

	for (mapPendingIter; mapPendingIter != this->_mapPendingTexByID.end(); mapPendingIter++)
	{
		delete mapPendingIter->second;
		mapPendingIter->second = nullptr;
	}

	for (mapNonOptIter; mapNonOptIter != this->_mapNonOptTexByName.end(); mapNonOptIter++)
	{
		delete mapNonOptIter->second;
		mapNonOptIter->second = nullptr;
	}

	if (this->_pTexTripleThread != nullptr)
	{
		delete this->_pTexTripleThread;
		this->_pTexTripleThread = nullptr;
	}
}

// Picks a random texture from the textures loaded
std::string cTextureManager::PickRandomTexture(void)
{
	if (this->_mapNonOptTexByName.empty())
	{
		// There are no textures loaded, yet.
		return "";
	}

	int textureIndexRand = rand() % (this->_mapNonOptTexByName.size() + 1);
	if (textureIndexRand >= this->_mapNonOptTexByName.size())
	{
		textureIndexRand = 0;
	}

	std::map< std::string, cTextureBMP* >::iterator itTex = this->_mapNonOptTexByName.begin();
	for (unsigned int count = 0; count != textureIndexRand; count++, itTex++)
	{
	}

	return itTex->second->getTextureName();
}


bool cTextureManager::CreateCubeTextureFromBMPFiles(
	std::string cubeMapName,
	std::string posX_fileName, std::string negX_fileName,
	std::string posY_fileName, std::string negY_fileName,
	std::string posZ_fileName, std::string negZ_fileName,
	bool bIsSeamless, std::string &errorString)
{
	std::string posX_fileName_FullPath = this->_basePath + "/" + posX_fileName;
	std::string negX_fileName_FullPath = this->_basePath + "/" + negX_fileName;
	std::string posY_fileName_FullPath = this->_basePath + "/" + posY_fileName;
	std::string negY_fileName_FullPath = this->_basePath + "/" + negY_fileName;
	std::string posZ_fileName_FullPath = this->_basePath + "/" + posZ_fileName;
	std::string negZ_fileName_FullPath = this->_basePath + "/" + negZ_fileName;

	GLenum errorEnum;
	std::string errorDetails;
	cTextureBMP* pTempTexture = new cTextureBMP(cubeMapName, "", 0, false);
	if (!pTempTexture->CreateNewCubeTextureFromBMPFiles(
		cubeMapName,
		posX_fileName_FullPath, negX_fileName_FullPath,
		posY_fileName_FullPath, negY_fileName_FullPath,
		posZ_fileName_FullPath, negZ_fileName_FullPath,
		bIsSeamless, errorEnum, errorString, errorDetails))
	{
		this->_AppendErrorString("Can't load ");
		this->_AppendErrorString(cubeMapName);
		this->_AppendErrorString(" because:\n");
		this->_AppendErrorString(errorString);
		this->_AppendErrorString("\n");
		this->_AppendErrorString(errorDetails);
		errorString += ("\n" + errorDetails);
		return false;
	}//if ( ! pTempTexture->CreateNewCubeTextureFromBMPFiles()

	// Texture is loaded OK
	//this->m_nextTextureUnitOffset++;

	this->_mapNonOptTexByName[cubeMapName] = pTempTexture;

	return true;
}

void cTextureManager::LoadTextureUniformLocations(cShaderManager* pShaderManager)
{
	cShaderManager::cShaderProgram* pSP = pShaderManager->GetShaderProgramByName("simpleShader");
	pSP->LoadUniLoc("texture00");
	pSP->LoadUniLoc("texture01");
	pSP->LoadUniLoc("texture02");
	pSP->LoadUniLoc("texture03");
	pSP->LoadUniLoc("texture04");
	pSP->LoadUniLoc("texture05");
	pSP->LoadUniLoc("texture06");
	pSP->LoadUniLoc("texture07");
	pSP->LoadUniLoc("texBlendWeights[0]");
	pSP->LoadUniLoc("texBlendWeights[1]");
}

void cTextureManager::Update()
{
	// Check to exit early
	if (!this->_pTexTripleThread || this->_mapPendingTexByID.size() == 0)
	{
		return;
	}

	if (this->_pTexTripleThread != nullptr)
	{
		this->_pTexTripleThread->SwitchBuffers();
		this->CheckForPendingTextureLoads();
	}
}

bool cTextureManager::CheckForPendingTextureLoads()
{
	bool allDone = true;
	std::map<unsigned long, cTextureBMP*>::iterator mapIter = this->_mapPendingTexByID.begin();
	for (mapIter; mapIter != this->_mapPendingTexByID.end(); mapIter++)
	{
		cTextureBMP* tempTex = mapIter->second;
		if (tempTex->GetTextureInfo()->_texState == cTextureBMP::eTexState::READY_TO_USE)
		{
			continue;
		}

		allDone = false;
		std::pair<std::string, cTextureBMP::eTexState> readPair = this->_pTexTripleThread->vecsTextures[this->_pTexTripleThread->GetCurrentReadBuffer()][mapIter->first];
		std::pair<std::string, cTextureBMP::eTexState>* writePair = &this->_pTexTripleThread->vecsTextures[this->_pTexTripleThread->GetCurrentWriteBuffer()][mapIter->first];
		std::pair<std::string, cTextureBMP::eTexState>* standbyPair = &this->_pTexTripleThread->vecsTextures[this->_pTexTripleThread->GetCurrentStandbyBuffer()][mapIter->first];

		if (readPair.second == cTextureBMP::eTexState::PENDING)
		{
			continue;
		}

		// Is this texture ready to be sent to the GPU?
		if (readPair.second == cTextureBMP::LOADED_FROM_FILE)
		{
			// Load this texture on the GPU
			/*cTextureBMP* tempTex = this->_mapPendingTexByID.find(i)->second;
			this->_LoadPendingTextureIntoGPU(tempTex);
			tempTex->GetTextureInfo()->_texState = cTextureBMP::eTexState::LOADED_IN_GPU;
			writePair->second = cTextureBMP::eTexState::LOADED_IN_GPU;*/
			cTextureBMP* tempTex = mapIter->second;
			bool result = this->_LoadPendingTextureIntoGPU(tempTex);
			if (result)
			{
				tempTex->GetTextureInfo()->_texState = cTextureBMP::eTexState::READY_TO_USE;
				this->_mapHighResTexByName[readPair.first] = tempTex;
			}
		}
		//else if (readPair.second == cTextureBMP::LOADED_IN_GPU)
		//{
		//	// Is this texture all ready to be set on the map?
		//	// Set this texture into the high res map and set state to ready
		//	cTextureBMP* tempTex = this->_mapPendingTexByID.find(i)->second;
		//	writePair->second = cTextureBMP::READY_TO_USE;
		//	tempTex->GetTextureInfo()->_texState = cTextureBMP::eTexState::READY_TO_USE;
		//	this->_mapPendingTexByID.erase(i);
		//	this->_mapHighResTexByName[readPair.first] = tempTex;
		//}
	}

	// Check if all are ready and clear things
	if (allDone)
	{
		this->_mapPendingTexByID.clear();
		this->_pTexTripleThread->ClearBuffers();
	}

	// All done, if anything
	return true;
}

void cTextureManager::LoadTexture(std::string textureName, cTextureManager::eTexType texType, cTextureManager* pTextureManager, bool useMipMap)
{
	switch (texType)
	{
	case cTextureManager::LOW_RES:
		if (!this->Create2DLowResTexture_BMP(textureName, useMipMap))
		{
			printf("Error loading the %s texture!", textureName.c_str());
		}
		break;
	case cTextureManager::HIGH_RES:
		if (!this->CreateThreaded2DHighResTexture_BMP(textureName, useMipMap))
		{
			printf("Error loading the %s texture!", textureName.c_str());
		}
		break;
	case cTextureManager::DEFAULT:
		if (!this->Create2DTexture_BMP(textureName, "", useMipMap))
		{
			printf("Error loading the %s texture!", textureName.c_str());
		}
		break;
	default:
		break;
	}
}
