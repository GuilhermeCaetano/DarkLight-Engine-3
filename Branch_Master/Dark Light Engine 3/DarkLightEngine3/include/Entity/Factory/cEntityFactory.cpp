// cEntityFactory.cpp

#include <Entity\Factory\cEntityFactory.h>
#include <Entity\Factory\cEntityFactory_Imp.h>
#include <Entity\sEntity.h>


cEntityFactory::cEntityFactory()
{
	this->_pEntityFactoryImp = new cEntityFactory_Imp();

	return;
}

cEntityFactory::~cEntityFactory()
{
	delete this->_pEntityFactoryImp;

	return;
}

sEntity* cEntityFactory::CreateEntity(const std::string& prefabType)
{
	return this->_pEntityFactoryImp->CreateEntity(prefabType);
}
