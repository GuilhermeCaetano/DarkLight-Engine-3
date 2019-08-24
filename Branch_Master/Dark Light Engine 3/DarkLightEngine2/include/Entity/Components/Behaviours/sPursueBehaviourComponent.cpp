// sChaseBehaviourComponent.cpp

#include <Entity\Components\Behaviours\sPursueBehaviourComponent.h>
#include <Entity\sEntity.h>


sPursueBehaviourComponent::sPursueBehaviourComponent(unsigned long long entityID)
{
	this->_parentID = entityID;
	this->agent = nullptr;
	this->target = nullptr;

	return;
}

sPursueBehaviourComponent::~sPursueBehaviourComponent()
{
	return;
}

void sPursueBehaviourComponent::Start()
{
}

void sPursueBehaviourComponent::_SerializeToJson(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& prettyWriter)
{
}

void sPursueBehaviourComponent::_DeserializeFromJson(rapidjson::Value::ConstValueIterator vIter)
{
}
