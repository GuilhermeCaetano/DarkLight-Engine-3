#ifndef _cTextureTripleBuffer_HG_
#define _cTextureTripleBuffer_HG_

#include <vector>
#include <OpenGL\OpenGLHeaders.h>
#include <string>


class cTextureBMP;
class cTextureTripleBuffer
{
public:
	cTextureTripleBuffer();
	~cTextureTripleBuffer();

	bool CreateTextureThread(cTextureBMP* pTextureBMP);

	// Initialize the buffers
	void InitBuffers(unsigned int bufferSize);

	// Select a buffer to work on
	unsigned int GetCurrentReadBuffer();
	unsigned int GetCurrentWriteBuffer();
	unsigned int GetCurrentStandbyBuffer();

	// Switch the buffers around
	bool SwitchBuffers();

	void UpdateBufferAt(unsigned int texIndex, std::pair<std::string, cTextureBMP*>texPair);
	
	// Array of vectors, the 3 buffers 
	static const unsigned int numBuffers = 3;
	std::vector<std::pair<std::string, cTextureBMP*>> vecsTextures[numBuffers];

private:
	unsigned int _currentReadBuffer;
	unsigned int _currentWriteBuffer;
	unsigned int _currentStandbyBuffer;

};



#endif // !_cTextureTripleBuffer_HG_

