// cTripleThread.cpp

#include <Threading\cTripleThread.h>
#include <process.h>
#include <Windows.h>

CRITICAL_SECTION CSBufferIndex;

cTripleThread::cTripleThread()
{
	// Initialize critical section
	InitializeCriticalSection(&CSBufferIndex);

	// The read buffer should always be in
	// front of the write buffer. This way we
	// have two buffers of space between reading and writing.

	this->_currentReadBuffer = 1;
	this->_currentWriteBuffer = 0;
	this->_currentStandbyBuffer = 2;

	return;
}

cTripleThread::~cTripleThread()
{
	// Delete critical section
	DeleteCriticalSection(&CSBufferIndex);

	return;
}

void cTripleThread::InitBuffers(unsigned int bufferSize)
{
	// Initialize all three buffers
	for (unsigned int i = 0; i != this->numBuffers; i++)
	{
		this->vecsTargetDirection[i].reserve(bufferSize);

		for (unsigned int j = 0; j <= bufferSize; j++)
		{
			this->vecsTargetDirection[i].push_back(glm::vec3(0.0f));
		}
	}
}

unsigned int cTripleThread::GetCurrentReadBuffer()
{
	return this->_currentReadBuffer;
}

unsigned int cTripleThread::GetCurrentWriteBuffer()
{
	return this->_currentWriteBuffer;
}

unsigned int cTripleThread::GetCurrentStandbyBuffer()
{
	return this->_currentStandbyBuffer;
}

bool cTripleThread::SwitchBuffers()
{
	// Enter critical section to safely move the buffers around
	EnterCriticalSection(&CSBufferIndex);

	// Update buffer indexes and check for looping
	this->_currentReadBuffer = ((this->_currentReadBuffer + 1 >= this->numBuffers) ? 0 : this->_currentReadBuffer + 1);
	this->_currentWriteBuffer = ((this->_currentWriteBuffer + 1 >= this->numBuffers) ? 0 : this->_currentWriteBuffer + 1);
	this->_currentStandbyBuffer = ((this->_currentStandbyBuffer + 1 >= this->numBuffers) ? 0 : this->_currentStandbyBuffer + 1);

	// Exit critical section
	LeaveCriticalSection(&CSBufferIndex);

	return true;
}
