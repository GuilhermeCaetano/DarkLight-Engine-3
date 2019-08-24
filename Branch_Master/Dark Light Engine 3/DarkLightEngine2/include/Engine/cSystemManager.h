#ifndef _cSystemManager_HG_
#define _cSystemManager_HG_

#include <vector>


class iSystem;
class cSystemManager
{
public:
	cSystemManager();
	~cSystemManager();

	// Create and manage memory for all systems needed for the engine
	void CreateSystems();

	// Update all created systems
	void UpdateSystems(double deltaTime);
	
	// Delete and cleanup
	void DeleteSystems();

private:
	void _AddSystem(iSystem* ecsSystem);

	std::vector<iSystem*> _vecSystems;

};

#endif // !_cSystemManager_HG_

