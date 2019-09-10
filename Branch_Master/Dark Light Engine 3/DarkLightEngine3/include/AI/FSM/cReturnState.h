#ifndef _cReturnState_HG_
#define _cReturnState_HG_

#include <AI\FSM\iFSMState.h>


class cReturnState : public iFSMState
{
public:
	cReturnState();
	~cReturnState();

	virtual void AddTransition(int condition, iFSMState* state);
	virtual iFSMState* GetTransition();

	virtual void EnterState();
	virtual void ExitState();

	virtual void Update();

	virtual bool IsDone();

private:
	void _Return();

};


#endif // !_cReturnState_HG_
