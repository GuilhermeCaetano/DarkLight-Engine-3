#ifndef _nPhysicsConfigLoader_HG_
#define _nPhysicsConfigLoader_HG_

#include <Engine\Configuration\sConfig.h>
#include <Interfaces\Physics\iPhysicsFactory.h>
#include <string>


namespace nConfig
{
	bool LoadPhysicsDLL(std::string& pathName, sConfig& outConfig);

	// Create physics factory from dll and returns the interface.
	nPhysics::iPhysicsFactory* CreatePhysicsFactory(HINSTANCE& hProckDLL, const sConfig& config);
}



#endif // !_nPhysicsConfigLoader_HG_

