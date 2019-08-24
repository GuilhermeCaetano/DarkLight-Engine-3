#ifndef _nAllocatedMemoryFinder_HG_
#define _nAllocatedMemoryFinder_HG_

#include <string>


namespace nMemory
{
	// Find a pre-allocated memory for an object of correct type.
	void* FindAllocatedMemory(const std::string& typeName, bool alreadyInitialized = false);
}


#endif // !_nAllocatedMemoryFinder_HG_

