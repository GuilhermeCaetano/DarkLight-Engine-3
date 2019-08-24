// cOrientToCommand.cpp

#include "cOrientToCommand.h"
#include "cSeneUtils.h"
#include "Slerp.h"
#include "cQuaternions.h"

cOrientToCommand::cOrientToCommand(unsigned long long uniqueID)
{
	this->_ID = uniqueID;
	std::string _type = "OrientTo";

	cMeshObject* _rotateObject = nullptr;
	cMeshObject* _objectToRotateTo = nullptr;

	float _rotationSpeed = 0.0f;

	this->_bIsInfinite = false;
	this->_bIsFinished = false;
	
	return;
}

cOrientToCommand::~cOrientToCommand()
{
	return;
}

void cOrientToCommand::Setup(rapidjson::StringBuffer& buffer)
{
	rapidjson::Document document;
	document.Parse(buffer.GetString());

	cSceneUtils* tempUtils = cSceneUtils::GetInstance();
	if (document.FindMember("OrientObject")->value.GetInt() != -1)
	{
		this->_rotateObject = tempUtils->FindObjectByIndex(document.FindMember("OrientObject")->value.GetInt());
	}

	this->_objectToRotateTo = tempUtils->FindObjectByIndex(document.FindMember("OrientToPosition")->value.GetInt());
	this->_rotationSpeed = document.FindMember("OrientSpeed")->value.GetDouble();
	this->_lifeTime = document.FindMember("LifeTime")->value.GetInt();
	this->_bIsInfinite = (this->_lifeTime == -1);

	//this->_currentRotation = _rotateObject->GetMeshQuatOrientation();
}

void cOrientToCommand::Update(double deltaTime)
{
	if (IsFinished())
	{
		return;
	}

	// Get the current rotation
	this->_currentRotation = _rotateObject->GetMeshQuatOrientation();

	// Get the direction vector from the object we want to rotate
	// and the vector towards what we want to look at
	glm::vec3 objectDirection = _rotateObject->GetDirection(); // This is the object's forward vector
	objectDirection = glm::normalize(objectDirection);
	objectDirection.y = 0.0f;
	
	glm::vec3 targetDirection = _objectToRotateTo->position - _rotateObject->position;
	targetDirection = glm::normalize(targetDirection);
	targetDirection.y = 0.0f;
	
	// Create the quaternion rotation from the two vectors
	//this->_targetRotation = RotateTowards(objectDirection, targetDirection);
	this->_targetRotation = glm::quatLookAt(-targetDirection, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::quat ftargetRotation = glm::inverse(_currentRotation) * _targetRotation;

	// Find the angle between the vectors and check for the slerp
	float angle = glm::acos(glm::min(1.0f, glm::dot(objectDirection, targetDirection)));
	angle = glm::degrees(angle);
	if (glm::abs(angle) >= 1.0f)
	{
		float timeElapsed = 1.0f / angle;
		//glm::quat dist = glm::distance(_currentRotation, _targetRotation);
		// Call Slerp
		//glm::quat finalRotation = glm::mix(_currentRotation, _targetRotation, timeElapsed);
		glm::quat finalRotation = glm::slerp(_currentRotation, _targetRotation, 0.1f);
		//glm::quat finalRotation = SlerpTo(_currentRotation, _targetRotation, timeElapsed * 0.001f);
		//finalRotation *= _currentRotation;
		//this->_rotateObject->adjMeshOrientationQ(_rotateObject->GetMeshQuatOrientation(), finalRotation);
		
		//this->_currentRotation = finalRotation;

		 //_rotateObject->GetMeshQuatOrientation() = finalRotation;
		//this->_rotateObject->setMeshOrientationQ(finalRotation);
		//this->_rotateObject->GetMeshQuatOrientation() = _targetRotation;
		//this->_rotateObject->adjMeshOrientationQ(_rotateObject->GetMeshQuatOrientation(), finalRotation);
		this->_rotateObject->setMeshOrientationQ(finalRotation);
		this->_currentRotation = _rotateObject->GetMeshQuatOrientation();
		return;
	}
	
	if (this->_lifeTime == 0)
	{
		return;
	}

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

void cOrientToCommand::Reset()
{
	if (this->_lifeTime > 0 || this->_bIsInfinite)
	{
		this->_currentRotation = _rotateObject->GetMeshQuatOrientation();
		_bIsFinished = false;
	}
}

bool cOrientToCommand::IsFinished()
{
	return this->_bIsFinished;
}

glm::vec3 cOrientToCommand::GetCurrentPosition()
{
	return glm::vec3(0.0f);
}