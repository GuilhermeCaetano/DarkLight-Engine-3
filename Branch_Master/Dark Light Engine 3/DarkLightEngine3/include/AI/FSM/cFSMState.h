#ifndef _cFSMState_HG_
#define _cFSMState_HG_

#include <AI\FSM\iFSMState.h>


class cFSMState : public iFSMState
{
public:
	cFSMState();
	~cFSMState();

	virtual void AddTransition(std::string conditionName, void* conditionValue, sCondition::eCompare conditionCompare, iFSMState* nextState, void* conditionGoal = nullptr);
	virtual iFSMState* GetTransition(std::string name);

	virtual void EnterState();
	virtual void ExitState();

	virtual void UpdateValue(std::string conditionName, void* conditionValue);
	virtual void Update(double deltaTime);

	virtual bool IsDone();

private:
	void _FSM();

};

#endif // !_cFSMState_HG_
