// cMemoryAllocator.cpp

#include "nMemoryAllocator.h"
#include <malloc.h>


namespace nMemoryAllocator
{
	void* AllocateMemory(unsigned int amount, unsigned int size)
	{
		return calloc(amount, size);
	}

	bool FreeMemory(void* allocObj)
	{
		operator delete(allocObj);

		return true;
	}
}
