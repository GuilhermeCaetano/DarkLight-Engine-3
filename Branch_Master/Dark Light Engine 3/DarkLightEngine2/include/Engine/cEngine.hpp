#ifndef _cEngine_HG_
#define _cEngine_HG_

/* cEngine
 - An actual PIMPL of the cECSControl class.
 - It gives easy user control while hiding the complex and
   dangerous Engine calls that users shouldn't have easy access to.
 - Should be included in most places, like scenes, for functionality
*/

#include "cECSControl.h"
#include "Graphics\cGraphicsControl.h"
#include <Interfaces\Physics\sRigidbodyDef.h>
#include <Interfaces\Physics\sSoftbodyDef.h>
#include <Interfaces\Physics\sCompoundbodyDef.h>
#include <string>

struct sEntity;
struct sModelDrawInfo;

extern cECSControl* gECSControl;
extern cGraphicsControl* gGraphicsControl;

class cEngine
{
public:
	cEngine();
	~cEngine();

	static sEntity* CreateEntity();
	static sEntity* CreateEntity(const std::string& prefabType);

	static bool FindMeshByName(const std::string& meshName, sModelDrawInfo** modelInfo);

	template <typename T>
	static T* CreateComponent(unsigned long long entityID)
	{
		T* component = gECSControl->GetComponentManager()->template CreateComponent<T>(entityID);
		return component;
	}

	template <typename T>
	static T* CreateComponent(nPhysics::iPhysicsDef& def, unsigned long long entityID)
	{
		T* component = gECSControl->GetComponentManager()->template CreateComponent<T>(def, entityID);
		return component;
	}

};


#endif // !_cEngine.hpp_HG_

