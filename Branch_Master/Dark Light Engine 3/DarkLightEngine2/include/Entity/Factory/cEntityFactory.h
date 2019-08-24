#ifndef _cEntityFactory_HG_
#define _cEntityFactory_HG_

#include <string>

struct sEntity;
class cEntityFactory_Imp;


class cEntityFactory
{
public:
	cEntityFactory();
	~cEntityFactory();

	sEntity* CreateEntity(const std::string& prefabType);

private:
	cEntityFactory_Imp* _pEntityFactoryImp;

};


#endif // !_cEntityFactory_HG_

