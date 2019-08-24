#ifndef _cECSPoolContainer_HG_
#define _cECSPoolContainer_HG_

#include <OpenGL\OpenGLHeaders.h>
#include <Engine\sObject.h>
#include <unordered_map>
#include <vector>
#include <string>


struct sMemoryChunk
{
	sMemoryChunk();
	~sMemoryChunk();

	struct sContainer
	{
		sContainer(size_t amount, size_t size)
		{
			this->arraySize = amount;
			this->objSize = size;
			this->dynamicArray = (sObject*)calloc(amount, size);
		}

		~sContainer() {};

		size_t arraySize;
		size_t objSize;
		sObject* dynamicArray;
	};

	// Allocates memory for a specific element type
	template <typename T>
	bool AllocateMemory(size_t amount, bool reallocateIfDuplicate = false)
	{
		std::string typeName = T::GetStaticType();

		// reallocate memory to new size
		if (this->ContainsElement(typeName))
		{
			if (!reallocateIfDuplicate)
			{
				return false;
			}

			//TODO: reallocate memory

		}

		double startTime = glfwGetTime();
		sContainer* cont = new sContainer(amount, sizeof(T));
		this->_mapContainersByTypeName[typeName] = cont;

		double endTime = glfwGetTime() - startTime;
		printf("Allocated memory for %d number of %s in %.2f seconds. Block size: %zd.\n", amount, typeName.c_str(), (float)endTime, (amount * sizeof(T)));

		// Create a new dynamic array, allocate memory for all the elements and initialize it all to 0.
		//sObject* dynamicArray = static_cast<sObject*>(calloc(amount, sizeof(T)));
		//void* baseObj = (void*)dynamicArray;

		// Set new array pair into the map
		//this->_mapArraysByTypeName[typeName] = std::make_pair(amount, baseObj);
	}
	
	// Adds elements to the arrays.
	//template <typename T>
	//bool AddElement(void* object)
	//{
	//	std::string typeName = T::GetStaticType();

	//	// Check if there is an array allocated
	//	if (!this->ContainsElement(typeName))
	//	{
	//		printf("WARNING! No type found to allocate memory!\n");
	//		return false;
	//	}

	//	// Get allocated array and its full size
	//	std::pair<unsigned int, void*> elementPair = this->_mapArraysByTypeName[typeName];
	//	unsigned int arrSize = elementPair.first;
	//	T* arr = (T*)elementPair.second;

	//	// Search the array for an empty slot
	//	for (unsigned int i = 0; i < arrSize; i++)
	//	{
	//		if (arr[i] == 0)
	//		{
	//			// Assign new slot
	//			T obj = (T)object;
	//			printf("Allocated new object at index %d\n", i);
	//			arr[i] = obj;
	//			return true;
	//		}
	//	}

	//	printf("WARNING! Could not find allocated space for entity!\n");
	//	return false;
	//}

	bool DeallocateAllMemory();

	// Returns whether the chunk contain a certain element type
	bool ContainsElement(const std::string& typeName);

	// Returns an array container by type
	sContainer* GetArrayContainer(const std::string& typeName);

	// Returns an array pair by type
	//bool GetArrayPair(const std::string& typeName, std::pair<unsigned int, void*>& retPair);

	// Returns the first object with matching parameters
	void* GetArrayElement(const std::string& typeName, bool isEnabled = false);

	std::unordered_map<std::string, std::pair<unsigned int, void*>> _mapArraysByTypeName;
	std::unordered_map<std::string, sContainer*> _mapContainersByTypeName;
};

class cECSPoolContainer
{
public:
	static cECSPoolContainer* GetInstance();
	~cECSPoolContainer();

	// Initialize and delete each pool.
	sMemoryChunk* CreateMemoryChunk();
	bool CleanAllMemoryChunks();

	// Get an array pair
	bool GetPoolPair(const std::string& typeName, std::pair<unsigned int, void*>& retPair);

	// Returns the first object with matching parameters
	void* GetPoolElement(const std::string& typeName, bool isEnabled = false);

private:
	cECSPoolContainer();
	static cECSPoolContainer* _pPoolContainer;

	// Returns a specific chunk by type
	sMemoryChunk* _GetChunk(const std::string& typeName);

	// Resizes a specific pool. This will resize it by 1.2%
	void _ResizePool();

	std::vector<sMemoryChunk*> _vecMemoryChunks;

};


#endif // !_cECSPoolContainer_HG_

