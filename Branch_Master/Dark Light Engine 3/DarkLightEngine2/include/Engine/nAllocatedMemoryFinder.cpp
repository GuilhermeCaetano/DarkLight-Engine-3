// nAllocatedMemoryFinder.cpp

#include "nAllocatedMemoryFinder.h"
#include "cECSPoolContainer.h"


namespace nMemory
{
	// Find a pre-allocated memory for an object of correct type.
	void* FindAllocatedMemory(const std::string& typeName, bool alreadyInitialized)
	{
		void* allocElement = cECSPoolContainer::GetInstance()->GetPoolElement(typeName, alreadyInitialized);
		return allocElement;
	}
}
