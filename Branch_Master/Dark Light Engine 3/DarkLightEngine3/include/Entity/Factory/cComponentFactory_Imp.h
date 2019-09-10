#ifndef _cComponentFactory_Imp_HG
#define _cComponentFactory_Imp_HG

#include <string>

struct iComponent;

class cComponentFactory_Imp
{
public:
	cComponentFactory_Imp();
	~cComponentFactory_Imp();

	iComponent* CreateComponentOfType(const std::string& type, unsigned long long entityID);

};



#endif // !_cComponentFactory_Imp_HG

