#ifndef _nCreateFactory_HG_
#define _nCreateFactory_HG_

#define EXTERN_DLL_EXPORT extern "C" __declspec(dllexport)

#include <Interfaces\Physics\iPhysicsFactory.h>

namespace nPhysics
{
	EXTERN_DLL_EXPORT nPhysics::iPhysicsFactory* CreateFactory();
}


#endif // !_nCreateFactory_HG_

