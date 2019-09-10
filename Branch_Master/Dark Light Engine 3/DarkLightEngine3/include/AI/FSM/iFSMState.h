#ifndef _iFSMState_HG_
#define _iFSMState_HG_

#include <string>
#include <map>


class iFSMState;
struct sCondition
{
	enum eCompare
	{
		LOWER,
		LOWER_OR_EQUAL,
		GREATER,
		GREATER_OR_EQUAL,
		EQUAL,
		NOT_EQUAL
	};

	sCondition(std::string conditionName, void* conditionValue, eCompare conditionCompare, iFSMState* nextState, void* conditionGoal)
	{
		this->mConditionName = conditionName;
		this->mConditionValue = conditionValue;
		this->mConditionGoal = conditionGoal;
		this->mConditionCompare = conditionCompare;
		this->mNextState = nextState;
	}

	std::string mConditionName;
	void* mConditionValue;
	void* mConditionGoal;
	eCompare mConditionCompare;
	iFSMState* mNextState;
};

class iFSMState
{
public:
	virtual ~iFSMState() {};

	virtual void AddTransition(std::string conditionName, void* conditionValue, sCondition::eCompare conditionCompare, iFSMState* nextState, void* conditionGoal = nullptr) = 0;
	virtual iFSMState* GetTransition(std::string name) = 0;

	virtual void EnterState() = 0;
	virtual void ExitState() = 0;

	virtual void UpdateValue(std::string conditionName, void* conditionValue) = 0;
	virtual void Update(double deltaTime) = 0;

	virtual bool IsDone() = 0;

protected:
	iFSMState(std::string name) {};
	iFSMState() {};
	int mCurrentCondition;

	std::string mStateName;
	std::map<int, iFSMState*> mMapTransitionsByID;
	std::map<std::string/*name*/, sCondition*/*info*/> mMapTransitionByName;

};




#endif // !_cFSMState_HG_

