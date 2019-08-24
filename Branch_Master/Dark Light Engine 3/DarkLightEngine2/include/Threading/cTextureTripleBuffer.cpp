// cTextureTripleBuffer.cpp

#include <Threading\cTextureTripleBuffer.h>
#include <Texture\cTextureBMP.h>


cTextureTripleBuffer::cTextureTripleBuffer()
{
	// Initialize critical section
	//InitializeCriticalSection(&CSBufferIndex);

	// The read buffer should always be in
	// front of the write buffer. This way we
	// have two buffers of space between reading and writing.
	this->_currentReadBuffer = 1;
	this->_currentWriteBuffer = 0;
	this->_currentStandbyBuffer = 2;

	return;
}

cTextureTripleBuffer::~cTextureTripleBuffer()
{
	// Need to nullify all the vectors, otherwise they will try to
	// destroy the pointers that are already managed by the manager
	for (unsigned int i = 0; i < 3; i++)
	{
		for (unsigned int j = 0; j < this->vecsTextures[i].size(); j++)
		{
			std::pair<std::string, cTextureBMP*> tempPair = this->vecsTextures[i][j];
			if (tempPair.second != nullptr)
			{
				delete tempPair.second;
			}
		}
	}

	return;
}



void cTextureTripleBuffer::InitBuffers(unsigned int bufferSize)
{
	// Initialize all three buffers
	for (unsigned int i = 0; i != this->numBuffers; i++)
	{
		this->vecsTextures[i].reserve(bufferSize);
		
		for (unsigned int j = 0; j < bufferSize; j++)
		{
			this->vecsTextures[i].push_back(std::make_pair("", nullptr));
		}
	}
}

unsigned int cTextureTripleBuffer::GetCurrentReadBuffer()
{
	return this->_currentReadBuffer;
}

unsigned int cTextureTripleBuffer::GetCurrentWriteBuffer()
{
	return this->_currentWriteBuffer;
}

unsigned int cTextureTripleBuffer::GetCurrentStandbyBuffer()
{
	return this->_currentStandbyBuffer;
}

bool cTextureTripleBuffer::SwitchBuffers()
{
	// Enter critical section to safely move the buffers around
	//EnterCriticalSection(&CSBufferIndex);

	if (this->vecsTextures->size() == 0)
	{
		return false;
	}

	if (this->vecsTextures[0].size() == 0)
	{
		return false;
	}

	// Update buffer indexes and check for looping
	this->_currentReadBuffer = ((this->_currentReadBuffer + 1 >= this->numBuffers) ? 0 : this->_currentReadBuffer + 1);
	this->_currentWriteBuffer = ((this->_currentWriteBuffer + 1 >= this->numBuffers) ? 0 : this->_currentWriteBuffer + 1);
	this->_currentStandbyBuffer = ((this->_currentStandbyBuffer + 1 >= this->numBuffers) ? 0 : this->_currentStandbyBuffer + 1);

	// Exit critical section
	//LeaveCriticalSection(&CSBufferIndex);

	return true;
}

void cTextureTripleBuffer::UpdateBufferAt(unsigned int texIndex, std::pair<std::string, cTextureBMP*> texPair)
{
	// Update current write buffer value
	this->vecsTextures[this->_currentWriteBuffer][texIndex] = texPair;
}
