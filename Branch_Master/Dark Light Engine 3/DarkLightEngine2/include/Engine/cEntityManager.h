#ifndef _cEntityManager_HG_
#define _cEntityManager_HG_

#include <vector>

struct sEntity;
struct iComponent;

class cEntityFactory;


class cEntityManager
{
public:
	cEntityManager();
	~cEntityManager();

	// Generates an entity from a prefab. Calling "EmptyPrefab" will create only the entity.
	sEntity* GenerateEntityOfType(const std::string& prefabType);

	// Update the assigned entities.
	void UpdateEntities();

private:
	// Set entities here for easy access
	void _ScheduleEntity(sEntity* entity, iComponent* locationC);
	void _UpdateEntityChildren();

	// Clean the manager
	void _Cleanup();

	// Vector of entities assigned for updates here
	std::vector<sEntity*> _vecEntitiesToUpdate;

	// The PIMPL for the entity factory
	cEntityFactory* _pEntityFactory;

};



#endif // !_cEntityManager_HG_

