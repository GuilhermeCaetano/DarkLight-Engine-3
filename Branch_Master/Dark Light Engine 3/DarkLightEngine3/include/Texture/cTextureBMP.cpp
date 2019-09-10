// Written by Michael Feeney, Fanshawe College, 2006
// mfeeney@fanshawec.on.ca
// It may be distributed under the terms of the MIT License:
// http://opensource.org/licenses/MIT
// (Also, it would be Supah Cool if I heard this was remotely useful!)
// Use this code at your own risk. It is indented only as a learning aid.


#include <Texture\cTextureBMP.h>

#include <fstream>
#include <iostream>

#include <Threading\cTextureTripleBuffer.h>

//#define DEBUG_MODE_BITMAPREAD
//#define DEBUG_MODE_BITMAPDISPLAY
//#define DEBUG_MODE_TEXTUREREPLACEMENT

// The thread function that will do the actual loading of the data
DWORD WINAPI TLoadTexture(PVOID pvParam)
{
	// Convert the class back from the void*
	cTextureBMP* pTextureBMP = (cTextureBMP*)pvParam;

	//printf("Thread is loading into CPU %s.\n", pTextureBMP->GetTextureFullPathName().c_str());

	// This will block this specific thread until the texture has been loaded,
	// but this won't affect performance since it's not the main thread.
	bool result = pTextureBMP->LoadTextureFromCPU(pTextureBMP->GetTextureFullPathName());

	// Keep updating the triple buffer until the vector has the correct full information
	while (!pTextureBMP->UpdateTripleBuffer())
	{
		// Delay for about 100 ms
		Sleep(100);
	}

	// Texture has been loaded to the CPU here
	//printf("Thread %s was loaded into CPU. \n", pTextureBMP->GetTextureFullPathName().c_str());

	// Textures are loaded in the CPU and ready to be added to the GPU.
	// This can't be done here, since OpenGL is NOT thread safe, so it will be done on the script.

	return 0;
}

cTextureBMP::cTextureBMP()
	: m_Height_or_Rows(0), m_Width_or_Columns(0), m_OriginalHeight(0), m_OriginalWidth(0), m_p_theImages(0),
	m_lastErrorNum(0), m_FileSize(0), m_offsetInBits(0), m_headerSize(0),
	m_numberOfRows(0), m_numberOfColumns(0), m_numberOfPlanes(0),
	m_bitPerPixel(0), m_compressionMode(0), m_imageSizeInBytes(0),
	m_PixelsPerMeterX(0), m_PixelsPerMeterY(0),
	m_numberOfLookUpTableEntries(0), m_numberOfImportantColours(0),
	m_textureNumber(0), m_bHave_cout_output(false), /*m_textureUnit(0),*/
	m_bIsCubeMap(false), m_bIs2DTexture(false)
{
	this->_pTexInfo = nullptr;
	this->m_p_theImages = nullptr;
	this->_pTexTripleBuffer = nullptr;

	return;
}

cTextureBMP::cTextureBMP(const std::string& textureFileName, const std::string& fullPathName, unsigned long index, bool bGenerateMipMap)
	: m_Height_or_Rows(0), m_Width_or_Columns(0), m_OriginalHeight(0), m_OriginalWidth(0), m_p_theImages(0),
	m_lastErrorNum(0), m_FileSize(0), m_offsetInBits(0), m_headerSize(0),
	m_numberOfRows(0), m_numberOfColumns(0), m_numberOfPlanes(0),
	m_bitPerPixel(0), m_compressionMode(0), m_imageSizeInBytes(0),
	m_PixelsPerMeterX(0), m_PixelsPerMeterY(0),
	m_numberOfLookUpTableEntries(0), m_numberOfImportantColours(0),
	m_textureNumber(0), m_bHave_cout_output(false), /*m_textureUnit(0),*/
	m_bIsCubeMap(false), m_bIs2DTexture(false)
{
	//this->m_p_theImages = nullptr;
	//this->_pTexTripleBuffer = nullptr;

	sTextureInfo* texInfo = new sTextureInfo(textureFileName, fullPathName, index, bGenerateMipMap);
	this->_pTexInfo = texInfo;

	return;
}

cTextureBMP::~cTextureBMP()
{
	if (this->_pTexInfo != nullptr)
	{
		delete this->_pTexInfo;
		this->_pTexInfo = nullptr;
	}
	if (this->m_p_theImages != nullptr)
	{
		delete[] this->m_p_theImages;
		this->m_p_theImages = nullptr;
	}
	
	return;
}

bool cTextureBMP::CreateTextureThread(cTextureTripleBuffer* pTexTripleBuffer)
{
	// Set triple buffer
	this->_pTexTripleBuffer = pTexTripleBuffer;

	// Create thread
	LPDWORD phThread = 0;
	DWORD hThread = 0;
	HANDLE hThreadHandle = 0;

	void* pTheThisPointer = (void*)this;

	// Pass the this pointer to the thread function
	hThreadHandle = CreateThread(NULL,	// Default security
		0,		// Stack size - default - win32 = 1 Mbyte
		&TLoadTexture, // Pointer to the thread's function
		pTheThisPointer,		// The value (parameter) we pass to the thread
			   // This is a pointer to void... more on this evil thing later...
		0,  // CREATE_SUSPENDED or 0 for threads...
		(DWORD*)&phThread);		// pointer or ref to variable that will get loaded with threadID

	if (hThreadHandle == NULL)
	{
		return false;
	}

	return true;
}

void cTextureBMP::SetDebug_cout_output(bool bHave_cout_output)
{
	this->m_bHave_cout_output = bHave_cout_output;
}

GLuint cTextureBMP::getTextureNumber(void)
{
	return this->m_textureNumber;
}

std::string cTextureBMP::getTextureName(void)
{
	return this->_pTexInfo->_texName;
}

std::string cTextureBMP::getFileNameFullPath(void)
{
	return this->_pTexInfo->_fullPathName;
}

