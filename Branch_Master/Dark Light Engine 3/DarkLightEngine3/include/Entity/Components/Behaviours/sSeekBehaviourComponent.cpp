// sSeekBehaviourComponent.cpp

#include <Entity\Components\Behaviours\sSeekBehaviourComponent.h>

sSeekBehaviourComponent::sSeekBehaviourComponent(unsigned long long entityID)
{
	this->_parentID = entityID;
	this->behaviourType = eBehaviourTypes::BOO;
	this->agent = nullptr;
	this->target = nullptr;

	return;
}

sSeekBehaviourComponent::~sSeekBehaviourComponent()
{
	return;
}

void sSeekBehaviourComponent::Start()
{
}

void sSeekBehaviourComponent::_SerializeToJson(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& prettyWriter)
{

}

void sSeekBehaviourComponent::_DeserializeFromJson(rapidjson::Value::ConstValueIterator vIter)
{

}
