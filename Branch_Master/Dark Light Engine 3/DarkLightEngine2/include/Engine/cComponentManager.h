#ifndef _cComponentManager_HG_
#define _cComponentManager_HG_


/*  COMPONENT MANAGER
 - Creates components based on their type from the factory
 - Deletes components based on their type
 - Responsible for calling for the allocation of memory for the components
 - Responsible for the calling for freeing the allocated memory
*/

#include "nAllocatedMemoryFinder.h"
#include <Entity\Components\iComponent.h>
#include <Interfaces\Physics\iPhysicsDef.h>
#include <Interfaces\Physics\sRigidbodyDef.h>
#include <Interfaces\Physics\sSoftbodyDef.h>
#include <Interfaces\Physics\sCompoundbodyDef.h>
#include <Entity\Factory\cComponentFactory.h>
#include <Interfaces\Physics\iPhysicsFactory.h>
#include <string>


class cComponentManager
{
public:
	cComponentManager();
	~cComponentManager();

	iComponent* CreateComponent(const std::string& componentType, unsigned long long parentID)
	{
		// Since we have the pre allocated pools, get the pointer to the matching element
		// Use placement new to manually call constructor, since we only get the malloc memory
		iComponent* component = this->_pComponentFactory->CreateComponentOfType(componentType, parentID);
		return component;
	}

	template <typename T>
	T* CreateComponent(unsigned long long parentID)
	{
		// Since we have the pre allocated pools, get the pointer to the matching element
		// Use placement new to manually call constructor, since we only get the malloc memory
		void* allocElement = nMemory::FindAllocatedMemory(T::GetStaticType());
		if (allocElement == nullptr)
		{
			printf("Error! Couldn't find allocation for %s!\n", typeid(T).name());
			return nullptr;
		}

		T* component = ::new (allocElement) T(parentID);
		return component;
	}

	template <typename T>
	T* CreateComponent(nPhysics::iPhysicsDef& def, unsigned long long parentID)
	{
		// Since we have the pre allocated pools, get the pointer to the matching element
		// Use placement new to manually call constructor, since we only get the malloc memory

		T* component = nullptr;
		if (def.defType == "Rigidbody")
		{
			nPhysics::sRigidbodyDef* rbDef = static_cast<nPhysics::sRigidbodyDef*>(&def);
			void* allocElement = nMemory::FindAllocatedMemory(this->_pPhysicsFactory->GetRigidbodyType());
			component = dynamic_cast<T*>(this->_pPhysicsFactory->CreateRigidbody(*rbDef, parentID, allocElement));
		}
		else if (def.defType == "Softbody")
		{
			nPhysics::sSoftbodyDef* sbDef = static_cast<nPhysics::sSoftbodyDef*>(&def);
			void* allocElement = nMemory::FindAllocatedMemory(this->_pPhysicsFactory->GetSoftdbodyType());
			component = dynamic_cast<T*>(this->_pPhysicsFactory->CreateSoftbody(*sbDef, parentID, allocElement));
		}
		else if (def.defType == "Compoundbody")
		{
			nPhysics::sCompoundbodyDef* cbDef = static_cast<nPhysics::sCompoundbodyDef*>(&def);
			void* allocElement = nMemory::FindAllocatedMemory(this->_pPhysicsFactory->GetCompoundbodyType());
			component = dynamic_cast<T*>(this->_pPhysicsFactory->CreateCompoundbody(*cbDef, parentID, allocElement));
		}

		return component;
	}

private:
	void _Cleanup();

	cComponentFactory* _pComponentFactory;
	nPhysics::iPhysicsFactory* _pPhysicsFactory;

};



#endif // !_cComponentManager_HG_

