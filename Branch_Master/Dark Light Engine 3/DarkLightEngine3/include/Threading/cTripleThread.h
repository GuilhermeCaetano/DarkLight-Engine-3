#ifndef _cTripleThread_HG_
#define _cTripleThread_HG_

#include <vector>
#include <OpenGL\OpenGLHeaders.h>


class cTripleThread
{
public:
	cTripleThread();
	~cTripleThread();

	// Initialize the buffers
	void InitBuffers(unsigned int bufferSize);

	// Select a buffer to work on
	unsigned int GetCurrentReadBuffer();
	unsigned int GetCurrentWriteBuffer();
	unsigned int GetCurrentStandbyBuffer();

	// Switch the buffers around
	bool SwitchBuffers();
	
	// Array of vectors, the 3 buffers 
	static const unsigned int numBuffers = 3;
	std::vector<glm::vec3> vecsTargetDirection[numBuffers];

private:
	unsigned int _currentReadBuffer;
	unsigned int _currentWriteBuffer;
	unsigned int _currentStandbyBuffer;

};



#endif // !_cTripleThread_HG_

