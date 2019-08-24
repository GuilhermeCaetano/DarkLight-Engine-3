// sFleeBehaviourComponent.cpp

#include <Entity\Components\Behaviours\sFleeBehaviourComponent.h>
#include <Entity\sEntity.h>


sFleeBehaviourComponent::sFleeBehaviourComponent(unsigned long long entityID)
{
	this->_parentID = entityID;
	this->behaviourType = eBehaviourTypes::BOO;
	this->agent = nullptr;
	this->target = nullptr;

	return;
}

sFleeBehaviourComponent::~sFleeBehaviourComponent()
{
	return;
}

void sFleeBehaviourComponent::Start()
{
}

void sFleeBehaviourComponent::_SerializeToJson(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& prettyWriter)
{
	prettyWriter.Key("BehaviourType"); prettyWriter.String(this->BehaviourEnumToString(this->behaviourType).c_str());
}

void sFleeBehaviourComponent::_DeserializeFromJson(rapidjson::Value::ConstValueIterator vIter)
{
	for (rapidjson::Value::ConstMemberIterator innerIter = vIter->MemberBegin(); innerIter != vIter->MemberEnd(); innerIter++)
	{
		if (innerIter->name == "BehaviourType") { this->behaviourType = this->BehaviourStringToBehaviourEnum(innerIter->value.GetString()); }
	}
}
