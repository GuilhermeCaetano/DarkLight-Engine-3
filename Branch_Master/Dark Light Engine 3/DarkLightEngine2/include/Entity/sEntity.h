#ifndef _sEntity_HG_
#define _sEntity_HG_

#include <Interfaces\Physics\iEntity.h>
#include "Components\iComponent.h"
#include "Components\sLocationComponent.h"
#include <Engine\nAllocatedMemoryFinder.h>
#include <Engine\cEngine.hpp>
#include <unordered_map>
#include <vector>
#include <string>

/*
 - The Entity is nothing more than a struct of data that will hold
   information about the possible functionalities that the object will have.
 - A unique ID that will separate every entity from each other
 - A name, that doesn't need to be unique
 - A bool to check for enabled status
*/

namespace nPhysics
{
	struct iPhysicsDef;
}


struct sEntity : public iEntity
{
public:
	sEntity();
	~sEntity();
	
	virtual void Start();

	inline static std::string GetStaticType() { return "ENTITY"; };

	// Get/Set entity name. Returns the entitys tag
	inline void SetName(const std::string& name) { this->_name = name; };
	inline std::string GetName() { return this->_name; };

	// Get/Set tags. Returns the entitys tag
	void SetTag(const std::string& tag);
	std::string GetTag();

	// Get/Set activation state. Returns whether the object is active or not
	inline void SetIsActive(bool active) { this->_bIsActive = active; };
	inline bool GetIsActive() { return this->_bIsActive; };

	// Returns the entitys own unique ID
	unsigned long long GetInstanceID();

	// Adds a component. It will actually find a pre-allocated
	// component and initialize it. Returns the component back.
	template <typename T>
	T* AddComponent()
	{
		T* component = cEngine::template CreateComponent<T>(this->GetInstanceID());
		this->_mapComponents[T::GetStaticType()] = component;
		return component;
	}

	// A way of adding physical components. This must be used if different DLL's
	// are being used. The DLL type is defined on the configuration of the engine.
	template <typename T>
	T* AddComponent(nPhysics::iPhysicsDef& def)
	{
		T* component = cEngine::template CreateComponent<T>(def, this->_instanceID);
		this->_mapComponents[T::GetStaticType()] = component;
		return component;
	}

	void AddComponent(iComponent* component)
	{
		this->_mapComponents[component->GetStaticType()] = component;
	}
	
	// Get a component by its class type.
	template <typename T>
	T* GetComponent()
	{
		std::unordered_map<std::string, iComponent*>::iterator mapIter = this->_mapComponents.find(T::GetStaticType());
		if (mapIter != this->_mapComponents.end())
		{
			return static_cast<T*>(mapIter->second);
		}

		return nullptr;
	}

	// Add child objects to the entity.
	void AddChildObject(sEntity* childEntity);
	std::vector<sEntity*>* GetChildObjects();

	// Get info and data of childs attached to this entity.
	unsigned int GetNumChilds();
	sEntity* GetChildAt(unsigned int index);
	
	// Operator new is being overloaded to prevent allocation
	// outside the pre-allocation. Can be used by itself instead of CreateEntity
	// in other scripts. The result is the same.
	void* operator new(size_t size)
	{
		// Get pre-allocated memory, or create it.
		// operator new will call constructor after it returns.
		void* element = nMemory::FindAllocatedMemory(sEntity::GetStaticType());
		return element;
	}

	// Operator delete is being overloaded to prevent memory being freed alone.
	// The manager deals with freeing it when needed to keep allocation under control.
	// Use Destroy instead, which will uninitialize the object for reuse later.
	//inline void operator delete(void* object) {};

	// Returns the location component
	inline sLocationComponent* GetTransform();
	sLocationComponent* transform;

protected:
	// Serialization
	virtual void _SerializeToJson(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& prettyWriter);
	virtual void _DeserializeFromJson(rapidjson::Value::ConstValueIterator vIter);

	// Containers
	std::unordered_map<std::string/*type*/, iComponent*/*component*/> _mapComponents;
	std::vector<sEntity*> _vecChilds;

	// Static IDs for entities only
	unsigned long long _instanceID = 0;
	static unsigned long long _nextID;
	static const unsigned long long _startID = 0;

	// Common data for all entities
	std::string _name;
	std::string _tag;
	
};


#endif // !_sEntity_HG_
