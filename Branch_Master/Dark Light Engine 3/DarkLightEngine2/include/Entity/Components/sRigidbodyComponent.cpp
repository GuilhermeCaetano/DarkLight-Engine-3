// sRigidbodyComponent.cpp

#include <Entity\Components\sRigidbodyComponent.h>

sRigidbodyComponent::sRigidbodyComponent()
{
	// Setup
	this->_componentDef._eComponentType = RIGIDBODY_COMPONENT;
	this->_componentDef._componentType = "RigidbodyComponent";

	this->acceleration = glm::vec3(0.0f);
	this->deceleration = glm::vec3(0.5f);
	this->velocity = glm::vec3(0.0f);

	this->angularAcceleration = glm::vec3(0.0f);
	this->angularVelocity = glm::vec3(0.0f);

	this->minMaxAcceleration = glm::vec2(0.0f, 10.0f);
	this->minMaxVelocity = glm::vec2(0.0f, 10.0f);
	this->minMaxAngularAcceleration = glm::vec2(-1.0f, 1.0f);
	this->minMaxAngularVelocity = glm::vec2(-1.0f, 1.0f);

	this->gravityModifier = glm::vec3(0.0f);
	this->mass = 5.0f;
	this->invMass = 1.0f / this->mass;
	this->radius = 1.0f;
	this->radiusAdjust = 0.0f;

	this->bUseGravity = false;
	this->bIsKinematic = false;

	return;
}

sRigidbodyComponent::~sRigidbodyComponent()
{
	return;
}

void sRigidbodyComponent::_SerializeComponentToJson(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& prettyWriter)
{
	prettyWriter.Key("PhysicsType"); prettyWriter.String(this->physicsType.c_str());
	prettyWriter.Key("MinMaxAcceleration"); prettyWriter.StartArray();
	prettyWriter.Double((double)this->minMaxAcceleration.x); prettyWriter.Double((double)this->minMaxAcceleration.y);
	prettyWriter.EndArray();
	prettyWriter.Key("MinMaxVelocity"); prettyWriter.StartArray();
	prettyWriter.Double((double)this->minMaxVelocity.x); prettyWriter.Double((double)this->minMaxVelocity.y);
	prettyWriter.EndArray();
	prettyWriter.Key("MinMaxAngularAcceleration"); prettyWriter.StartArray();
	prettyWriter.Double((double)this->minMaxAngularAcceleration.x); prettyWriter.Double((double)this->minMaxAngularAcceleration.y);
	prettyWriter.EndArray();
	prettyWriter.Key("MinMaxAngularVelocity"); prettyWriter.StartArray();
	prettyWriter.Double((double)this->minMaxAngularVelocity.x); prettyWriter.Double((double)this->minMaxAngularVelocity.y);
	prettyWriter.EndArray();
	prettyWriter.Key("GravityModifier"); prettyWriter.StartArray(); 
	prettyWriter.Double((double)this->gravityModifier.x); prettyWriter.Double((double)this->gravityModifier.y); prettyWriter.Double((double)this->gravityModifier.z);
	prettyWriter.EndArray();
	prettyWriter.Key("Mass"); prettyWriter.Double((double)this->mass);
	prettyWriter.Key("IsKinematic"); prettyWriter.Bool(this->bIsKinematic);
	prettyWriter.Key("UseGravity"); prettyWriter.Bool(this->bUseGravity);
}

void sRigidbodyComponent::_DeserializeComponentFromJson(rapidjson::Value::ConstValueIterator vIter)
{
	for (rapidjson::Value::ConstMemberIterator innerIter = vIter->MemberBegin(); innerIter != vIter->MemberEnd(); innerIter++)
	{
		if (innerIter->name == "PhysicsType") { this->physicsType = innerIter->value.GetString(); }
		else if (innerIter->name == "MinMaxAcceleration") { this->minMaxAcceleration.x = innerIter->value.GetArray()[0].GetDouble(); this->minMaxAcceleration.y = innerIter->value.GetArray()[1].GetDouble(); }
		else if (innerIter->name == "MinMaxVelocity") { this->minMaxVelocity.x = innerIter->value.GetArray()[0].GetDouble(); this->minMaxVelocity.y = innerIter->value.GetArray()[1].GetDouble(); }
		else if (innerIter->name == "MinMaxAngularAcceleration") { this->minMaxAngularAcceleration.x = innerIter->value.GetArray()[0].GetDouble(); this->minMaxAngularAcceleration.y = innerIter->value.GetArray()[1].GetDouble(); }
		else if (innerIter->name == "MinMaxAngularVelocity") { this->minMaxAngularVelocity.x = innerIter->value.GetArray()[0].GetDouble(); this->minMaxAngularVelocity.y = innerIter->value.GetArray()[1].GetDouble(); }
		else if (innerIter->name == "GravityModifier") { this->gravityModifier.x = innerIter->value.GetArray()[0].GetDouble(); this->gravityModifier.y = innerIter->value.GetArray()[1].GetDouble(); this->gravityModifier.z = innerIter->value.GetArray()[2].GetDouble(); }
		else if (innerIter->name == "Mass") { this->mass = innerIter->value.GetDouble(); }
		else if (innerIter->name == "IsKinematic") { this->bIsKinematic = innerIter->value.GetBool(); }
		else if (innerIter->name == "UseGravity") { this->bUseGravity = innerIter->value.GetBool(); }
	}
}
