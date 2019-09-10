#ifndef _cCommandFactory_Imp_HG_
#define _cCommandFactory_Imp_HG_

#include "iCommand.h"
#include <string>

class cCommandFactory_Imp
{
public:
	cCommandFactory_Imp(); // c'tor
	~cCommandFactory_Imp(); // d'tor

	// Check if the command type is valid
	bool CheckCommandType(std::string& commandType);

	// Create a new command if valid
	iCommand* CreateCommand(std::string& commandType, rapidjson::StringBuffer& buffer, unsigned long long uniqueID);
};



#endif // !_cCommandFactory_Imp_HG_

