// sApproachBehaviourComponent.cpp

#include <Entity\Components\Behaviours\sApproachBehaviourComponent.h>
#include <Entity\sEntity.h>


sApproachBehaviourComponent::sApproachBehaviourComponent(unsigned long long entityID)
{
	this->_parentID = entityID;
	this->Start();

	return;
}

sApproachBehaviourComponent::~sApproachBehaviourComponent()
{
	return;
}

void sApproachBehaviourComponent::Start()
{
	this->agent = nullptr;
	this->target = nullptr;
}

void sApproachBehaviourComponent::_SerializeToJson(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& prettyWriter)
{
}

void sApproachBehaviourComponent::_DeserializeFromJson(rapidjson::Value::ConstValueIterator vIter)
{
}
