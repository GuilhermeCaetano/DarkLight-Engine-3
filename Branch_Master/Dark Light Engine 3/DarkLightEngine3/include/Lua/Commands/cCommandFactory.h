#ifndef _cCommandFactory_HG_
#define _cCommandFactory_HG_

#include "iCommand.h"
#include <string>

// Forward declaration to keep this from
// knowing too much from the implementation and blocking access
class cCommandFactory_Imp; 

class cCommandFactory
{
public:
	cCommandFactory(); // c'tor
	~cCommandFactory(); // d'tor

	// Check if the command type is valid
	bool CheckCommandType(std::string& commandType);

	// Create a new command if valid
	iCommand* CreateCommand(std::string& commandType, rapidjson::StringBuffer& buffer, unsigned long long uniqueID);

private:
	cCommandFactory_Imp* _factoryImp; // The actual factory
};



#endif // !_cCommandFactory_HG_

