// cFSMState

#include <AI\FSM\cFSMState.h>

cFSMState::cFSMState()
{
	return;
}

cFSMState::~cFSMState()
{
	std::map<std::string, sCondition*>::iterator mapIter;
	for (mapIter; mapIter != this->mMapTransitionByName.end(); mapIter++)
	{
		delete mapIter->second;
		mapIter->second = nullptr;
	}
	this->mMapTransitionByName.clear();

	return;
}

void cFSMState::AddTransition(std::string conditionName, void* conditionValue, sCondition::eCompare conditionCompare, iFSMState* nextState, void* conditionGoal)
{
	sCondition* newCondition = new sCondition(conditionName, conditionValue, conditionCompare, nextState, conditionGoal);
	this->mMapTransitionByName[conditionName] = newCondition;
}

iFSMState* cFSMState::GetTransition(std::string name)
{
	std::map<std::string, sCondition*>::iterator mapIter = this->mMapTransitionByName.find(name);
	if (mapIter != this->mMapTransitionByName.end())
	{
		return mapIter->second->mNextState;
	}

	return nullptr;
}

void cFSMState::EnterState()
{
	printf("Entered Idle State\n");
	
}

void cFSMState::ExitState()
{
	printf("Exited Idle State\n");
	
}

void cFSMState::UpdateValue(std::string conditionName, void* conditionValue)
{
	sCondition* tempCondition;
	std::map<std::string, sCondition*>::iterator mapiter = this->mMapTransitionByName.find(conditionName);
	if (mapiter != this->mMapTransitionByName.end())
	{
		tempCondition = mapiter->second;
		tempCondition->mConditionValue = conditionValue;
	}
}

void cFSMState::Update(double deltaTime)
{
	
}

bool cFSMState::IsDone()
{
	sCondition* tempCondition;
	std::map<std::string, sCondition*>::iterator mapiter = this->mMapTransitionByName.begin();
	for (mapiter; mapiter != this->mMapTransitionByName.end(); mapiter++)
	{
		tempCondition = mapiter->second;

		switch (tempCondition->mConditionCompare)
		{
		case sCondition::LOWER:
			if (tempCondition->mConditionValue < tempCondition->mConditionGoal)
			{
				return true;
			}
			break;
		case sCondition::LOWER_OR_EQUAL:
			if (tempCondition->mConditionValue <= tempCondition->mConditionGoal)
			{
				return true;
			}
			break;
		case sCondition::GREATER:
			if (tempCondition->mConditionValue > tempCondition->mConditionGoal)
			{
				return true;
			}
			break;
		case sCondition::GREATER_OR_EQUAL:
			if (tempCondition->mConditionValue >= tempCondition->mConditionGoal)
			{
				return true;
			}
			break;
		case sCondition::EQUAL:
			if (tempCondition->mConditionValue == tempCondition->mConditionGoal)
			{
				return true;
			}
			break;
		case sCondition::NOT_EQUAL:
			if (tempCondition->mConditionValue != tempCondition->mConditionGoal)
			{
				return true;
			}
			break;
		default:
			if (tempCondition->mConditionValue < tempCondition->mConditionGoal)
			{
				return true;
			}
			break;
		}
	}

	return false;
}

void cFSMState::_FSM()
{

}