bool cTextureBMP::CreateNewTextureFromBMPFile2(std::string textureName, std::string fileNameFullPath,
	/*GLenum textureUnit,*/ bool bGenerateMIPMap)
{
	bool bReturnVal = true;

	// Clear any old openGL errors???
	int IHateYou = glGetError();

	// Pick a texture number...
//	GLuint textureNum = 0;
	this->m_textureNumber = 0;
	glGenTextures(1, &(this->m_textureNumber));
	// Worked?
	if ((glGetError() & GL_INVALID_VALUE) == GL_INVALID_VALUE)
	{
		bReturnVal = false;
		return false;
	}


	if (!this->LoadBMP2(this->_pTexInfo->_fullPathName))
	{
		return false;
	}

	//this->m_fileNameFullPath = this->_pTexInfo->fullPathName;
	//this->m_textureName = this->_pTexInfo->texName;

	// Good to go (valid texture ID and loaded bitmap...
	// Now set the texture...
	//glActiveTexture( textureUnit );	// GL_TEXTURE0, GL_TEXTURE1, etc.
	glBindTexture(GL_TEXTURE_2D, this->m_textureNumber);

	// In case texture is oddly aligned, set the client alignment to 1 byte (default is 4)
	GLint GL_UNPACK_ALIGNMENT_old = 0;
	glGetIntegerv(GL_UNPACK_ALIGNMENT, &GL_UNPACK_ALIGNMENT_old);
	// Set alignment to 1 byte
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexImage2D(GL_TEXTURE_2D,		// target (2D, 3D, etc.)		// OpenGL 2.0 and up
		0,					// MIP map level 
		GL_RGBA,			// internal format
		this->m_numberOfColumns,	// width (pixels)	
		this->m_numberOfRows,		// height (pixels)	
		0,					// border (0 or 1)
		GL_RGB,			// format of pixel data
		GL_UNSIGNED_BYTE,	// type of pixel data
		this->m_p_theImages);	// pointer to data in memory

//	glTexStorage2D( GL_TEXTURE_2D,								// OpenGL 4.2 and up
//		            1, 
//					GL_RGBA8, 
//					this->m_numberOfColumns,					// When reversed the image is correct, but B & W
//					this->m_numberOfRows );
//					//this->m_numberOfColumns, 
//					//this->m_numberOfRows );

	//if (this->bWasThereAnOpenGLError()) { return false; }

	//for ( int index = 0; index != 400; index++ )
	//{
	//	std::cout << (int)this->m_p_theImages[index].redPixel << ", ";
	//	std::cout << (int)this->m_p_theImages[index].greenPixel << ", ";
	//	std::cout << (int)this->m_p_theImages[index].bluePixel << std::endl;
	//}

//	glTextureSubImage2D(								// OpenGL 4.5 and up
//	glTexSubImage2D( GL_TEXTURE_2D,						// OpenGL 2.0 and up
//		             0,		// Level 0
//					 0, 0,	// Offset of 0,0
//					 this->m_numberOfColumns,
//					 this->m_numberOfRows,					// When reversed the image is correct, but B & W
//					 //this->m_numberOfColumns, 
//					 //this->m_numberOfRows,
//					 GL_RGB,			// Pixel data format
//					 GL_UNSIGNED_BYTE,	// Pixel data type  
//					 this->m_p_theImages );

	//if (this->bWasThereAnOpenGLError()) { return false; }

	// Put the pixel store aligment back to what it was
	glPixelStorei(GL_UNPACK_ALIGNMENT, GL_UNPACK_ALIGNMENT_old);


	this->ClearBMP();


	if (bGenerateMIPMap)
	{
		glGenerateMipmap(GL_TEXTURE_2D);		// OpenGL 4.0
	}

	//if ( this->bWasThereAnOpenGLError() )	{ return false;	}


	//glBindTexture(GL_TEXTURE_2D, m_textureNumber);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, /*GL_CLAMP*/ GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, /*GL_CLAMP*/ GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST /*GL_LINEAR*/);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, /*GL_NEAREST*/ GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	//	GLfloat largest_supported_anisotropy;
	//	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largest_supported_anisotropy);
	//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, largest_supported_anisotropy);

		//if ( this->bWasThereAnOpenGLError() )	{ return false;	}

		//this->m_textureUnit = textureUnit;

	this->m_bIs2DTexture = true;

	//glBindTexture(GL_TEXTURE_2D, 0);

	if (this->bWasThereAnOpenGLError()) { return false; }

	return bReturnVal;
}

bool cTextureBMP::LoadTextureFromCPU(const std::string& fileNameFullPath)
{
	// Load the bitmap. Since this is threaded, it won't stop processing

	if (!this->LoadBMP2(fileNameFullPath))
	{
		return false;
	}

	if (this->bWasThereAnOpenGLError()) { return false; }

	// File is loaded and ready to be sent to the GPU
	//this->_pTexInfo->texState = eTexState::LOADED_FROM_FILE;

	return true;
}

bool cTextureBMP::LoadTextureIntoGPU(bool bGenerateMIPMap)
{
	//printf("Copying %s to GPU.\n", this->_pTexInfo->fullPathName.c_str());

	bool bReturnVal = true;

	// Clear any old openGL errors???
	int IHateYou = glGetError();

	// Pick a texture number...
//	GLuint textureNum = 0;
	this->m_textureNumber = 0;
	glGenTextures(1, &(this->m_textureNumber));

	// Worked?
	if ((glGetError() & GL_INVALID_VALUE) == GL_INVALID_VALUE)
	{
		bReturnVal = false;
		return false;
	}

	// Good to go (valid texture ID and loaded bitmap...
	// Now set the texture...
	//glActiveTexture( textureUnit );	// GL_TEXTURE0, GL_TEXTURE1, etc.
	glBindTexture(GL_TEXTURE_2D, this->m_textureNumber);

	// In case texture is oddly aligned, set the client alignment to 1 byte (default is 4)
	GLint GL_UNPACK_ALIGNMENT_old = 0;
	glGetIntegerv(GL_UNPACK_ALIGNMENT, &GL_UNPACK_ALIGNMENT_old);
	// Set alignment to 1 byte
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexImage2D(GL_TEXTURE_2D,	// target (2D, 3D, etc.)		// OpenGL 2.0 and up
		0,					// MIP map level 
		GL_RGBA,			// internal format
		this->m_numberOfColumns,	// width (pixels)	
		this->m_numberOfRows,		// height (pixels)	
		0,					// border (0 or 1)
		GL_RGB,			// format of pixel data
		GL_UNSIGNED_BYTE,	// type of pixel data
		this->m_p_theImages);	// pointer to data in memory

//	glTexStorage2D( GL_TEXTURE_2D,								// OpenGL 4.2 and up
//		            1, 
//					GL_RGBA8, 
//					this->m_numberOfColumns,					// When reversed the image is correct, but B & W
//					this->m_numberOfRows );
//					//this->m_numberOfColumns, 
//					//this->m_numberOfRows );

	if (this->bWasThereAnOpenGLError()) { return false; }

	//	glTextureSubImage2D(								// OpenGL 4.5 and up
	//	glTexSubImage2D( GL_TEXTURE_2D,						// OpenGL 2.0 and up
	//		             0,		// Level 0
	//					 0, 0,	// Offset of 0,0
	//					 this->m_numberOfColumns,
	//					 this->m_numberOfRows,					// When reversed the image is correct, but B & W
	//					 //this->m_numberOfColumns, 
	//					 //this->m_numberOfRows,
	//					 GL_RGB,			// Pixel data format
	//					 GL_UNSIGNED_BYTE,	// Pixel data type  
	//					 this->m_p_theImages );

	//if (this->bWasThereAnOpenGLError()) { return false; }

	// Put the pixel store aligment back to what it was
	glPixelStorei(GL_UNPACK_ALIGNMENT, GL_UNPACK_ALIGNMENT_old);


	this->ClearBMP();


	if (bGenerateMIPMap)
	{
		glGenerateMipmap(GL_TEXTURE_2D);		// OpenGL 4.0
	}

	//if ( this->bWasThereAnOpenGLError() )	{ return false;	}


	//glBindTexture(GL_TEXTURE_2D, m_textureNumber);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, /*GL_CLAMP*/ GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, /*GL_CLAMP*/ GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST /*GL_LINEAR*/);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, /*GL_NEAREST*/ GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	this->m_bIs2DTexture = true;


	// Set the status to indicate that it's in the GPU
	this->_pTexInfo->_texState = eTexState::LOADED_IN_GPU;

	std::cout << this->getFileNameFullPath() << " is in GPU" << std::endl;
	std::cout.flush();

	//if (this->bWasThereAnOpenGLError()) { return false; }

	return true;
}

