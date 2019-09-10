// cECSPoolContainer.cpp

#include "cECSPoolContainer.h"
#include "sObject.h"
#include <Entity\sEntity.h>


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

	printf("\nMemory Block:\n");
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
		printf("Freed memory of %s array.\n", mapIter->first.c_str());
	}

	this->_mapContainersByTypeName.clear();

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

void* sMemoryChunk::InitArrayElement(const std::string& typeName)
{
	// Get the array pair from the type
	sContainer* cont = this->GetArrayContainer(typeName);

	if (cont == nullptr)
	{
		// Didn't find it, bail.
		return nullptr;
	}

	// Go through the array and find matching element.
	void* object = cont->GetArrayElement(typeName);
	if (object != nullptr)
	{
		cont->numInitializedElements++;
		return object;
	}

	return nullptr;
}

void* sMemoryChunk::sContainer::GetArrayElement(const std::string& typeName, bool isInitialized)
{
	// Go through the array and find matching element.
	sObject* object = nullptr;
	for (unsigned int i = 0; i < this->arraySize; i++)
	{
		sObject* firstElement = static_cast<sObject*>(this->dynamicArray); // Pointer to the first element of the array
		object = static_cast<sObject*>(&(*(firstElement + (i * this->objSize)))); // Get i element of the array by using pointer arithmetic

		// Check for match
		if (object->GetIsInitialized() == isInitialized)
		{
			return (void*)object;
		}
	}

	return nullptr;
}

void* sMemoryChunk::sContainer::GetArrayElementFromEntity(const std::string& typeName, const std::string& entityName, bool isActive)
{
	// Go through the array and find matching element.
	sObject* object = nullptr;
	for (unsigned int i = 0; i < this->arraySize; i++)
	{
		sObject* firstElement = static_cast<sObject*>(this->dynamicArray); // Pointer to the first element of the array
		object = static_cast<sObject*>(&(*(firstElement + (i * this->objSize)))); // Get i element of the array by using pointer arithmetic

		// Check for match
		if (object->GetIsInitialized() == true)
		{
			sEntity* entity = static_cast<sEntity*>(object->GetParent());
			if (entity != nullptr && entity->GetName() == entityName && entity->GetIsActive() == isActive)
			{
				return (void*)object;
			}
		}
	}

	return nullptr;
}

void* sMemoryChunk::GetEntityByName(const std::string& elementName, bool isActive)
{
	// Get the array pair from the type
	sContainer* cont = this->GetArrayContainer(sEntity::GetStaticType());

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
		if (object->GetIsInitialized())
		{
			sEntity* entity = static_cast<sEntity*>(object);
			if (entity->GetName() == elementName && entity->GetIsActive() == isActive)
			{
				return entity;
			}
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
	for (unsigned int i = 0; i < this->_vecMemoryChunks.size(); i++)
	{
		delete this->_vecMemoryChunks[i];
	}
	printf("\n");

	return true;
}

void* cECSPoolContainer::InitPoolElement(const std::string& typeName)
{
	//void* retValue = nMemory::InitAllocatedMemory(typeName);
	// Look for all memory chunks that contain the type
	std::vector<sMemoryChunk*> vecChunks = this->_GetChunkMultiple(typeName);

	// Search in all chunks for non-initialized elements to use
	for (unsigned int i = 0; i < vecChunks.size(); i++)
	{
		sMemoryChunk* chunk = vecChunks[i];
		void* element = chunk->InitArrayElement(typeName);

		if (element != nullptr)
		{
			return element;
		}
	}

	return nullptr;
}

sMemoryChunk::sContainer* cECSPoolContainer::GetPoolContainer(const std::string& typeName)
{
	// Get the first chunk that contains the type
	sMemoryChunk* chunk = this->_GetChunk(typeName);
	if (chunk == nullptr)
	{
		// Didn't find it, bail
		return nullptr;
	}

	sMemoryChunk::sContainer* container = chunk->GetArrayContainer(typeName);
	return container;
}

std::vector<void*> cECSPoolContainer::GetPoolContainerMultiple(const std::string& typeName)
{
	std::vector<sMemoryChunk*> vecChunks = this->_GetChunkMultiple(typeName);
	std::vector<void*> vecContainers;
	for (unsigned int i = 0; i < vecChunks.size(); i++)
	{
		sMemoryChunk::sContainer* container = vecChunks[i]->GetArrayContainer(typeName);
		if (container != nullptr)
		{
			vecContainers.push_back((void*)container);
		}
	}

	return vecContainers;
}

void* cECSPoolContainer::GetPoolElement(const std::string& typeName, bool isActive)
{
	// Try to get memory chunk containing the type
	sMemoryChunk* chunk = this->_GetChunk(typeName);

	if (chunk == nullptr)
	{
		// Didn't find it, bail
		return nullptr;
	}

	// Get the array pair from the type
	sMemoryChunk::sContainer* cont = chunk->GetArrayContainer(sEntity::GetStaticType());

	if (cont == nullptr)
	{
		// Didn't find it, bail.
		return nullptr;
	}

	void* retValue = cont->GetArrayElement(typeName, isActive);
	return retValue;
}

void* cECSPoolContainer::GetPoolElementFromEntity(const std::string& typeName, const std::string& entityName, bool isActive)
{
	// Try to get memory chunk containing the type
	std::vector<sMemoryChunk*> chunks = this->_GetChunkMultiple(typeName);

	for (unsigned int i = 0; i < chunks.size(); i++)
	{
		// Get the array pair from the type
		sMemoryChunk::sContainer* cont = chunks[i]->GetArrayContainer(typeName);
		if (cont == nullptr)
		{
			// Didn't find it, move on.
			continue;
		}

		void* retValue = cont->GetArrayElementFromEntity(typeName, entityName, isActive);
		if (retValue != nullptr)
		{
			return retValue;
		}
	}

	return nullptr;
}

//void* cECSPoolContainer::GetPoolElement(sMemoryChunk::sContainer* container, unsigned int index, bool isEnabled)
//{
//	// Go through the array and find matching element.
//	sObject* object = nullptr;
//	sObject* firstElement = static_cast<sObject*>(container->dynamicArray); // Pointer to the first element of the array
//	object = static_cast<sObject*>(&(*(firstElement + (index * container->objSize)))); // Get i element of the array by using pointer arithmetic
//
//	// Check for match
//	if (object->GetIsInitialized())
//	{
//		return object;
//	}
//
//	return nullptr;
//}

void* cECSPoolContainer::GetPoolEntityByName(const std::string& entityName, bool isEnabled)
{
	// Try to get memory chunk containing the type
	sMemoryChunk* chunk = this->_GetChunk(sEntity::GetStaticType());

	if (chunk == nullptr)
	{
		// Didn't find it, bail
		return nullptr;
	}

	void* retValue = chunk->GetEntityByName(entityName, isEnabled);
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

std::vector<sMemoryChunk*> cECSPoolContainer::_GetChunkMultiple(const std::string & typeName)
{
	std::vector<sMemoryChunk*> vecChunks;
	for (unsigned int i = 0; i < this->_vecMemoryChunks.size(); i++)
	{
		// Search for a memory chunk that contains the type
		if (this->_vecMemoryChunks[i]->ContainsElement(typeName))
		{
			vecChunks.push_back(this->_vecMemoryChunks[i]);
		}
	}

	return vecChunks;
}

