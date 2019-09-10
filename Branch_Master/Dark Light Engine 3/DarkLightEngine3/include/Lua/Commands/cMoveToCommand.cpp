// cMoveToCommand.cpp

#include "cMoveToCommand.h"
#include "cSeneUtils.h"
#include "Lerp.h"

cMoveToCommand::cMoveToCommand(unsigned long long uniqueID)
{
	this->_ID = uniqueID;
	this->_type = "MoveTo";

	this->_moveObject = nullptr;
	this->_currentPosition = glm::vec3(0.0f);
	this->_moveToPosition = glm::vec3(0.0f);
	this->_moveTime = 0.0f;
	this->_lifeTime = 0;
	this->bHasStartPosition = false;
	this->_bIsInfinite = false;
	this->_bIsFinished = false;
}

cMoveToCommand::~cMoveToCommand()
{
	return;
}

void cMoveToCommand::Setup(rapidjson::StringBuffer& buffer)
{
	rapidjson::Document document;
	document.Parse(buffer.GetString());

	cSceneUtils* tempUtils = cSceneUtils::GetInstance();
	if (document.FindMember("MoveObject")->value.GetInt() != -1)
	{
		this->_moveObject = tempUtils->FindObjectByIndex(document.FindMember("MoveObject")->value.GetInt());
	}
	else
	{
		this->_currentPosition = tempUtils->GetCurrentCamera()->eyePosition; // camera
	}

	if (document.HasMember("StartPosition"))
	{
		this->bHasStartPosition = true;
		rapidjson::Value::Array v = document.FindMember("StartPosition")->value.GetArray();
		this->_startPosition = glm::vec3(v[0].GetDouble(), v[1].GetDouble(), v[2].GetDouble());
	}
	//else
	//{
	//	if (this->_moveObject != nullptr)
	//	{
	//		this->_startPosition = this->_moveObject->position;
	//		this->_currentPosition = this->_startPosition;
	//	}
	//	else
	//	{
	//		this->_startPosition = this->_currentPosition; // camera
	//	}
	//}

	rapidjson::Value::Array v = document.FindMember("MoveToPosition")->value.GetArray();
	this->_moveToPosition.x = v[0].GetDouble(); this->_moveToPosition.y = v[1].GetDouble(); this->_moveToPosition.z = v[2].GetDouble();
	this->_moveTime = document.FindMember("MoveTime")->value.GetDouble();
	this->_lifeTime = document.FindMember("LifeTime")->value.GetInt();
	this->_bIsInfinite = (this->_lifeTime == -1);

	//this->_totalDistance = abs(glm::distance(this->_startPosition, this->_moveToPosition));
}

void cMoveToCommand::Update(double deltaTime)
{
	if (IsFinished())
	{
		return;
	}

	if (!bHasStartPosition)
	{
		bHasStartPosition = true;
		if (this->_moveObject != nullptr)
		{
			this->_startPosition = this->_moveObject->position;
			this->_currentPosition = this->_startPosition;
			this->_totalDistance = abs(glm::distance(this->_startPosition, this->_moveToPosition));
		}
		else
		{
			this->_startPosition = this->_currentPosition; // camera
		}
	}

	if (abs(glm::distance(this->_moveObject->position, this->_moveToPosition)) > 0.1f)
	{
		// Find the line path to travel
		this->_currentPosition = this->_moveObject->position;
		glm::vec3 direction = this->_moveToPosition - this->_currentPosition;
		glm::vec3 dirVelocity = glm::normalize(direction);
		dirVelocity = SmoothVelocity(dirVelocity);
		//glm::vec3 dirVelocity = SmoothVelocity(direction);
		// Smooth the velocity
		//float distance = abs(glm::distance(this->_moveObject->position, this->_moveToPosition));
		//float velocity = distance / this->_totalDistance;
		//velocity = ((velocity > 1.0f) ? 1.0f : velocity);

		////float smoothValue = distance / this->_moveTime;
		////float smooth2 = LerpTo(0.0f, 1.0f, smoothValue);
		//float smooth = LerpTo(0.0f, this->_totalDistance / this->_moveTime, velocity);

		/*dirVelocity *= smoothValue;
		dirVelocity *= (float)deltaTime;*/
		this->_moveObject->position += dirVelocity * float(deltaTime);
		return;
	}

	//if (abs(glm::distance(this->_moveObject->position, this->_moveToPosition)) > 0.1f)
	//{
	//	// Find the line path to travel
	//	glm::vec3 direction = this->_moveToPosition - this->_moveObject->position;
	//	direction = glm::normalize(direction);
	//	//glm::vec3 dirVelocity = SmoothVelocity(direction);
	//	// Smooth the velocity
	//	float distance = abs(glm::distance(this->_moveObject->position, this->_moveToPosition));
	//	float velocity = distance / this->_totalDistance;
	//	velocity = ((velocity > 1.0f) ? 1.0f : velocity);

	//	//float smoothValue = distance / this->_moveTime;
	//	//float smooth2 = LerpTo(0.0f, 1.0f, smoothValue);
	//	float smooth = LerpTo(0.0f, this->_totalDistance / this->_moveTime, velocity);

	//	/*dirVelocity *= smoothValue;
	//	dirVelocity *= (float)deltaTime;*/
	//	this->_moveObject->position += direction * velocity * smooth * 20.0f * float(deltaTime);
	//	return;
	//}

	this->_moveObject->position = this->_moveToPosition;

	/*if (this->_bIsInfinite)
	{
		return;
	}*/

	if (this->_lifeTime > 0)
	{
		this->_lifeTime--;
		if (this->_lifeTime > 0)
		{
			Reset();
			return;
		}
	}
	_bIsFinished = true;
}

glm::vec3 cMoveToCommand::SmoothVelocity(glm::vec3 dirVelocity)
{
	float currentDistance = glm::distance(this->_moveToPosition, this->_currentPosition);
	//float distance = Clamp(currentDistance, this->_totalDistance, currentDistance);

	// Scale this from 0 to 1 (1 at max distance)
	float slowDownRatio = currentDistance / this->_totalDistance;

	// Lerp using minimum, maximum and the percentage traveled
	slowDownRatio = LerpToPrecise(0.0f, 1.0f, slowDownRatio);

	dirVelocity *= (slowDownRatio * (this->_totalDistance/this->_moveTime) * 4.0f);

	return dirVelocity;
}

//glm::vec3 SmoothVelocity(glm::vec3 direction)
//{
//	float currentDistance = direction.length();
//
//	glm::vec3 normDirection = glm::normalize(direction);
//
//	return normDirection;
//}

void cMoveToCommand::Reset()
{
	if (this->_lifeTime > 0)
	{
		this->_bIsFinished = false;
		this->_currentPosition = this->_startPosition;
		if (this->_moveObject != nullptr) // not camera
		{
			this->_moveObject->position = this->_startPosition;
		}
	}
	else if (this->_bIsInfinite)
	{
		this->_bIsFinished = false;
	}
}

bool cMoveToCommand::IsFinished()
{
	return this->_bIsFinished;
}

glm::vec3 cMoveToCommand::GetCurrentPosition()
{
	return this->_currentPosition;
}