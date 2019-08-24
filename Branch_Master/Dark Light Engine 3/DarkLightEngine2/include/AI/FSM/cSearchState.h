#ifndef _cSearchState_HG_
#define _cSearchState_HG_

#include <AI\FSM\iFSMState.h>


class cSearchState : public iFSMState
{
public:
	cSearchState();
	~cSearchState();

	virtual void AddTransition(int condition, iFSMState* state);
	virtual iFSMState* GetTransition();

	virtual void EnterState();
	virtual void ExitState();

	virtual void Update();

	virtual bool IsDone();

private:
	void _Search();

};


#endif // !_cSearchState_HG_
