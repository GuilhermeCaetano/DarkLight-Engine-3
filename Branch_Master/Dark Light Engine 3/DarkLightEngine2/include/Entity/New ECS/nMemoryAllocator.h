#ifndef _nMemoryAllocator_HG_
#define _nMemoryAllocator_HG_

namespace nMemoryAllocator
{
	static bool AllocateMemory(unsigned long size, void** allocObj);
	static bool FreeMemory(void* allocObj);
}


#endif // !_nMemoryAllocator_HG_

