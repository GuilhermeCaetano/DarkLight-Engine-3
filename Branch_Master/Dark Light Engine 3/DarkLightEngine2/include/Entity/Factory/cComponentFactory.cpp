// cComponentFactory.cpp

#include <Entity\Factory\cComponentFactory.h>
#include <Entity\Factory\cComponentFactory_Imp.h>


cComponentFactory::cComponentFactory()
{
	this->_pFactoryImp = new cComponentFactory_Imp();

	return;
}

cComponentFactory::~cComponentFactory()
{
	if (this->_pFactoryImp)
	{
		delete this->_pFactoryImp;
		this->_pFactoryImp = nullptr;
	}

	return;
}

iComponent* cComponentFactory::CreateComponentOfType(const std::string& type, unsigned long long entityID)
{
	return this->_pFactoryImp->CreateComponentOfType(type, entityID);
}
