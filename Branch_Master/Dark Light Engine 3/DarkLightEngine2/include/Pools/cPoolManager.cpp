// cPoolManager.cpp

#include <Pools\cPoolManager.h>

cPoolManager* cPoolManager::_pPoolManager = nullptr;

cPoolManager* cPoolManager::GetInstance()
{
	if (_pPoolManager == nullptr)
	{
		_pPoolManager = new cPoolManager();
	}

	return _pPoolManager;
}

cPoolManager::cPoolManager()
{
	return;
}

cPoolManager::~cPoolManager()
{
	return;
}

void cPoolManager::AddEntityOfType(std::string entityPoolType, sEntity* entity)
{
	if (entityPoolType == "StaticEntity")
	{
		this->vecStaticEntities.push_back(entity);
		return;
	}
	else if (entityPoolType == "BulletEntity")
	{
		this->vecBulletEntities.push_back(entity);
		return;
	}

	printf("Error in finding pool of type %s to add!\n", entityPoolType.c_str());
}

sEntity* cPoolManager::GetEntityOfType(std::string entityPoolType)
{
	if (entityPoolType == "StaticEntity")
	{
		return this->vecStaticEntities[0];
	}

	printf("Error in pool %s type!\n", entityPoolType.c_str());
	return nullptr;
}

sEntity* cPoolManager::GetEntityByName(std::string entityName, bool isActive)
{
	for (int i = 0; i != this->vecStaticEntities.size(); i++)
	{
		if (this->vecStaticEntities[i]->GetName() == entityName && this->vecStaticEntities[i]->GetIsActive() == isActive)
		{
			return this->vecStaticEntities[i];
		}
	}

	//HACK: Again, if we had a definitive type, we would just pass that..
	for (int i = 0; i != this->vecBulletEntities.size(); i++)
	{
		if (this->vecBulletEntities[i]->GetName() == entityName && !this->vecBulletEntities[i]->GetIsActive())
		{
			return this->vecBulletEntities[i];
		}
	}

	printf("No Entity with the name %s was found!\n", entityName.c_str());
	return nullptr;
}

sEntity* cPoolManager::GetEntityByID(unsigned long long ID)
{
	for (int i = 0; i != this->vecStaticEntities.size(); i++)
	{
		if (this->vecStaticEntities[i]->GetInstanceID() == ID)
		{
			return this->vecStaticEntities[i];
		}
	}

	//HACK: Again, if we had a definitive type, we would just pass that..
	for (int i = 0; i != this->vecBulletEntities.size(); i++)
	{
		if (this->vecBulletEntities[i]->GetInstanceID() == ID)
		{
			return this->vecBulletEntities[i];
		}
	}

	printf("No Entity with the ID %d was found!\n", ID);
	return nullptr;
}
