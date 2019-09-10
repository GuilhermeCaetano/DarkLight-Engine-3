// cIdleState.cpp

#include <AI\FSM\cIdleState.h>

cIdleState::cIdleState(float idleTime) : iFSMState("Idle")
{
	this->mIdleTime = idleTime;
	this->mCurIdleTime = 0.0f;

	return;
}

cIdleState::~cIdleState()
{
	/*std::map<std::string, sCondition*>::iterator mapIter;
	for (mapIter; mapIter != this->mMapTransitionByName.end(); mapIter++)
	{
		delete mapIter->second;
		mapIter->second = nullptr;
	}
	this->mMapTransitionByName.clear();*/

	return;
}

void cIdleState::AddTransition(int condition, iFSMState* state)
{
	/*sCondition* newCondition = new sCondition(name, condition, state);
	this->mMapTransitionByName[name] = newCondition;*/
}

iFSMState* cIdleState::GetTransition(std::string name)
{
	/*std::map<std::string, sCondition*>::iterator mapIter = this->mMapTransitionByName.find(name);
	if (mapIter != this->mMapTransitionByName.end())
	{
		return mapIter->second->nextState;
	}*/

	return nullptr;
}

void cIdleState::EnterState()
{
	printf("Entered Idle State\n");
	this->mCurIdleTime = 0.0f;
}

void cIdleState::ExitState()
{
	printf("Exited Idle State\n");
	this->mCurIdleTime = 0.0f;
}

void cIdleState::Update(double deltaTime)
{
	this->mCurrentCondition = 1;

	this->mIdle(deltaTime);
}

bool cIdleState::IsDone()
{
	if (this->mCurIdleTime >= this->mIdleTime)
	{
		this->mCurIdleTime = 0.0f;
		return true;
	}

	return false;
}

void cIdleState::mIdle(double deltaTime)
{
	this->mIdleTime += 1.0f * (float)deltaTime;
}
