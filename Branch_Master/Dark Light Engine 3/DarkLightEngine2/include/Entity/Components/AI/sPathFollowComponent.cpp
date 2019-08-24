// sPathFollowComponent.cpp

#include "sPathFollowComponent.h"


sPathFollowComponent::sPathFollowComponent(unsigned long long entityID)
{
	this->_parentID = entityID;
	this->Start();

	return;
}

sPathFollowComponent::~sPathFollowComponent()
{
	return;
}

void sPathFollowComponent::Start()
{
	
}

void sPathFollowComponent::_SerializeToJson(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& prettyWriter)
{

}

void sPathFollowComponent::_DeserializeFromJson(rapidjson::Value::ConstValueIterator iter)
{

}
