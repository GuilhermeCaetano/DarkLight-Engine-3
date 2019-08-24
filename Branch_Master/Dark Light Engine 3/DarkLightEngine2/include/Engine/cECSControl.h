#ifndef _cECSControl_HG_
#define _cECSControl_HG_

/* cECSControl
 - Creates the ECS Managers
 - Responsible for calling and destroying the managers
 - Static calls to facilitate communication
*/

#include "cComponentManager.h"
#include "cEntityManager.h"

class cSystemManager;
class cVaoMeshManager;


namespace nPhysics
{
	class iPhysicsFactory;
}


class cECSControl
{
public:
	cECSControl();
	~cECSControl();

	void CreateECS();
	void DestroyECS();

	void UpdateECS(double deltaTime);

	// Static calls to facilitate communication
	// between parts inside the engine.
	static cEntityManager* GetEntityManager();
	static cComponentManager* GetComponentManager();
	static cSystemManager* GetSystemManager();
	static nPhysics::iPhysicsFactory* GetPhysicsFactory();

	// Sets physics factory. They depend on dll configuration
	// and will be set from the main engine side.
	void SetPhysicsFactory(nPhysics::iPhysicsFactory* pPhysicsFactory);

private:
	// These need to be static so they can 
	// be returned on the static functions.
	static cEntityManager* _pEntityManager;
	static cComponentManager* _pComponentManager;
	static cSystemManager* _pSystemManager;
	static cVaoMeshManager* _pVaoMeshManager;
	static nPhysics::iPhysicsFactory* _pPhysicsFactory;

};


#endif // !_cECSControl_HG_

