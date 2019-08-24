// cWaitCommand.cpp

#include "cWaitCommand.h"
#include "rapidjson\document.h"

cWaitCommand::cWaitCommand(unsigned long long uniqueID)
{
	this->_ID = uniqueID;
	this->_type = "Wait";

	this->_waitForTime = 0.0f;
	this->_timeWaited = 0.0f;
	this->_bIsFinished = false;

	return;
}

cWaitCommand::~cWaitCommand()
{
	return;
}

void cWaitCommand::Setup(rapidjson::StringBuffer& buffer)
{
	rapidjson::Document document;
	document.Parse(buffer.GetString());

	this->_waitForTime = document.FindMember("WaitForTime")->value.GetDouble();
}

void cWaitCommand::Update(double deltaTime)
{
	if (IsFinished())
	{
		return;
	}

	this->_timeWaited += (float)deltaTime;
}

void cWaitCommand::Reset()
{
	this->_timeWaited = 0.0f;
}

bool cWaitCommand::IsFinished()
{
	if (this->_timeWaited >= this->_waitForTime)
	{
		this->_bIsFinished = true;
	}
	else
	{
		this->_bIsFinished = false;
	}

	return this->_bIsFinished;
}

glm::vec3 cWaitCommand::GetCurrentPosition()
{
	return glm::vec3(0.0f);
}