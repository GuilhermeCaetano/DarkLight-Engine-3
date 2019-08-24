#ifndef _cFSMSystem_HG_
#define _cFSMSystem_HG_

#include <AI\FSM\iFSMState.h>
#include <vector>


class cFSMSystem
{
public:
	cFSMSystem();
	~cFSMSystem();

	void AddState(iFSMState* state);
	void TransitionToState(iFSMState* state);

	void Start();
	void Reset();

	void Update(double deltaTime);

private:
	iFSMState* mCurrentState;
	bool mbIsRunning;

	std::vector<iFSMState*> mVecStates;
};


#endif // !_cFSM_HG_