bool cTextureBMP::CreateNewCubeTextureFromBMPFiles(std::string cubeMapName,
	std::string posX_fileName, std::string negX_fileName,
	std::string posY_fileName, std::string negY_fileName,
	std::string posZ_fileName, std::string negZ_fileName,
	/*GLenum textureUnit, */bool bIsSeamless,
	GLenum &errorEnum, std::string &errorString, std::string &errorDetails)
{

	bool bReturnVal = true;

	// Do the magic...

	// Pick a texture number...
	//GLuint textureNum = 0;
	glGenTextures(1, &(this->m_textureNumber));
	// Worked?
	if ((glGetError() & GL_INVALID_VALUE) == GL_INVALID_VALUE)
	{
		bReturnVal = false;
		return false;
	}

	//
	//glEnable(GL_TEXTURE_2D);
	//glActiveTexture( textureUnit );	// GL_TEXTURE0, GL_TEXTURE1, etc.
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->m_textureNumber);

	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_GENERATE_MIPMAP, GL_TRUE);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE /*GL_REPEAT*/);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE /*GL_REPEAT*/);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE /*GL_REPEAT*/);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// IF YOU DON'T GENERATE MIP MAPS, then you can use LINEAR filtering
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );

	if (bIsSeamless)
	{
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);	// OpenGL 4.1, maybe
	}

	if (this->bWasThereAnOpenGLError(errorEnum, errorString, errorDetails)) { return false; }

	// Positive X image...
	// Assume all the images are the same size. If not, then it will screw up
	if (this->LoadBMP2(posX_fileName))
	{
		glTexStorage2D(GL_TEXTURE_CUBE_MAP,
			10, // Mipmap levels
			GL_RGBA8,	// Internal format
			this->m_numberOfColumns,	// width (pixels)
			this->m_numberOfRows);		// height (pixels)

		if (this->bWasThereAnOpenGLError(errorEnum, errorString, errorDetails)) { return false; }
	}
	else
	{
		this->m_lastErrorNum = cTextureBMP::ERORR_FILE_WONT_OPEN;
		return false;
	}

	// Positive X image...
	glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		0,   // Level
		0, 0, // Offset
		this->m_numberOfColumns,	// width
		this->m_numberOfRows,		// height
		GL_RGB,
		GL_UNSIGNED_BYTE,
		this->m_p_theImages);
	this->ClearBMP();
	if (this->bWasThereAnOpenGLError(errorEnum, errorString, errorDetails)) { return false; }


	// Negative X image...
	if (this->LoadBMP2(negX_fileName))
	{
		glTexSubImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, 0, 0, this->m_numberOfColumns, this->m_numberOfRows, GL_RGB, GL_UNSIGNED_BYTE, this->m_p_theImages);
		this->ClearBMP();
		if (this->bWasThereAnOpenGLError(errorEnum, errorString, errorDetails)) { return false; }
	}
	else
	{
		this->m_lastErrorNum = cTextureBMP::ERORR_FILE_WONT_OPEN;
		return false;
	}

	// Positive Y image...
	if (this->LoadBMP2(posY_fileName))
	{
		glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, 0, 0, this->m_numberOfColumns, this->m_numberOfRows, GL_RGB, GL_UNSIGNED_BYTE, this->m_p_theImages);
		this->ClearBMP();
		if (this->bWasThereAnOpenGLError(errorEnum, errorString, errorDetails)) { return false; }
	}
	else
	{
		this->m_lastErrorNum = cTextureBMP::ERORR_FILE_WONT_OPEN;
		return false;
	}

	// Negative Y image...
	if (this->LoadBMP2(negY_fileName))
	{
		glTexSubImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, 0, 0, this->m_numberOfColumns, this->m_numberOfRows, GL_RGB, GL_UNSIGNED_BYTE, this->m_p_theImages);
		this->ClearBMP();
		if (this->bWasThereAnOpenGLError(errorEnum, errorString, errorDetails)) { return false; }
	}
	else
	{
		this->m_lastErrorNum = cTextureBMP::ERORR_FILE_WONT_OPEN;
		return false;
	}

	// Positive Z image...
	if (this->LoadBMP2(posZ_fileName))
	{
		glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, 0, 0, this->m_numberOfColumns, this->m_numberOfRows, GL_RGB, GL_UNSIGNED_BYTE, this->m_p_theImages);
		this->ClearBMP();
		if (this->bWasThereAnOpenGLError(errorEnum, errorString, errorDetails)) { return false; }
	}
	else
	{
		this->m_lastErrorNum = cTextureBMP::ERORR_FILE_WONT_OPEN;
		return false;
	}

	// Negative Z image...
	if (this->LoadBMP2(negZ_fileName))
	{
		glTexSubImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, 0, 0, this->m_numberOfColumns, this->m_numberOfRows, GL_RGB, GL_UNSIGNED_BYTE, this->m_p_theImages);
		this->ClearBMP();
		if (this->bWasThereAnOpenGLError(errorEnum, errorString, errorDetails)) { return false; }
	}
	else
	{
		this->m_lastErrorNum = cTextureBMP::ERORR_FILE_WONT_OPEN;
		return false;
	}

	//this->m_textureName = cubeMapName;

	//this->m_textureUnit = textureUnit;

	this->m_bIsCubeMap = true;

	// End of Do the magic...

	return bReturnVal;
}

bool cTextureBMP::ClearBMP(void)
{
	delete[] this->m_p_theImages;
	this->m_p_theImages = 0;
	return true;
}

