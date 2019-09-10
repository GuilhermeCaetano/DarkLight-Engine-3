// cFollowObjectCommand.cpp

#include "cFollowObjectCommand.h"
#include "rapidjson\prettywriter.h"
#include "rapidjson\document.h"
#include <iostream>
#include "cSeneUtils.h"
#include "Lerp.h"

cFollowObjectCommand::cFollowObjectCommand(unsigned long long uniqueID)
{
	this->_ID = uniqueID;
	this->_type = "FollowObject";

	this->_followObject = nullptr;
	this->_objectToFollow = nullptr;
	this->_followDistance = 0.0f;
	this->_farDistance = 0.0f;
	this->_currentPosition = glm::vec3(0.0f, 1.0f, 0.0f);
	this->_offset = glm::vec3(0.0f);
	this->_maxSpeed = 0.0f;
	this->_lifeTime = 0;
	this->_bIsFinished = false;

	return;
}

cFollowObjectCommand::~cFollowObjectCommand()
{
	return;
}

// Read values from json
void cFollowObjectCommand::Setup(rapidjson::StringBuffer& buffer)
{
	rapidjson::Document document;
	document.Parse(buffer.GetString());

	cSceneUtils* tempUtils = cSceneUtils::GetInstance();
	if (document.FindMember("FollowObject")->value.GetInt() != -1)
	{
		this->_followObject = tempUtils->FindObjectByIndex(document.FindMember("FollowObject")->value.GetInt());
		this->_currentPosition = _followObject->position;
	}
	else
	{
		this->_currentPosition = tempUtils->GetCurrentCamera()->eyePosition;
	}
	
	this->_objectToFollow = tempUtils->FindObjectByIndex(document.FindMember("ObjectToFollow")->value.GetInt());
	if (document.HasMember("StartPosition"))
	{
		rapidjson::Value::Array v = document.FindMember("StartPosition")->value.GetArray();
		this->_currentPosition = glm::vec3(v[0].GetDouble(), v[1].GetDouble(), v[2].GetDouble());
	}
	this->_followDistance = document.FindMember("FollowDistance")->value.GetDouble();
	this->_farDistance = document.FindMember("FarDistance")->value.GetDouble();
	rapidjson::Value::Array v = document.FindMember("Offset")->value.GetArray();
	this->_offset = glm::vec3(v[0].GetDouble(), v[1].GetDouble(), v[2].GetDouble());
	this->_maxSpeed = document.FindMember("MaxSpeed")->value.GetDouble();
	this->_lifeTime = document.FindMember("LifeTime")->value.GetInt();
	this->_bIsInfinite = (this->_lifeTime == -1);
}

//glm::vec3 cFollowObjectCommand::GetCurrentPosition()
//{
//	return this->_currentPosition;
//}

void cFollowObjectCommand::Update(double deltaTime)
{
	/*if (IsFinished() && this->_lifeTime <= 0 && !this->_bIsInfinite)
	{
		return;
	}*/

	glm::vec3 finalPosition = _objectToFollow->position + this->_objectToFollow->GetDirection() * this->_offset.z - this->_objectToFollow->GetRight() * this->_offset.x;
	// Find the current distance from the object that is following to the followed
	float currentDistance = glm::distance(finalPosition, this->_currentPosition);

	// Check if the current distance is smaller than the follow distance
	if (currentDistance > this->_followDistance)
	{
		this->_bIsFinished = false;
		// Calculate the direction to the object to follow
		glm::vec3 direction = finalPosition - this->_currentPosition;
		glm::vec3 dirVelocity = glm::normalize(direction);

		// Smoothly lerp when in the slowdonw
		dirVelocity = this->_SmoothVelocity(finalPosition, dirVelocity);

		// How far to move in THIS time step?
		glm::vec3 deltaMove = dirVelocity * (float)deltaTime;

		this->_currentPosition += deltaMove;
		if (this->_followObject != nullptr)
		{
			this->_followObject->position = this->_currentPosition;
		}
		else
		{
			cSceneUtils::GetInstance()->GetCurrentCamera()->eyePosition = this->_currentPosition;
		}
		return;
	}

	// Finished depending on the infinity status
	this->_bIsFinished = true;

	/*if (this->_lifeTime > 0)
	{
	this->_lifeTime--;
	this->_bIsFinished = false;
	}*/
}

glm::vec3 cFollowObjectCommand::_SmoothVelocity(glm::vec3 finalPosition, glm::vec3 dirVelocity)
{
	float currentDistance = glm::distance(finalPosition, this->_currentPosition);
	float distance = Clamp(this->_followDistance, this->_farDistance, currentDistance);

	// Scale this from 0 to 1 (1 at max distance)
	float slowDownRatio = distance / (this->_farDistance - this->_followDistance);

	// Lerp using minimum, maximum and the percentage traveled
	slowDownRatio = LerpTo(0.0f, 1.0f, slowDownRatio);

	dirVelocity *= (slowDownRatio * this->_maxSpeed);

	return dirVelocity;
}

void cFollowObjectCommand::Reset()
{
	this->_bIsFinished = false;
}

bool cFollowObjectCommand::IsFinished()
{
	glm::vec3 finalPosition = _objectToFollow->position + this->_objectToFollow->GetDirection() * this->_offset.z - this->_objectToFollow->GetRight() * this->_offset.x;
	float currentDistance = glm::distance(finalPosition, this->_currentPosition);
	if (currentDistance > this->_followDistance)
	{
		return false;
	}
	return true;
}

glm::vec3 cFollowObjectCommand::GetCurrentPosition()
{
	return this->_currentPosition;
}