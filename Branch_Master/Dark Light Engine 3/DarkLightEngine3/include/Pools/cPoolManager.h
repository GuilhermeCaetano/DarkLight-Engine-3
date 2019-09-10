#ifndef _cPoolManager_HG_
#define _cPoolManager_HG_

#include <Entity\sEntity.h>
#include <vector>

class cPoolManager
{
public:
	static cPoolManager* GetInstance();
	~cPoolManager();

	void AddEntityOfType(std::string entityPoolType, sEntity* entity);
	sEntity* GetEntityOfType(std::string entityPoolType);
	sEntity* GetEntityByName(std::string entityName, bool isActive = true);
	sEntity* GetEntityByID(unsigned long long ID);

	std::vector<sEntity*> vecStaticEntities;
	std::vector<sEntity*> vecBulletEntities;

	std::vector<iComponent*> vecLightComponents;

private:
	static cPoolManager* _pPoolManager;
	cPoolManager();

	
};



#endif // !_cPoolManager_HG_