// Some of this taken from: http://www.allegro.cc/forums/thread/601551
bool cTextureBMP::CreateNewBMPFromCurrentTexture(int mipMapLevel)
{
	GLint textureWidth, textureHeight;

	glGetTexLevelParameteriv(GL_TEXTURE_2D, mipMapLevel, GL_TEXTURE_WIDTH, &textureWidth);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, mipMapLevel, GL_TEXTURE_HEIGHT, &textureHeight);

	int numberOfPixels = textureWidth * textureHeight;

	//GLubyte *buffer = (GLubyte *)malloc(textureSize);
	//GLubyte *buffer = new GLubyte[ textureSize ];
	this->m_p_theImages = new c24BitBMPPixel[numberOfPixels];

	//GLubyte buffer[ 5000000 ] = {0};
	//
	glGetTexImage(GL_TEXTURE_2D, mipMapLevel, GL_RGB, GL_UNSIGNED_BYTE, this->m_p_theImages);

	// Now fill in all the BMP stuff...
	this->m_reserved1 = this->m_reserved2 = 0;
	this->m_offsetInBits = 54;	// (always 54);
	this->m_headerSize = 40;	// (always 40);
	this->m_numberOfPlanes = 1;	// (always 1);
	this->m_bitPerPixel = 24;	// (always 24);

	this->m_compressionMode = 0;	// (0);
	this->m_imageSizeInBytes = numberOfPixels * sizeof(c24BitBMPPixel);
	this->m_PixelsPerMeterX = 2880;		// Not sure why...
	this->m_PixelsPerMeterY = 2880;		// Not sure why...
	this->m_numberOfLookUpTableEntries = 0;	// Always seem to be zero.
	this->m_numberOfImportantColours = 0;	// Always seem to be zero.

	this->m_numberOfColumns = textureHeight;
	this->m_numberOfRows = textureWidth;

	long bytesPerRow = ((3 * this->m_numberOfRows + 3) / 4) * 4;
	this->m_FileSize = this->m_numberOfRows * bytesPerRow + 54;

	//TODO: The rest of this mess...

	return true;
}


bool cTextureBMP::SaveBMP(std::string fileName)
{
	std::ofstream theFile;
	theFile.open(fileName.c_str(), std::ios_base::binary);
	if (!theFile.is_open())
	{
		return false;
	}

	//unsigned char *img = NULL;
	//int filesize = 54 + 3*w*h


	std::cout << "Writing texture to BMP" << std::endl;
	theFile << 'B' << 'M';
	this->WriteAsUnsignedLong(this->m_FileSize, theFile);	//	theFile << this->m_FileSize;
	this->WriteAsUnsignedShort(this->m_reserved1, theFile);			// 0
	this->WriteAsUnsignedShort(this->m_reserved2, theFile);			// 0
	this->WriteAsUnsignedLong(this->m_offsetInBits, theFile);			//54
	this->WriteAsUnsignedLong(this->m_headerSize, theFile);			//40
	this->WriteAsUnsignedLong(this->m_numberOfRows, theFile);
	this->WriteAsUnsignedLong(this->m_numberOfColumns, theFile);
	this->WriteAsUnsignedShort(this->m_numberOfPlanes, theFile);		// 1
	this->WriteAsUnsignedShort(this->m_bitPerPixel, theFile);			// 24
	this->WriteAsUnsignedLong(this->m_compressionMode, theFile);
	this->WriteAsUnsignedLong(this->m_imageSizeInBytes, theFile);
	this->WriteAsUnsignedLong(this->m_PixelsPerMeterX, theFile);
	this->WriteAsUnsignedLong(this->m_PixelsPerMeterY, theFile);
	this->WriteAsUnsignedLong(this->m_numberOfLookUpTableEntries, theFile);
	this->WriteAsUnsignedLong(this->m_numberOfImportantColours, theFile);


	long bytesPerRow = ((3 * this->m_numberOfRows + 3) / 4) * 4;
	long numberOfPaddingBytes = bytesPerRow - 3 * this->m_numberOfColumns;

	// Write the bitmap data to the file... 
	long bytesWritten = 0;
	long totalBytesInFile = this->m_numberOfRows * this->m_numberOfColumns;
	long pixelCount = 0;

	for (unsigned long row = 0; row < this->m_numberOfRows; row++)
	{
#ifdef DEBUG_MODE_BITMAPREAD
		std::cout << "Row:" << row << std::endl;
#endif
		for (unsigned long col = 0; col < this->m_numberOfColumns; col++)
		{
			char thered = static_cast<unsigned char>(this->m_p_theImages[pixelCount].redPixel);
			char thegreen = static_cast<unsigned char>(this->m_p_theImages[pixelCount].greenPixel);
			char theblue = static_cast<unsigned char>(this->m_p_theImages[pixelCount].bluePixel);
			theFile.put(theblue);
			theFile.put(thegreen);
			theFile.put(thered);

			pixelCount++;
			bytesWritten += 3;	// Add three bytes
		}
#ifdef DEBUG_MODE_BITMAPREAD
		std::cout << std::endl;
#endif
		// We are at the end of the row and there will 
		//	still be bytes padding up to a multiple of 4...
		// Write thes padding bytes here...
		for (int discardedBytes = 0; discardedBytes < numberOfPaddingBytes; discardedBytes++)
		{
			char tempChar = 0;
			theFile.put(tempChar);
		}
	}

	theFile.close();
	return true;
}

// These are used to read the data from a file (LoadBMP2 method).
// * index is incremented appropriate amount (to next char)
// * no bounds checking for array
char cTextureBMP::ReadNextChar(char* data, unsigned long &index)
{
	char returnVal = static_cast<unsigned char>(data[index]);
	index++;
	return returnVal;
}

unsigned short cTextureBMP::ReadNextUnsignedShort(char* data, unsigned long &index)
{
	unsigned short usTheReturnVal = 0;
	usTheReturnVal = this->ShiftAndAdd_Byte_to_UShort(usTheReturnVal, static_cast<unsigned char>(data[index]), 0);
	index++;
	usTheReturnVal = this->ShiftAndAdd_Byte_to_UShort(usTheReturnVal, static_cast<unsigned char>(data[index]), 1);
	index++;
	return usTheReturnVal;
}

unsigned long cTextureBMP::ReadNextUnsignedLong(char* data, unsigned long &index)
{
	unsigned long ulTheReturnVal = 0;
	// Read four bytes, but return them in reverse order.
	ulTheReturnVal = this->ShiftAndAdd_Byte_to_ULong(ulTheReturnVal, static_cast<unsigned char>(data[index]), 0);
	index++;
	ulTheReturnVal = this->ShiftAndAdd_Byte_to_ULong(ulTheReturnVal, static_cast<unsigned char>(data[index]), 1);
	index++;
	ulTheReturnVal = this->ShiftAndAdd_Byte_to_ULong(ulTheReturnVal, static_cast<unsigned char>(data[index]), 2);
	index++;
	ulTheReturnVal = this->ShiftAndAdd_Byte_to_ULong(ulTheReturnVal, static_cast<unsigned char>(data[index]), 3);
	index++;
	return ulTheReturnVal;
}

