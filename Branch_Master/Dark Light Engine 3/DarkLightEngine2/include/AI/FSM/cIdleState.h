#ifndef _cIdleState_HG_
#define _cIdleState_HG_

#include <AI\FSM\iFSMState.h>


class cIdleState : public iFSMState
{
public:
	cIdleState(float idleTime);
	~cIdleState();

	virtual void AddTransition(int condition, iFSMState* state);
	//virtual void AddTransition(std::string name, void* condition, iFSMState* state);
	virtual iFSMState* GetTransition(std::string name);

	virtual void EnterState();
	virtual void ExitState();

	virtual void Update(double deltaTime);

	virtual bool IsDone();

private:
	void mIdle(double deltaTime);
	float mCurIdleTime;
	float mIdleTime;

};


#endif // !_cIdleState_HG_

