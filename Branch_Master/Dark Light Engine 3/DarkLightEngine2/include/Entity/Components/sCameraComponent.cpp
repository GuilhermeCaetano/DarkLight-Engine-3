// sCameraComponent.cpp

#include <Entity\Components\sCameraComponent.h>
#include <Entity\sEntity.h>
#include <Math\nQuaternionHelper.h>


sCameraComponent::sCameraComponent(unsigned long long entityID)
{
	this->_parentID = entityID;
	this->Start();

	return;
}

sCameraComponent::~sCameraComponent()
{
	return;
}

void sCameraComponent::Start()
{
	this->eyePosition = glm::vec3(0.0f, 0.0f, -10.0f);
	this->forward = glm::vec3(0.0f, 0.0f, 1.0f);
	this->up = glm::vec3(0.0f, 1.0f, 0.0f);
	this->right = glm::vec3(1.0f, 0.0f, 0.0f);
	this->qOrientation = glm::quat(glm::vec3(0.0f));
	this->_forwardDirection = glm::vec3(0.0f, 0.0f, 1.0f);
	this->_upDirection = glm::vec3(0.0f, 1.0f, 0.0f);
	this->_rightDirection = glm::vec3(1.0f, 0.0f, 0.0f);
	this->movementSpeed = 1.0f;

	this->target = nullptr;
	this->targetOffset = glm::vec3(0.0f);
	this->lookAtOffset = glm::vec3(0.0f);

	this->bCanControl = true;
	this->bFollowTarget = false;
}

glm::vec3 sCameraComponent::GetLookAtInWorldSpace()
{
	// The "At" is relative to the where the camera is...
	// So just add the "At" to the current camera location
	if (this->target != nullptr && this->bFollowTarget)
	{
		glm::vec3 outTarget = this->target->transform->GetWorldPosition() + this->target->transform->GetRightVector() * this->lookAtOffset.x
			+ this->target->transform->GetUpVector() * this->lookAtOffset.y + this->target->transform->GetForwardVector() * this->lookAtOffset.z;
		return outTarget;
	}

	return this->eyePosition + this->_lookAt;
}

glm::vec3 sCameraComponent::GetCameraDirection()
{
	// Update direction looking at by current rotation
	this->UpdateAtFromOrientation();

	return this->_lookAt;
}

glm::vec3 sCameraComponent::GetUpVector()
{
	if (this->target != nullptr && this->bFollowTarget)
	{
		return this->up;
		return glm::vec3(0.0f, 1.0f, 0.0f);
		//return this->target->transform->up;
	}

	return this->up;
}

void sCameraComponent::UpdateAtFromOrientation()
{
	// Generate matrix rotation from quaternion
	glm::mat4 matRotation = glm::mat4(this->qOrientation);

	// Transform to vec4 to match
	glm::vec4 frontOfCamera = glm::vec4(this->_forwardDirection, 1.0f);

	// Multiply to add to the matrix
	glm::vec4 newAt = matRotation * frontOfCamera;

	// Update the look at
	this->_lookAt = glm::vec3(newAt);
}

void sCameraComponent::UpdateForwardFromOrientation()
{
	// Generate matrix rotation from quaternion
	glm::mat4 matRotation = glm::mat4(this->qOrientation);

	// Transform to vec4 to match
	glm::vec4 forwardVector = glm::vec4(this->_forwardDirection, 1.0f);

	// Multiply to add to the matrix
	glm::vec4 newForward = matRotation * forwardVector;

	// Update the current up
	this->forward = glm::vec3(newForward);
}

void sCameraComponent::UpdateUpFromOrientation()
{
	// Generate matrix rotation from quaternion
	glm::mat4 matRotation = glm::mat4(this->qOrientation);

	// Transform to vec4 to match
	glm::vec4 upVector = glm::vec4(this->_upDirection, 1.0f);

	// Multiply to add to the matrix
	glm::vec4 newUp = matRotation * upVector;

	// Update the current up
	this->up = glm::vec3(newUp);
}

void sCameraComponent::UpdateRightFromOrientation()
{
	// Generate matrix rotation from quaternion
	glm::mat4 matRotation = glm::mat4(this->qOrientation);

	// Transform to vec4 to match
	glm::vec4 rightVector = glm::vec4(this->_rightDirection, 1.0f);

	// Multiply to add to the matrix
	glm::vec4 newRight = matRotation * rightVector;

	// Update the current up
	this->right = glm::vec3(newRight);
}

void sCameraComponent::SetTarget(sEntity* newTarget, glm::vec3 offset, float followDistance, float maxSpeed)
{
	this->target = newTarget;
	this->targetOffset = offset;
	this->fullSpeedDistance = followDistance;
	this->maxFollowSpeed = maxSpeed;
	this->bFollowTarget = true;
}

void sCameraComponent::SetLookAtOffset(const glm::vec3& offset)
{
	this->lookAtOffset = offset;
}

sCameraComponent::eFollowTypes sCameraComponent::GetCameraFollowType()
{
	return this->_followType;
}

void sCameraComponent::SetCameraFollowType(eFollowTypes followType)
{
	this->_followType = followType;
}

void sCameraComponent::SetTarget(sEntity* newTarget)
{
	this->target = newTarget;
}

void sCameraComponent::_SerializeToJson(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& prettyWriter)
{
	prettyWriter.Key("EyePosition"); prettyWriter.StartArray();
	prettyWriter.Double((double)this->eyePosition.x); prettyWriter.Double((double)this->eyePosition.y); prettyWriter.Double((double)this->eyePosition.z);
	prettyWriter.EndArray();
	prettyWriter.Key("Rotation"); prettyWriter.StartArray();
	glm::vec3 eulerQuat = glm::eulerAngles(this->qOrientation);
	prettyWriter.Double((double)eulerQuat.x); prettyWriter.Double((double)eulerQuat.y); prettyWriter.Double((double)eulerQuat.z);
	prettyWriter.EndArray();
	prettyWriter.Key("MovementSpeed"); prettyWriter.Double((double)this->movementSpeed);
	prettyWriter.Key("CanControl"); prettyWriter.Bool(this->bCanControl);
}

void sCameraComponent::_DeserializeFromJson(rapidjson::Value::ConstValueIterator vIter)
{
	for (rapidjson::Value::ConstMemberIterator innerIter = vIter->MemberBegin(); innerIter != vIter->MemberEnd(); innerIter++)
	{
		if (innerIter->name == "EyePosition") { this->eyePosition.x = (float)innerIter->value.GetArray()[0].GetDouble(); this->eyePosition.y = (float)innerIter->value.GetArray()[1].GetDouble(); this->eyePosition.z = (float)innerIter->value.GetArray()[2].GetDouble(); }
		else if (innerIter->name == "Rotation")
		{
			float x, y, z;
			x = (float)innerIter->value.GetArray()[0].GetDouble(); y = (float)innerIter->value.GetArray()[1].GetDouble(); z = (float)innerIter->value.GetArray()[2].GetDouble();
			glm::quat quatEuler = glm::quat(glm::vec3(x, y, z));
			this->qOrientation = quatEuler;
			this->UpdateAtFromOrientation();
			this->UpdateUpFromOrientation();
		}
		else if (innerIter->name == "MovementSpeed") { this->movementSpeed = (float)innerIter->value.GetDouble(); }
		else if (innerIter->name == "CanControl") { this->bCanControl = innerIter->value.GetBool(); }
	}
}