// Loads it in one "go" instead of streaming it
bool cTextureBMP::LoadBMP2(std::string fileName)
{
	std::ifstream theFile;
	if (this->m_bHave_cout_output)
	{
		std::cout << "Reading texture file: " << fileName;
	}
	theFile.open(fileName.c_str(), std::ios_base::binary);
	if (!theFile)
	{
		this->m_lastErrorNum = cTextureBMP::ERORR_FILE_WONT_OPEN;
		return false;
	}
	// Scan to the end to get the size
	theFile.seekg(0, std::ios::end);
	std::ios::pos_type fileSize = theFile.tellg();
	// Return to start
	theFile.seekg(0, std::ios::beg);

	char* pRawData = new char[static_cast<unsigned int>(fileSize)];

	theFile.read(pRawData, fileSize);
	theFile.close();

	// Now go through and decode the BMP file.
	unsigned long curIndex = 0;
	char letter1 = this->ReadNextChar(pRawData, curIndex);
	char letter2 = this->ReadNextChar(pRawData, curIndex);
	if ((letter1 != 'B') && (letter2 != 'M'))
	{
		this->m_lastErrorNum = cTextureBMP::ERROR_NOT_A_BMP_FILE;
		delete[] pRawData;
		return false;
	}
	// File is BMP
	this->m_FileSize = this->ReadNextUnsignedLong(pRawData, curIndex);
	this->m_reserved1 = this->ReadNextUnsignedShort(pRawData, curIndex);
	this->m_reserved2 = this->ReadNextUnsignedShort(pRawData, curIndex);
	this->m_offsetInBits = this->ReadNextUnsignedLong(pRawData, curIndex);
	this->m_headerSize = this->ReadNextUnsignedLong(pRawData, curIndex);
	this->m_numberOfColumns = this->ReadNextUnsignedLong(pRawData, curIndex);
	this->m_Width_or_Columns = this->m_OriginalWidth = this->m_numberOfColumns;
	this->m_numberOfRows = this->ReadNextUnsignedLong(pRawData, curIndex);
	this->m_Height_or_Rows = this->m_OriginalHeight = this->m_numberOfRows;
	this->m_numberOfPlanes = this->ReadNextUnsignedShort(pRawData, curIndex);
	this->m_bitPerPixel = this->ReadNextUnsignedShort(pRawData, curIndex);
	// Is is a 24 bit bitmap?
	if (this->m_bitPerPixel != 24)
	{
		this->m_lastErrorNum = cTextureBMP::ERROR_NOT_A_24_BIT_BITMAP;
		delete[] pRawData;
		return false;
	}

	this->m_compressionMode = this->ReadNextUnsignedLong(pRawData, curIndex);
	this->m_imageSizeInBytes = this->ReadNextUnsignedLong(pRawData, curIndex);
	this->m_PixelsPerMeterX = this->ReadNextUnsignedLong(pRawData, curIndex);
	this->m_PixelsPerMeterY = this->ReadNextUnsignedLong(pRawData, curIndex);
	this->m_numberOfLookUpTableEntries = this->ReadNextUnsignedLong(pRawData, curIndex);
	this->m_numberOfImportantColours = this->ReadNextUnsignedLong(pRawData, curIndex);

	// 24 bits is 3 bytes per row
	long bytesPerRow = this->m_numberOfColumns * 3;

	if ((this->m_numberOfColumns % 4) != 0)
	{
		bytesPerRow = (bytesPerRow / 4) * 4 + 4;
	}
	//	long bytesPerRow = (int)ceil( (3.0f * (float)this->m_numberOfColumns) / 4.0f) * 4;

	//	long bytesPerRow = ((3 * this->m_numberOfColumns + 3) / 4) * 4;
	//	long bytesPerRow = ((3 * this->m_numberOfRows + 3) / 4) * 4;
	long numberOfPaddingBytes = bytesPerRow - 3 * this->m_numberOfColumns;

	// Allocate enough space...
	this->m_p_theImages = new c24BitBMPPixel[this->m_numberOfColumns * this->m_numberOfRows];

	// Did we run out of memory?
	if (!this->m_p_theImages)
	{
		this->m_lastErrorNum = cTextureBMP::ERROR_NOT_ENOUGHT_MEMORY_FOR_BITMAP;
		delete[] pRawData;
		return false;
	}


	// Read the bitmap into memory...
	unsigned long bytesRead = 0;
	unsigned long totalBytesInFile = bytesPerRow * this->m_numberOfColumns;
	//	unsigned long totalBytesInFile = this->m_numberOfRows * this->m_numberOfColumns;
	unsigned long pixelCount = 0;

	for (unsigned long row = 0; row < m_numberOfRows; row++)
	{
		for (unsigned long col = 0; col < m_numberOfColumns; col++)
		{
			char theblue = this->ReadNextChar(pRawData, curIndex);
			char thegreen = this->ReadNextChar(pRawData, curIndex);
			char thered = this->ReadNextChar(pRawData, curIndex);

			this->m_p_theImages[pixelCount].redPixel = static_cast<unsigned char>(thered);
			this->m_p_theImages[pixelCount].greenPixel = static_cast<unsigned char>(thegreen);
			this->m_p_theImages[pixelCount].bluePixel = static_cast<unsigned char>(theblue);

			//this->m_p_theImages32[pixelCount].alphaPixel = 255;

#ifdef DEBUG_MODE_BITMAPREAD
			std::cout << (static_cast<unsigned int>(thered) & 0x00FF) << ", ";
			std::cout << (static_cast<unsigned int>(thegreen) & 0x00FF) << ", ";
			std::cout << (static_cast<unsigned int>(theblue) & 0x00FF) << "\t";
#endif
			pixelCount++;
			bytesRead += 3;	// Add three bytes
		}// for (unsigned long col
#ifdef DEBUG_MODE_BITMAPREAD
		std::cout << std::endl;
#endif
		// We are at the end of the row and there will 
		//	still be bytes padding up to a multiple of 4...
		// Read and discard these...

		for (int discardedBytes = 0; discardedBytes < numberOfPaddingBytes; discardedBytes++)
		{
			char tempChar = this->ReadNextChar(pRawData, curIndex);
			bytesRead += 1;	// Add a byte
		}
		// How much have we read?
		// NOTE: This really isn't percent, but it does the job of 
		//	printing out fewer dots.
		int percentRead = static_cast<int>((static_cast<double>(bytesRead) / static_cast<double>(totalBytesInFile)) * 100);
		if ((percentRead % 25) == 0)
		{
			if (this->m_bHave_cout_output)
			{
				std::cout << ".";
			}
		}
	}// for (unsigned long row...

	if (this->m_bHave_cout_output)
	{
		std::cout << "complete." << std::endl;
	}
	// BMP file is read....

	// Delete the raw data array!
	delete[] pRawData;

	return true;
}


