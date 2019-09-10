#ifndef _cGatherState_HG_
#define _cGatherState_HG_

#include <AI\FSM\iFSMState.h>


class cGatherState : public iFSMState
{
public:
	cGatherState();
	~cGatherState();

	virtual void AddTransition(int condition, iFSMState* state);
	virtual iFSMState* GetTransition();

	virtual void EnterState();
	virtual void ExitState();

	virtual void Update();

	virtual bool IsDone();

private:
	void _Gather();

};


#endif // !_cGatherState_HG_
