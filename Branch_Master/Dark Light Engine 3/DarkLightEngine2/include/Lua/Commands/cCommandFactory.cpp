// cCommandFactory.cpp
#include "cCommandFactory.h"
#include "cCommandFactory_Imp.h"

cCommandFactory::cCommandFactory()
{
	this->_factoryImp = new cCommandFactory_Imp();
}

cCommandFactory::~cCommandFactory()
{
	delete this->_factoryImp;
}

bool cCommandFactory::CheckCommandType(std::string& commandType)
{
	return this->_factoryImp->CheckCommandType(commandType);
}

iCommand* cCommandFactory::CreateCommand(std::string& commandType, rapidjson::StringBuffer& buffer, unsigned long long uniqueID)
{
	return this->_factoryImp->CreateCommand(commandType, buffer, uniqueID);
}