// __  __       _       _____         _                     _        _   _
//|  \/  | __ _| | ____|_   _|____  _| |_ _   _ _ __ ___   / \   ___| |_(_)_   _____
//| |\/| |/ _` | |/ / _ \| |/ _ \ \/ / __| | | | '__/ _ \ / _ \ / __| __| \ \ / / _ \
//| |  | | (_| |   <  __/| |  __/>  <| |_| |_| | | |  __// ___ \ (__| |_| |\ V /  __/
//|_|  |_|\__,_|_|\_\___||_|\___/_/\_\\__|\__,_|_|  \___/_/   \_\___|\__|_| \_/ \___|
//
// Returns true if texture exists and can be applied
bool cTextureBMP::MakeTextureActive(void)
{
	bool bReturnVal = true;
	// Check if texturing is enabled?
	GLboolean bOldTextureMode = glIsEnabled(GL_TEXTURE_2D);
	// Set the texture mode...
	if (bOldTextureMode != GL_FALSE)
	{
		glEnable(GL_TEXTURE_2D);
	}
	// No implementation yet.
	glBindTexture(GL_TEXTURE_2D, m_textureNumber);

	// Restore the initial texture mode...
	if (bOldTextureMode != GL_TRUE)
	{
		glDisable(GL_TEXTURE_2D);
	}
	return bReturnVal;
}
// This allows you to set the GL_MODULATE, GL_DECAL, GL_BLEND, or GL_REPLACE modes
//	in OpenGL as a parameter
bool cTextureBMP::SetTextureEnvironmentMode(GLint textureReplacementMode)
{
	bool bReturnVal = true;
	// Is this a valid replacementMode?
	if ((textureReplacementMode != GL_MODULATE) &&
		(textureReplacementMode != GL_DECAL) &&
		(textureReplacementMode != GL_BLEND) &&
		(textureReplacementMode != GL_REPLACE))
	{
		textureReplacementMode = GL_REPLACE;
	}
#ifdef DEBUG_MODE_TEXTUREREPLACEMENT
	switch (textureReplacementMode)
	{
	case GL_MODULATE:
		std::cout << "Texture replacement mode is GL_MODULATE" << std::endl;
		break;
	case GL_DECAL:
		std::cout << "Texture replacement mode is GL_DECAL" << std::endl;
		break;
	case GL_BLEND:
		std::cout << "Texture replacement mode is GL_BLEND" << std::endl;
		break;
	case GL_REPLACE:
		std::cout << "Texture replacement mode is GL_REPLACE" << std::endl;
		break;
	default:
		std::cout << "Unknown Texture repalcement mode" << std::endl;
		break;
	}
#endif
	// Set the environment mode...
	// First, get the mode...
	glGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &m_oldTextureEnvironmentMode);
	// Set the new mode...
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, textureReplacementMode);
	return bReturnVal;
}

bool cTextureBMP::RestoreOldTextureEnvironmentMode(void)
{
	bool bReturnVal = true;
	// Restore the old mode...
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, m_oldTextureEnvironmentMode);
	return bReturnVal;
}


// ____          _         ____  _ _
//|  _ \ ___ ___(_)_______| __ )(_) |_ _ __ ___   __ _ _ __
//| |_) / _ Y __| |_  / _ \  _ \| | __| '_ ` _ \ / _` | '_ \
//|  _ <  __|__ \ |/ /  __/ |_) | | |_| | | | | | (_| | |_) |
//|_| \_\___|___/_/___\___|____/|_|\__|_| |_| |_|\__,_| .__/
//                                                    |_|
// This filters a bitmap into a new size (likely powers of 2 in width or height)
bool cTextureBMP::ResizeBitmap(int DesiredHeight, int DesiredWidth)
{
	bool bReturnVal = true;
	// No implementation yet.
	return bReturnVal;
}

// This is for error handling.
//
int cTextureBMP::GetLastErrorNumber(void)
{
	return m_lastErrorNum;
}
//
std::string cTextureBMP::DecodeLastError(int errorNum)
{
	std::string lastErrorString;
	switch (m_lastErrorNum)
	{
	case cTextureBMP::ERORR_FILE_WONT_OPEN:
		lastErrorString = "ERROR: File does not exist or would not open";
		break;
	case cTextureBMP::ERROR_NOT_A_BMP_FILE:
		lastErrorString = "ERROR: File is not a BMP file - \'BM\' tag not present";
		break;
	case cTextureBMP::ERROR_NOT_A_24_BIT_BITMAP:
		lastErrorString = "ERROR: Not a 24 Bit bitmap format";
		break;
	case cTextureBMP::ERROR_NOT_ENOUGHT_MEMORY_FOR_BITMAP:
		lastErrorString = "ERROR: Not enought memory to hold bitmap";
		break;
	case cTextureBMP::ERROR_NO_ERROR:
	default:
		lastErrorString = "No error";
		break;
	}
	return lastErrorString;
}


// These functions shift numbers by one, two, and three bytes.
unsigned long cTextureBMP::ShiftAndAdd_Byte_to_ULong(unsigned long theULong, char theByte, int bytesToShift)
{
	// Too many bytes to shift (there are only 4 bytes in a long, so max is 3)?
	if (bytesToShift > 3)
	{
		bytesToShift = 0;
	}
	unsigned short bitsToShift = bytesToShift * 8;	// 8 bits / byte.
	// Now, convert the uChar to a uLong, 
	//	shift it the number of bits,
	//	then or it (add it) to the long
	unsigned char uByte = theByte;
	theULong = theULong | (static_cast<unsigned long>(uByte) << bitsToShift);
	return theULong;
}

unsigned short cTextureBMP::ShiftAndAdd_Byte_to_UShort(unsigned short theUShort, char theByte, int bytesToShift)
{
	// Too many bytes to shift (there is only 2 bytes in a long, so max is 1)?
	if (bytesToShift > 1)
	{
		bytesToShift = 0;
	}
	unsigned short bitsToShift = bytesToShift * 8;	// 8 bits / byte.
	// Now, convert the uChar to a uLong, 
	//	shift it the number of bits,
	//	then or it (add it) to the long
	unsigned char uByte = theByte;
	theUShort = theUShort | (static_cast<unsigned long>(uByte) << bitsToShift);
	return theUShort;
}


