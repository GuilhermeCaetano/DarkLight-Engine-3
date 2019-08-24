#ifndef _cTextureManager_HG_
#define _cTextureManager_HG_

#include <Shader\cShaderManager.h>
#include <Texture\cTextureBMP.h>


class cTextureTripleBuffer;
class cTextureManager
{
public:
	cTextureManager();
	~cTextureManager();

	enum eTexType
	{
		LOW_RES,
		HIGH_RES,
		DEFAULT
	};

	bool CreateTextureTripleBuffer();

	bool Create2DTexture_BMP(std::string textureFileName, std::string texturePrepend = "", bool bGenerateMipMap = true);

	// For the thread low, high type
	bool Create2DLowResTexture_BMP(std::string textureFileName, bool bGenerateMipMap = true);

	// Creates a thread
	bool CreateThreaded2DHighResTexture_BMP(const std::string& textureFileName, bool bGenerateMipMap = true);

	// Picks a random texture from the textures loaded
	std::string PickRandomTexture(void);

	bool CreateCubeTextureFromBMPFiles(std::string cubeMapName,
		std::string posX_fileName, std::string negX_fileName,
		std::string posY_fileName, std::string negY_fileName,
		std::string posZ_fileName, std::string negZ_fileName,
		bool bIsSeamless, std::string &errorString);


	// returns 0 on error
	GLuint getTextureIDFromName(std::string textureFileName);

	GLuint GetThreadedTextureIDFromName(std::string textureFileName);

	void SetBasePath(std::string basepath);

	void LoadTextureUniformLocations(cShaderManager* pShaderManager);

	void Update();

	// Check to see if the texture has been loaded, and is ready to go into the GPU
	// This gets called every frame (maybe) to see if there are some textures ready to go.
	bool CheckForPendingTextureLoads();

	void LoadTexture(std::string textureName, cTextureManager::eTexType texType, cTextureManager* pTextureManager, bool useMipMap = true);

	bool _bAllLowResLoaded;

private:
	std::string _basePath = "";
	std::string _lastError;
	void _AppendErrorString(std::string nextErrorText);
	void _AppendErrorStringLine(std::string nextErrorTextLine);

	// This does the actual loading of the textures
	bool _LoadPendingTextureIntoGPU(cTextureBMP* pTexture);

	void _Cleanup();

	std::map< std::string, cTextureBMP* > _mapNonOptTexByName;
	std::map< std::string, cTextureBMP* > _mapLowResTexByName;
	std::map< std::string, cTextureBMP* > _mapHighResTexByName;

	cTextureTripleBuffer* _pTexTripleThread;

	static unsigned long _uniqueID;

	unsigned long _totalNumTextures;
	bool _bAllHighResLoaded;
};


#endif // !_cTextureManager_HG_
