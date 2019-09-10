#ifndef _nAllocatedMemoryFinder_HG_
#define _nAllocatedMemoryFinder_HG_

#include <string>
#include <Engine\sObject.h>
#include <vector>


namespace nMemory
{
	// Find a pre-allocated memory for an object of correct type.
	void* InitAllocatedMemory(const std::string& typeName);
	void* FindAllocatedMemoryFromEntity(const std::string& typeName, const std::string& entityName, bool isActive = true);
	//void* FindAllocatedMemory(const std::string& typeName, bool alreadyInitialized = false);
	void* FindAllocatedContainer(const std::string& typeName);
	std::vector<void*> FindAllocatedContainerMultiple(const std::string& typeName);

	//TODO: Move to a better place
	template <typename T>
	T* GetArithmeticArrayElement(unsigned int index, sObject* dynamicArr, size_t objSize)
	{
		sObject* firstElement = static_cast<sObject*>(dynamicArr); // Pointer to the first element of the array
		sObject* object = static_cast<sObject*>(&(*(firstElement + (index * objSize)))); // Get i element of the array by using pointer arithmetic
		return static_cast<T*>(object);
	}
}


#endif // !_nAllocatedMemoryFinder_HG_

