// sLocationComponent.cpp

#include <Entity\Components\sLocationComponent.h>
#include <Math\nQuaternionHelper.h>
#include <PhyMath\nPhyMath.h>


sLocationComponent::sLocationComponent(unsigned long long entityID)
{
	this->_parentID = entityID;
	this->Start();

	return;
}

sLocationComponent::~sLocationComponent()
{
	return;
}

void sLocationComponent::Start()
{
	this->_position = glm::vec3(0.0f);
	this->_qOrientation = glm::quat(glm::vec3(0.0f));

	this->_frontOfObject = glm::vec3(0.0f, 0.0f, 1.0f);
	this->_rightOfObject = glm::vec3(1.0f, 0.0f, 0.0f);
	this->_upOfObject = glm::vec3(0.0f, 1.0f, 0.0f);

	this->_forward = glm::vec3(0.0f, 0.0f, 1.0f);
	this->_right = glm::vec3(1.0f, 0.0f, 0.0f);
	this->_up = glm::vec3(0.0f, 1.0f, 0.0f);

	this->_lastPosition = glm::vec3(0.0f);
	this->_respawnPosition = glm::vec3(0.0f);
	this->_safeDistancePosition = 10.0f;

	this->_bChangedPosition = false;
	this->_bChangedRotation = false;
}

glm::vec3 sLocationComponent::GetWorldPosition()
{
	return this->_position;
}

void sLocationComponent::SetWorldPosition(const glm::vec3& position)
{
	if (this->_position == position)
	{
		return;
	}

	this->_position = position;
	this->_bChangedPosition = true;

	// Check if we can save this as a last position. Count this only for X and Z.
	float difX = fabs(this->_position.x - this->_lastPosition.x);
	float difZ = fabs(this->_position.z - this->_lastPosition.z);
	if (difX > this->_safeDistancePosition || difZ > this->_safeDistancePosition)
	{
		this->_lastPosition = this->_position;
		this->_respawnPosition = this->_position;
	}
}

glm::quat sLocationComponent::GetWorldRotation()
{
	return this->_qOrientation;
}

void sLocationComponent::SetWorldRotation(const glm::quat& rotation)
{
	if (this->_qOrientation == rotation)
	{
		return;
	}

	this->_qOrientation = rotation;
	this->_bChangedRotation = true;
}

glm::vec3 sLocationComponent::GetForwardVector()
{
	// Recalculate forward
	this->_CheckVectorOrientation();
	return this->_forward;
}

glm::vec3 sLocationComponent::GetUpVector()
{
	this->_CheckVectorOrientation();
	return this->_up;
}

glm::vec3 sLocationComponent::GetRightVector()
{
	this->_CheckVectorOrientation();
	return this->_right;
}

glm::vec3 sLocationComponent::GetLastPosition()
{
	return this->_lastPosition;
}

glm::vec3 sLocationComponent::GetRespawnPosition()
{
	return this->_respawnPosition;
}

void sLocationComponent::_CheckVectorOrientation()
{
	// Check if we need to recalculate the vectors
	if (this->_bChangedRotation)
	{
		nPhyMath::GetRelativeAxis(this->_frontOfObject, this->GetWorldRotation(), this->_forward);
		nPhyMath::GetRelativeAxis(this->_upOfObject, this->GetWorldRotation(), this->_up);
		nPhyMath::GetRelativeAxis(this->_rightOfObject, this->GetWorldRotation(), this->_right);
		this->_bChangedRotation = false;
	}
}

void sLocationComponent::_SerializeToJson(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& prettyWriter)
{
	prettyWriter.Key("Position"); prettyWriter.StartArray();
	prettyWriter.Double((double)this->_position.x); prettyWriter.Double((double)this->_position.y); prettyWriter.Double((double)this->_position.z);
	prettyWriter.EndArray();
	prettyWriter.Key("Rotation"); prettyWriter.StartArray();
	glm::vec3 eulerQuat = glm::degrees(glm::eulerAngles(this->_qOrientation));
	prettyWriter.Double((double)eulerQuat.x); prettyWriter.Double((double)eulerQuat.y); prettyWriter.Double((double)eulerQuat.z);
	prettyWriter.EndArray();
}

void sLocationComponent::_DeserializeFromJson(rapidjson::Value::ConstValueIterator vIter)
{
	for (rapidjson::Value::ConstMemberIterator innerIter = vIter->MemberBegin(); innerIter != vIter->MemberEnd(); innerIter++)
	{
		if (innerIter->name == "Position") { this->_position.x = (float)innerIter->value.GetArray()[0].GetDouble(); this->_position.y = (float)innerIter->value.GetArray()[1].GetDouble(); this->_position.z = (float)innerIter->value.GetArray()[2].GetDouble(); }
		else if (innerIter->name == "Rotation") 
		{
			float x, y, z;
			x = (float)innerIter->value.GetArray()[0].GetDouble(); y = (float)innerIter->value.GetArray()[1].GetDouble(); z = (float)innerIter->value.GetArray()[2].GetDouble();
			glm::quat quatEuler = glm::quat(glm::radians(glm::vec3(x, y, z)));
			this->_qOrientation = quatEuler;
		}
	}
}
