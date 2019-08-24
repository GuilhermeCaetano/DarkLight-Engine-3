#ifndef _nCreateBulletFactory_HG_
#define _nCreateBulletFactory_HG_

#define EXTERN_DLL_EXPORT extern "C" __declspec(dllexport)

#include <cBulletPhysicsFactory.h>

namespace nPhysics
{
	EXTERN_DLL_EXPORT nPhysics::iPhysicsFactory* CreateFactory();
}



#endif // !_nCreateBulletFactory_HG_

