#ifndef _cEntityFactory_Imp_HG_
#define _cEntityFactory_Imp_HG_

#include <string>


struct sEntity;
class cEntityBuilder;
class cEntityFactory_Imp
{
public:
	cEntityFactory_Imp();
	~cEntityFactory_Imp();

	sEntity* CreateEntity(const std::string& prefabType);

private:
	//bool _CheckPrefabType(const std::string& entityType);

	cEntityBuilder* _pEntityBuilder;

};


#endif // !_cEntityFactory_Imp_HG_

