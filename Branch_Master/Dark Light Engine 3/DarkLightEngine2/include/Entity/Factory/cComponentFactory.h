#ifndef _cComponentFactory_HG_
#define _cComponentFactory_HG_

#include <Entity\Components\iComponent.h>


class cComponentFactory_Imp;
class cComponentFactory
{
public:
	cComponentFactory();
	~cComponentFactory();

	iComponent* CreateComponentOfType(const std::string& type, unsigned long long entityID);

private:
	cComponentFactory_Imp* _pFactoryImp;

};


#endif // !_cComponentFactory_HG_
