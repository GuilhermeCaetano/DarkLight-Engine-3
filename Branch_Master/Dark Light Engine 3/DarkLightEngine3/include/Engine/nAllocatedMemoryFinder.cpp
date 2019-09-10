// nAllocatedMemoryFinder.cpp

#include "nAllocatedMemoryFinder.h"
#include "cECSPoolContainer.h"


namespace nMemory
{
	// Find and set to initialize an allocated, but uninitializd data of correct type.
	void* InitAllocatedMemory(const std::string& typeName)
	{
		void* allocElement = cECSPoolContainer::GetInstance()->InitPoolElement(typeName);
		return allocElement;
	}

	// Find a pre-allocated memory for an object of correct type.
	void* FindAllocatedMemoryFromEntity(const std::string& typeName, const std::string& entityName, bool isActive)
	{
		void* allocElement = cECSPoolContainer::GetInstance()->GetPoolElementFromEntity(typeName, entityName, isActive);
		return allocElement;
	}

	// Find a pre-allocated memory for an object of correct type.
	/*void* FindAllocatedMemory(const std::string& typeName, bool isActive)
	{
		void* allocElement = cECSPoolContainer::GetInstance()->GetPoolElement(typeName, isActive);
		return allocElement;
	}*/

	void* FindAllocatedContainer(const std::string& typeName)
	{
		sMemoryChunk::sContainer* container = cECSPoolContainer::GetInstance()->GetPoolContainer(typeName);
		return ((void*)container);
	}

	std::vector<void*> FindAllocatedContainerMultiple(const std::string& typeName)
	{
		std::vector<void*> vecContainers = cECSPoolContainer::GetInstance()->GetPoolContainerMultiple(typeName);
		return vecContainers;
	}
}
