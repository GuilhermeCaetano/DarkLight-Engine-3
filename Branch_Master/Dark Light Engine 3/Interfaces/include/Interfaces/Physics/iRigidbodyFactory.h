#ifndef _iRigidbodyFactory_HG_
#define _iRigidbodyFactory_HG_

#include <Interfaces\Physics\iRigidbody.h>
#include <Interfaces\Physics\sRigidbodyDef.h>

namespace nPhysics
{
	class iRigidbodyFactory
	{
	public:
		virtual ~iRigidbodyFactory() {};
		virtual iRigidbody* CreateRigidbody(nPhysics::sRigidbodyDef& other) = 0;
	};
}


#endif // !_iRigidbodyFactory_HG_
