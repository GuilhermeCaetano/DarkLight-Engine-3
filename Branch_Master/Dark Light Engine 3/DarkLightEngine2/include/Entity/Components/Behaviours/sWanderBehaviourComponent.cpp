// sWanderBehaviourComponent.cpp

#include <Entity\Components\Behaviours\sWanderBehaviourComponent.h>
#include <Entity\sEntity.h>


sWanderBehaviourComponent::sWanderBehaviourComponent(unsigned long long entityID)
{
	this->_parentID = entityID;
	this->agent = nullptr;

	this->center = glm::vec3(0.0f);
	this->radius = 13.0f;
	this->minDistance = 1.0f;
	this->speed = 2.0f;

	return;
}

sWanderBehaviourComponent::~sWanderBehaviourComponent()
{
	return;
}

void sWanderBehaviourComponent::Start()
{
}

void sWanderBehaviourComponent::_SerializeToJson(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& prettyWriter)
{
	prettyWriter.Key("Center"); prettyWriter.StartArray();
	prettyWriter.Double(this->center.x); prettyWriter.Double(this->center.y); prettyWriter.Double(this->center.z); 
	prettyWriter.EndArray();
	prettyWriter.Key("Radius"); prettyWriter.Double((double)this->radius);
	prettyWriter.Key("MinDistance"); prettyWriter.Double((double)this->minDistance);
	prettyWriter.Key("Speed"); prettyWriter.Double((double)this->speed);
}

void sWanderBehaviourComponent::_DeserializeFromJson(rapidjson::Value::ConstValueIterator vIter)
{
	for (rapidjson::Value::ConstMemberIterator innerIter = vIter->MemberBegin(); innerIter != vIter->MemberEnd(); innerIter++)
	{
		if (innerIter->name == "Center")
		{
			this->center.x = (float)innerIter->value.GetArray()[0].GetDouble();
			this->center.y = (float)innerIter->value.GetArray()[1].GetDouble();
			this->center.z = (float)innerIter->value.GetArray()[2].GetDouble();
		}
		else if (innerIter->name == "Radius") { this->radius = (float)innerIter->value.GetDouble(); }
		else if (innerIter->name == "MinDistance") { this->minDistance = (float)innerIter->value.GetDouble(); }
		else if (innerIter->name == "Speed") { this->speed = (float)innerIter->value.GetDouble(); }
	}
}
