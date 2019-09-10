// cEntityManager.cpp

#include "cEntityManager.h"
#include <Entity\sEntity.h>
#include <Entity\Factory\cEntityFactory.h>


cEntityManager::cEntityManager()
{
	this->_pEntityFactory = new cEntityFactory();

	return;
}

cEntityManager::~cEntityManager()
{
	this->_Cleanup();

	return;
}

sEntity* cEntityManager::GenerateEntityOfType(const std::string& prefabType)
{
	// Create base entity
	sEntity* newEntity = this->_pEntityFactory->CreateEntity(prefabType);
	
	if (newEntity == nullptr)
	{
		printf("Error creating entity of type %s!", prefabType.c_str());
		return nullptr;
	}

	// Add entity automatically to be updated?
	//this->_vecEntitiesToUpdate.push_back(newEntity);

	return newEntity;
}

void cEntityManager::UpdateEntities()
{
	this->_UpdateEntityChildren();
}

void cEntityManager::_ScheduleEntity(sEntity* entity, iComponent* locationC)
{
	_vecEntitiesToUpdate.push_back(entity);
}

void cEntityManager::_UpdateEntityChildren()
{
	if (this->_vecEntitiesToUpdate.size() <= 0)
	{
		return;
	}

	for (unsigned int i = 0; i < this->_vecEntitiesToUpdate.size(); i++)
	{
		sEntity* entity = this->_vecEntitiesToUpdate[i];
		unsigned int numChilds = entity->GetNumChilds();
		for (unsigned int j = 0; j < numChilds; j++)
		{
			sEntity* childEntity = entity->GetChildAt(j);
			childEntity->transform->GetWorldPosition() = entity->transform->GetWorldPosition() + childEntity->transform->GetWorldPosition();
		}
	}
}

void cEntityManager::_Cleanup()
{
	if (this->_pEntityFactory != nullptr)
	{
		delete this->_pEntityFactory;
		this->_pEntityFactory = nullptr;
	}
}
