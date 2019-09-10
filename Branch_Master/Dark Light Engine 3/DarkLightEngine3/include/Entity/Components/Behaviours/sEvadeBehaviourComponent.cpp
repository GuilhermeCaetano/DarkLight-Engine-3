// sEvadeBehaviourComponent.cpp

#include <Entity\Components\Behaviours\sEvadeBehaviourComponent.h>
#include <Entity\sEntity.h>


sEvadeBehaviourComponent::sEvadeBehaviourComponent(unsigned long long entityID)
{
	this->_parentID = entityID;
	this->Start();

	return;
}

sEvadeBehaviourComponent::~sEvadeBehaviourComponent()
{
	return;
}

void sEvadeBehaviourComponent::Start()
{
	this->behaviourType = eBehaviourTypes::CLOSEST;
	this->agent = nullptr;
	this->target = nullptr;
}

void sEvadeBehaviourComponent::_SerializeToJson(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& prettyWriter)
{
	prettyWriter.Key("BehaviourType"); prettyWriter.String(this->BehaviourEnumToString(this->behaviourType).c_str());
}

void sEvadeBehaviourComponent::_DeserializeFromJson(rapidjson::Value::ConstValueIterator vIter)
{
	for (rapidjson::Value::ConstMemberIterator innerIter = vIter->MemberBegin(); innerIter != vIter->MemberEnd(); innerIter++)
	{
		if (innerIter->name == "BehaviourType") { this->behaviourType = this->BehaviourStringToBehaviourEnum(innerIter->value.GetString()); }
	}
}
