// nCreateBulletFactory.cpp

#include <nCreateBulletFactory.h>
#include <cBulletPhysicsFactory.h>

namespace nPhysics
{
	EXTERN_DLL_EXPORT iPhysicsFactory* CreateFactory()
	{
		return new cBulletPhysicsFactory();
	}
}
