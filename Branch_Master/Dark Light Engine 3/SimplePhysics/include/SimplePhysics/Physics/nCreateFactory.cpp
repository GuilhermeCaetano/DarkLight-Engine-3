// nCreateFactory.cpp

#include <SimplePhysics\Physics\nCreateFactory.h>
#include <SimplePhysics\Physics\cSimplePhysicsFactory.h>

namespace nPhysics
{
	EXTERN_DLL_EXPORT nPhysics::iPhysicsFactory* CreateFactory()
	{
		return new cSimplePhysicsFactory();
	}
}
