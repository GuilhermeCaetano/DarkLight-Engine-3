// sLightComponent.cpp

#include <Entity\Components\sLightComponent.h>
#include <glm\vec4.hpp>


sLightComponent::sLightComponent(unsigned long long entityID)
{
	this->_parentID = entityID;
	this->Start();

	return;
}

sLightComponent::~sLightComponent()
{
	return;
}

void sLightComponent::Start()
{
	this->SetLightType(POINT_LIGHT);
	this->SetPosition(glm::vec4(0.0f));
	this->SetDiffuseColour(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	this->SetConstantAttenuation(0.001f);
	this->SetLinearAttenuation(0.001f);
	this->SetQuadraticAttenuation(0.0001f);
	this->SetSpotConeAngles(10.0f, 30.0f);
	this->SetRelativeDirection(glm::vec3(0.0f, -1.0f, 0.0f));
	this->bisWireframeOn = false;
	this->SetLightTurnedOn(true);
}

bool sLightComponent::GetIsLightOn()
{
	return (bool)this->param2.x;
}

void sLightComponent::SetLightType(sLightType lightType)
{
	switch (lightType)
	{
		case POINT_LIGHT:
			this->param1.x = 0.0f;
		break;
		case DIRECTIONAL_LIGHT:
			this->param1.x = 1.0f;
		break;
		case SPOT_LIGHT:
			this->param1.x = 2.0f;
		break;
		default:
			this->param1.x = 0.0f; // In any errors, set to point light
		break;
	}
}

void sLightComponent::SetPosition(glm::vec4 lightPos)
{
	this->position = lightPos;
}

void sLightComponent::SetDiffuseColour(glm::vec4 lightColour)
{
	this->diffuse = lightColour;
}

void sLightComponent::SetConstantAttenuation(float constAtten)
{
	this->attenuation.x = constAtten;
}

void sLightComponent::SetLinearAttenuation(float linAtten)
{
	this->attenuation.y = linAtten;
}

void sLightComponent::SetQuadraticAttenuation(float quadAtten)
{
	this->attenuation.z = quadAtten;
}

void sLightComponent::SetLightTurnedOn(bool turnedOn)
{
	this->param2.x = (float)turnedOn;
}

void sLightComponent::SetSpotConeAngles(float innerAngleDegrees, float outerAngleDegrees)
{
	this->param1.y = innerAngleDegrees;
	this->param1.z = outerAngleDegrees;
}

void sLightComponent::SetRelativeDirection(glm::vec3 direction)
{
	this->direction = glm::vec4(direction, 1.0f);
}

void sLightComponent::SetRelativeDirectionByEulerAngles(glm::vec3 directionAngle)
{
	// Take the angles and make a quaternion out of them
	// Use a mat4x4 x vec4 tranformation (just like the shader or in physics)
	// Use the final xyz location to send to SetRelativeDirectionByLookAt()

	// Ta-Da!

	return;
}

void sLightComponent::SetRelativeDirectionByLookAt(glm::vec3 pointInWorldXYZ)
{
	// The vector from what I'm looking at to where I am, then normalize
	glm::vec3 lookVector = pointInWorldXYZ - glm::vec3(this->position);
	lookVector = glm::normalize(lookVector);
	this->SetRelativeDirection(lookVector);
}

void sLightComponent::_SerializeToJson(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& prettyWriter)
{
	prettyWriter.Key("Position"); prettyWriter.StartArray();
	prettyWriter.Double((double)this->position.x); prettyWriter.Double((double)this->position.y); prettyWriter.Double((double)this->position.z);
	prettyWriter.EndArray();
	prettyWriter.Key("Diffuse"); prettyWriter.StartArray();
	prettyWriter.Double((double)this->diffuse.r); prettyWriter.Double((double)this->diffuse.g); prettyWriter.Double((double)this->diffuse.b); prettyWriter.Double((double)this->diffuse.a);
	prettyWriter.EndArray();
	prettyWriter.Key("Attenuation"); prettyWriter.StartArray();
	prettyWriter.Double((double)this->attenuation.x); prettyWriter.Double((double)this->attenuation.y); prettyWriter.Double((double)this->attenuation.z); prettyWriter.Double((double)this->attenuation.w);
	prettyWriter.EndArray();
	prettyWriter.Key("Direction"); prettyWriter.StartArray();
	prettyWriter.Double((double)this->direction.x); prettyWriter.Double((double)this->direction.y); prettyWriter.Double((double)this->direction.z); prettyWriter.Double((double)this->direction.w);
	prettyWriter.EndArray();
	prettyWriter.Key("Param1"); prettyWriter.StartArray();
	prettyWriter.Double((double)this->param1.x); prettyWriter.Double((double)this->param1.y); prettyWriter.Double((double)this->param1.z); prettyWriter.Double((double)this->param1.w);
	prettyWriter.EndArray();
	prettyWriter.Key("Param2"); prettyWriter.StartArray();
	prettyWriter.Double((double)this->param2.x); prettyWriter.Double((double)this->param2.y); prettyWriter.Double((double)this->param2.z); prettyWriter.Double((double)this->param2.w);
	prettyWriter.EndArray();
	prettyWriter.Key("IsWireframeOn"); prettyWriter.Bool(this->bisWireframeOn);
}

void sLightComponent::_DeserializeFromJson(rapidjson::Value::ConstValueIterator vIter)
{
	for (rapidjson::Value::ConstMemberIterator innerIter = vIter->MemberBegin(); innerIter != vIter->MemberEnd(); innerIter++)
	{
		if (innerIter->name == "Position") { this->position.x = innerIter->value.GetArray()[0].GetDouble(); this->position.y = innerIter->value.GetArray()[1].GetDouble(); this->position.z = innerIter->value.GetArray()[2].GetDouble(); }
		else if (innerIter->name == "Diffuse") { this->diffuse.r = innerIter->value.GetArray()[0].GetDouble(); this->diffuse.g = innerIter->value.GetArray()[1].GetDouble(); this->diffuse.b = innerIter->value.GetArray()[2].GetDouble(); this->diffuse.a = innerIter->value.GetArray()[3].GetDouble(); }
		else if (innerIter->name == "Attenuation") { this->attenuation.x = innerIter->value.GetArray()[0].GetDouble(); this->attenuation.y = innerIter->value.GetArray()[1].GetDouble(); this->attenuation.z = innerIter->value.GetArray()[2].GetDouble(); this->attenuation.w = innerIter->value.GetArray()[3].GetDouble(); }
		else if (innerIter->name == "Direction") { this->direction.x = innerIter->value.GetArray()[0].GetDouble(); this->direction.y = innerIter->value.GetArray()[1].GetDouble(); this->direction.z = innerIter->value.GetArray()[2].GetDouble(); this->direction.w = innerIter->value.GetArray()[3].GetDouble(); }
		else if (innerIter->name == "Param1") { this->param1.x = innerIter->value.GetArray()[0].GetDouble(); this->param1.y = innerIter->value.GetArray()[1].GetDouble(); this->param1.z = innerIter->value.GetArray()[2].GetDouble(); this->param1.w = innerIter->value.GetArray()[3].GetDouble(); }
		else if (innerIter->name == "Param2") { this->param2.x = innerIter->value.GetArray()[0].GetDouble(); this->param2.y = innerIter->value.GetArray()[1].GetDouble(); this->param2.z = innerIter->value.GetArray()[2].GetDouble(); this->param2.w = innerIter->value.GetArray()[3].GetDouble(); }
		else if (innerIter->name == "IsWireframeOn") { this->bisWireframeOn = innerIter->value.GetBool(); }
	}
}
