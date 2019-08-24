// sIdleBehaviourComponent.cpp

#include <Entity\Components\Behaviours\sIdleBehaviourComponent.h>
#include <Entity\sEntity.h>


sIdleBehaviourComponent::sIdleBehaviourComponent(unsigned long long entityID)
{
	this->_parentID = entityID;
	this->agent = nullptr;
	this->idle = true;
	this->idleTime = 5.0f;

	return;
}

sIdleBehaviourComponent::~sIdleBehaviourComponent()
{
	return;
}

void sIdleBehaviourComponent::Start()
{
}

void sIdleBehaviourComponent::_SerializeToJson(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& prettyWriter)
{
	prettyWriter.Key("IdleTime"); prettyWriter.Double((double)this->idleTime);
}

void sIdleBehaviourComponent::_DeserializeFromJson(rapidjson::Value::ConstValueIterator vIter)
{
	for (rapidjson::Value::ConstMemberIterator innerIter = vIter->MemberBegin(); innerIter != vIter->MemberEnd(); innerIter++)
	{
		if (innerIter->name == "IdleTime") { this->_parentID = (float)innerIter->value.GetDouble(); }
	}
}