// These are used to read the data from a file.
unsigned long cTextureBMP::ReadAnUnsignedLong(std::ifstream& theStream)
{
	unsigned long ulTheReturnVal = 0;
	// Read four bytes, but return them in reverse order.
	char TheByte = 0;
	theStream.get(TheByte);
	ulTheReturnVal = ShiftAndAdd_Byte_to_ULong(ulTheReturnVal, static_cast<unsigned char>(TheByte), 0);
	theStream.get(TheByte);
	ulTheReturnVal = ShiftAndAdd_Byte_to_ULong(ulTheReturnVal, static_cast<unsigned char>(TheByte), 1);
	theStream.get(TheByte);
	ulTheReturnVal = ShiftAndAdd_Byte_to_ULong(ulTheReturnVal, static_cast<unsigned char>(TheByte), 2);
	theStream.get(TheByte);
	ulTheReturnVal = ShiftAndAdd_Byte_to_ULong(ulTheReturnVal, static_cast<unsigned char>(TheByte), 3);
	return ulTheReturnVal;
}





void cTextureBMP::WriteAsUnsignedShort(unsigned short value, std::ofstream& theStream)
{
	// AB --> BA
	char TheByte[2] = { 0 };
	TheByte[1] = static_cast<char>((value & 0xFF00) >> 8);
	TheByte[0] = static_cast<char>(value & 0x00FF);

	theStream.put(TheByte[0]);
	theStream.put(TheByte[1]);
}

void cTextureBMP::WriteAsUnsignedLong(unsigned long value, std::ofstream& theStream)
{
	// ABCD --> DCBA
	char TheByte[4] = { 0 };
	TheByte[3] = static_cast<char>((value & 0xFF000000) >> 24);
	TheByte[2] = static_cast<char>((value & 0x00FF0000) >> 16);
	TheByte[1] = static_cast<char>((value & 0x0000FF00) >> 8);
	TheByte[0] = static_cast<char>(value & 0x000000FF);

	theStream.put(TheByte[0]);
	theStream.put(TheByte[1]);
	theStream.put(TheByte[2]);
	theStream.put(TheByte[3]);
}

unsigned short cTextureBMP::ReadAnUnsignedShort(std::ifstream& theStream)
{
	unsigned short usTheReturnVal = 0;
	// Read two bytes, but reverse the order.
	char TheByte;
	theStream.get(TheByte);
	usTheReturnVal = ShiftAndAdd_Byte_to_UShort(usTheReturnVal, TheByte, 0);
	theStream.get(TheByte);
	usTheReturnVal = ShiftAndAdd_Byte_to_UShort(usTheReturnVal, TheByte, 1);
	return usTheReturnVal;
}

char cTextureBMP::ReadAByte(std::ifstream& theStream)
{
	char theByte = 0;
	theStream.get(theByte);
	return theByte;
}

// ____  _           _            _____         _                     _        ____           _            ____  _ _
//|  _ \(_)___ _ __ | | __ _ _   |_   _|____  _| |_ _   _ _ __ ___   / \   ___|  _ \ __ _ ___| |_ ___ _ __| __ )(_) |_ _ __ ___   __ _ _ __
//| | | | / __| '_ \| |/ _` | | | || |/ _ \ \/ / __| | | | '__/ _ \ / _ \ / __| |_) / _` / __| __/ _ \ '__|  _ \| | __| '_ ` _ \ / _` | '_ \
//| |_| | \__ \ |_) | | (_| | |_| || |  __/>  <| |_| |_| | | |  __// ___ \\__ \  _ < (_| \__ \ ||  __/ |  | |_) | | |_| | | | | | (_| | |_) |
//|____/|_|___/ .__/|_|\__,_|\__, ||_|\___/_/\_\\__|\__,_|_|  \___/_/   \_\___/_| \_\__,_|___/\__\___|_|  |____/|_|\__|_| |_| |_|\__,_| .__/
//            |_|            |___/                                                                                                    |_|
// This takes the bitmap that is read and displays it on the screen
//	as a raster image. i.e. it doesn't go through the transformations, 
//	etc., it just plops it on the screen at the current raster 
//	location. This is like bitmap fonts.
void cTextureBMP::DisplayTextureAsRasterBitmap(GLfloat xOrigin, GLfloat yOrigin, GLfloat xMove, GLfloat yMove)
{
	// convert the array of structures into a bitmap array
	GLubyte* p_theBitmap = new GLubyte[m_Width_or_Columns * m_Height_or_Rows * 4];	// 4 bytes / pixel
	unsigned long arrayIndex = 0;
#ifdef DEBUG_MODE_BITMAPDISPLAY
	static bool bDisplayedOnce = false;
#endif
	// copy the data into array...
	long pixeloffset = 0;
	for (unsigned long row = 0; row < this->m_Height_or_Rows; row++)
	{
#ifdef DEBUG_MODE_BITMAPDISPLAY
		if (!bDisplayedOnce)
		{
			std::cout << "Row = " << row << std::endl;
		}
#endif
		for (unsigned long column = 0; column < this->m_Width_or_Columns; column++)
		{
			arrayIndex = (row * m_Width_or_Columns) + column;
			double theRow = row;
			double theCol = column;
			p_theBitmap[arrayIndex] = m_p_theImages[pixeloffset].redPixel;
			p_theBitmap[arrayIndex + 1] = m_p_theImages[pixeloffset].greenPixel;
			p_theBitmap[arrayIndex + 2] = m_p_theImages[pixeloffset].bluePixel;
			p_theBitmap[arrayIndex + 3] = 0;
			pixeloffset++;

#ifdef DEBUG_MODE_BITMAPDISPLAY
			if (!bDisplayedOnce)
			{
				std::cout << "(R:" << static_cast<unsigned int>(p_theBitmap[arrayIndex]) << ",";
				std::cout << "G:" << static_cast<unsigned int>(p_theBitmap[arrayIndex + 1]) << ",";
				std::cout << "B:" << static_cast<unsigned int>(p_theBitmap[arrayIndex + 2]) << ",";
				std::cout << "A:" << static_cast<unsigned int>(p_theBitmap[arrayIndex + 3]);
				std::cout << ")" << std::endl;
			}
#endif
			arrayIndex++;
		}
	}
#ifdef DEBUG_MODE_BITMAPDISPLAY
	if (!bDisplayedOnce)
	{
		bDisplayedOnce = true;
	}
#endif

	// Draw the image using the glDrawPixels instead of bitmap...
	glRasterPos2i(0, 0);
	glPixelZoom(10, 10);
	glDrawPixels(m_Width_or_Columns, m_Height_or_Rows, GL_BGRA, GL_UNSIGNED_BYTE, p_theBitmap);

	//glBitmap(m_Width, m_Height, xOrigin, yOrigin, xMove, yMove, p_theBitmap);
	delete[] p_theBitmap;
	return;
}

bool cTextureBMP::bWasThereAnOpenGLError(void)
{
	GLenum errorEnum = GL_NO_ERROR;
	std::string errorString;
	std::string errorDetails;
	return this->bWasThereAnOpenGLError(errorEnum, errorString, errorDetails);
}

