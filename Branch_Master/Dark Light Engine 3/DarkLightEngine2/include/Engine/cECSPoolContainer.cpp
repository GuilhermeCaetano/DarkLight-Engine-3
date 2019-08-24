// cECSPoolContainer.cpp

#include "cECSPoolContainer.h"
#include "sObject.h"


cECSPoolContainer* cECSPoolContainer::_pPoolContainer = nullptr;

cECSPoolContainer::cECSPoolContainer()
{
	return;
}

cECSPoolContainer* cECSPoolContainer::GetInstance()
{
	if (_pPoolContainer == nullptr)
	{
		_pPoolContainer = new cECSPoolContainer();
	}

	return _pPoolContainer;
}

cECSPoolContainer::~cECSPoolContainer()
{
	return;
}

sMemoryChunk::sMemoryChunk()
{
	return;
}

sMemoryChunk::~sMemoryChunk()
{
	bool result = this->DeallocateAllMemory();
	if (!result)
	{
		printf("Error deallocating memory chunk!\n");
	}

	return;
}

bool sMemoryChunk::DeallocateAllMemory()
{
	if (this->_mapContainersByTypeName.size() <= 0)
	{
		return false;
	}

	// Go through the map and free all array content, as well as the array itself
	std::unordered_map<std::string, sMemoryChunk::sContainer*>::iterator mapIter = this->_mapContainersByTypeName.begin();
	for (mapIter; mapIter != this->_mapContainersByTypeName.end(); mapIter++)
	{
		// The contents inside the array will be created using malloc and placement new.
		// So first we must call the destructor on every object in the array.
		sMemoryChunk::sContainer* cont = mapIter->second;
		char* firstElement = (char*)cont->dynamicArray; // Pointer to the first element of the array
		for (unsigned int i = 0; i < cont->arraySize; i++)
		{
			void* iElement = (void*)(&(*firstElement) + (i * cont->objSize)); // Get i element of the array by using pointer arithmetic
			sObject* finalElement = static_cast<sObject*>(iElement);
			if (finalElement != NULL && finalElement->_bIsInitialized)
			{
				// This will call the overloaded delete, which allows us to call the
				// destructor from the derivate class without knowing its actual class.
				// This has to be checked first, otherwise it will try to access an uninitialized class!
				finalElement->~sObject();
			}
		}

		// Whole array created by malloc, use free to free the memory back
		free(cont->dynamicArray);
		delete cont;
	}

	this->_mapArraysByTypeName.clear();

	return true;
}

bool sMemoryChunk::ContainsElement(const std::string& typeName)
{
	if (this->_mapContainersByTypeName.size() <= 0)
	{
		return false;
	}

	// Check for the desired type
	std::unordered_map<std::string, sContainer*>::iterator mapIter = this->_mapContainersByTypeName.find(typeName);
	if (mapIter != this->_mapContainersByTypeName.end())
	{
		// Found it
		return true;
	}

	return false;
}

sMemoryChunk::sContainer* sMemoryChunk::GetArrayContainer(const std::string& typeName)
{
	// Find the type on the map
	std::unordered_map<std::string, sContainer*>::iterator mapIter = this->_mapContainersByTypeName.find(typeName);
	if (mapIter != this->_mapContainersByTypeName.end())
	{
		// Return the pair by reference
		return mapIter->second;
	}

	return nullptr;
}

//bool sMemoryChunk::GetArrayPair(const std::string& typeName, std::pair<unsigned int, void*>& retPair)
//{
//	// Find the type on the map
//	std::unordered_map<std::string, std::pair<unsigned int, void*>>::iterator mapIter = this->_mapArraysByTypeName.find(typeName);
//	if (mapIter != this->_mapArraysByTypeName.end())
//	{
//		// Return the pair by reference
//		retPair = mapIter->second;
//		return true;
//	}
//
//	return false;
//}

void* sMemoryChunk::GetArrayElement(const std::string& typeName, bool isActive)
{
	// Get the array pair from the type
	sContainer* cont = this->GetArrayContainer(typeName);

	if (cont == nullptr)
	{
		// Didn't find it, bail.
		return nullptr;
	}

	// Go through the array and find matching element.
	sObject* object = nullptr;
	for (unsigned int i = 0; i < cont->arraySize; i++)
	{
		sObject* firstElement = static_cast<sObject*>(cont->dynamicArray); // Pointer to the first element of the array
		object = static_cast<sObject*>(&(*(firstElement + (i * cont->objSize)))); // Get i element of the array by using pointer arithmetic

		// Check for match
		if (!object->GetIsInitialized())
		{
			return object;
		}
	}

	return nullptr;
}

sMemoryChunk* cECSPoolContainer::CreateMemoryChunk()
{
	sMemoryChunk* pChunk = new sMemoryChunk();
	this->_vecMemoryChunks.push_back(pChunk);
	return pChunk;
}

bool cECSPoolContainer::CleanAllMemoryChunks()
{
	// Deallocate memory on all chunks
	/*bool safelyDeallocated = true;
	for (unsigned int i = 0; i < this->_vecMemoryChunks.size(); i++)
	{
		bool result = this->_vecMemoryChunks[i]->DeallocateAllMemory();
		if (!result)
		{
			safelyDeallocated = false;
			printf("Error deallocating memory chunk %d!\n", i);
		}
	}*/

	for (unsigned int i = 0; i < this->_vecMemoryChunks.size(); i++)
	{
		delete this->_vecMemoryChunks[i];
	}

	return true;
}

bool cECSPoolContainer::GetPoolPair(const std::string& typeName, std::pair<unsigned int, void*>& retPair)
{
	// Get the first chunk that contains the type
	sMemoryChunk* chunk = this->_GetChunk(typeName);

	// Attempt to get the pair
	//bool result = chunk->GetArrayPair(typeName, retPair);

	return false;
}

void* cECSPoolContainer::GetPoolElement(const std::string& typeName, bool isEnabled)
{
	// Try to get memory chunk containing the type
	sMemoryChunk* chunk = this->_GetChunk(typeName);

	if (chunk == nullptr)
	{
		// Didn't find it, bail
		return nullptr;
	}

	void* retValue = chunk->GetArrayElement(typeName, isEnabled);
	return retValue;
}

sMemoryChunk* cECSPoolContainer::_GetChunk(const std::string& typeName)
{
	for (unsigned int i = 0; i < this->_vecMemoryChunks.size(); i++)
	{
		// Search for a memory chunk that contains the type
		if (this->_vecMemoryChunks[i]->ContainsElement(typeName))
		{
			return this->_vecMemoryChunks[i];
		}
	}

	// Didn't find any, return null
	return nullptr;
}