bool cTextureBMP::bWasThereAnOpenGLError(GLenum &errorEnum)
{
	std::string errorString;
	std::string errorDetails;
	return this->bWasThereAnOpenGLError(errorEnum, errorString, errorDetails);
}

bool cTextureBMP::bWasThereAnOpenGLError(GLenum &errorEnum, std::string &errorString)
{
	std::string errorDetails;
	return this->bWasThereAnOpenGLError(errorEnum, errorString, errorDetails);
}

// Reutn false if everything is OK
bool cTextureBMP::bWasThereAnOpenGLError(GLenum &errorEnum, std::string &errorString, std::string &errorDetails)
{
	errorEnum = glGetError();
	// from: https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glGetError.xml
	switch (errorEnum)
	{
	case GL_NO_ERROR:
		errorString = "GL_NO_ERROR";
		errorDetails = "GL_NO_ERROR: No error has been recorded.";
		/******************/
		return false;
		/******************/
		break;
	case GL_INVALID_ENUM:
		errorString = "GL_INVALID_ENUM";
		errorDetails = "GL_INVALID_ENUM: An unacceptable value is specified for an enumerated argument.";
		break;
	case GL_INVALID_VALUE:
		errorString = "GL_INVALID_VALUE";
		errorDetails = "GL_INVALID_VALUE: A numeric argument is out of range.";
		break;
	case GL_INVALID_OPERATION:
		errorString = "GL_INVALID_OPERATION";
		errorDetails = "GL_INVALID_OPERATION: The specified operation is not allowed in the current state.";
		break;
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		errorString = "GL_INVALID_FRAMEBUFFER_OPERATION";
		errorDetails = "GL_INVALID_FRAMEBUFFER_OPERATION: The framebuffer object is not complete.";
		break;
	case GL_OUT_OF_MEMORY:
		errorString = "GL_OUT_OF_MEMORY";
		errorDetails = "GL_OUT_OF_MEMORY: There is not enough memory left to execute the command.";
		break;
	case GL_STACK_UNDERFLOW:
		errorString = "GL_STACK_UNDERFLOW";
		errorDetails = "GL_STACK_UNDERFLOW: An attempt has been made to perform an operation that would cause an internal stack to underflow.";
		break;
	case GL_STACK_OVERFLOW:
		errorString = "GL_STACK_OVERFLOW";
		errorDetails = "GL_STACK_OVERFLOW: An attempt has been made to perform an operation that would cause an internal stack to overflow.";
		break;
	default:
		errorString = "Unknown error";
		errorDetails = "Unknown error";
		break;
	};

	return true;	// There -WAS- an error
}

//GLenum CTextureFromBMP::getTextureUnit(void)
//{
//	return this->m_textureUnit;
//}

bool cTextureBMP::getIsCubeMap(void)
{
	return this->m_bIsCubeMap;
}

bool cTextureBMP::getIs2DTexture(void)
{
	return this->m_bIs2DTexture;
}

// The actual image information
//C24BitBMPpixel m_p_theImages[];	
unsigned long cTextureBMP::GetHeightInRows(void)
{
	return m_Height_or_Rows;
}

unsigned long cTextureBMP::GetWidthInColumns(void)
{
	return m_Width_or_Columns;
}

unsigned long cTextureBMP::GetOriginalHeightInRows(void)
{
	return m_OriginalHeight;
}

unsigned long cTextureBMP::GetOriginalWidthInColumns(void)
{
	return m_OriginalWidth;
}

unsigned long cTextureBMP::GetFileSize(void)
{
	return m_FileSize;
}

unsigned long cTextureBMP::GetFileSizeInBytes(void)
{
	return m_FileSize;
}

unsigned short cTextureBMP::GetReserved1(void)
{
	return m_reserved1;
}

unsigned short cTextureBMP::GetReserved2(void)
{
	return m_reserved2;
}

unsigned long cTextureBMP::GetImageSizeInBytes(void)
{
	return m_imageSizeInBytes;
}

unsigned long cTextureBMP::GetBitsPerPixel(void)
{
	return m_bitPerPixel;
}

unsigned long cTextureBMP::GetOffsetInBits(void)
{
	return m_offsetInBits;
}

unsigned long cTextureBMP::GetCompressionMode(void)
{
	return m_compressionMode;
}

unsigned long cTextureBMP::GetPixelsPerMeterX(void)
{
	return m_PixelsPerMeterX;
}

unsigned long cTextureBMP::GetPixelsPerMeterY(void)
{
	return m_PixelsPerMeterY;
}

unsigned long cTextureBMP::GetNumberOfLookUpTables(void)
{
	return m_numberOfLookUpTableEntries;
}

unsigned long cTextureBMP::GetNumberOfImportantColours(void)
{
	return m_numberOfImportantColours;
}

c24BitBMPPixel cTextureBMP::getPixelAtRowColumn(unsigned int row, unsigned int column)
{
	if (row >= this->m_numberOfRows)
	{
		row = this->m_numberOfRows - 1;
	}
	if (column >= this->m_numberOfColumns)
	{
		column = this->m_numberOfColumns - 1;
	}

	return this->m_p_theImages[row * this->m_Width_or_Columns + column];
}

c24BitBMPPixel cTextureBMP::getPixelAtUV(float U, float V)
{
	U = fabs(U);
	V = fabs(V);

	if (U > 1.0f) { U = U - std::ceil(U); }
	if (V > 1.0f) { V = V - std::ceil(V); }

	unsigned int row = static_cast<unsigned int>(U * this->m_numberOfRows);
	unsigned int col = static_cast<unsigned int>(V * this->m_numberOfColumns);

	return this->getPixelAtRowColumn(row, col);
}

std::string cTextureBMP::GetTextureFullPathName()
{
	return this->_pTexInfo->_fullPathName;
}

cTextureBMP::sTextureInfo* cTextureBMP::GetTextureInfo()
{
	return this->_pTexInfo;
}

bool cTextureBMP::UpdateTripleBuffer()
{
	if (this->_pTexTripleBuffer->vecsTextures[this->_pTexTripleBuffer->GetCurrentReadBuffer()][this->_pTexInfo->_texIndex].second == eTexState::PENDING)
	{
		this->_pTexTripleBuffer->vecsTextures[this->_pTexTripleBuffer->GetCurrentWriteBuffer()][this->_pTexInfo->_texIndex].first = this->_pTexInfo->_texName;
		this->_pTexTripleBuffer->vecsTextures[this->_pTexTripleBuffer->GetCurrentWriteBuffer()][this->_pTexInfo->_texIndex].second = eTexState::LOADED_FROM_FILE;
		return false;
	}

	return true;
	//this->_pTexInfo->_texState = this->_pTexTripleBuffer->vecsTextures[this->_pTexTripleBuffer->GetCurrentReadBuffer()][this->_pTexInfo->_texIndex].second;
}
