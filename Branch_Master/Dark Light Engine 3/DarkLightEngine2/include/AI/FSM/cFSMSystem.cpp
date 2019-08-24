// cFSMSystem.cpp

#include <AI\FSM\cFSMSystem.h>


cFSMSystem::cFSMSystem()
{
	this->mCurrentState = 0;
	this->mbIsRunning = false;

	return;
}

cFSMSystem::~cFSMSystem()
{
	return;
}

void cFSMSystem::AddState(iFSMState* state)
{
	this->mVecStates.push_back(state);
}

void cFSMSystem::Start()
{
	if (this->mVecStates.size() == 0)
	{
		printf("Finite State Machine is empty! Can not start it!\n");
		return;
	}

	this->mbIsRunning = true;
	TransitionToState(this->mVecStates[0]);
}

void cFSMSystem::Update(double deltaTime)
{
	if (!this->mbIsRunning)
	{
		return;
	}

	if (this->mCurrentState == 0)
	{
		return;
	}

	this->mCurrentState->Update(deltaTime);

	if (this->mCurrentState->IsDone())
	{
		//iFSMState* newState = this->mCurrentState->GetTransition();
		//this->TransitionToState(newState);
	}
}

void cFSMSystem::TransitionToState(iFSMState* state)
{
	if (state == 0) 
	{
		printf("Error: [FSMSystem::TransitionToState] The state is null!");
		this->mbIsRunning = false;
		return;
	}

	if (this->mCurrentState != 0) 
	{
		this->mCurrentState->ExitState();
	}

	this->mCurrentState = state;
	this->mCurrentState->EnterState();
}

void cFSMSystem::Reset()
{
	if (this->mCurrentState != 0) 
	{
		this->mCurrentState->ExitState();
	}
	this->mCurrentState = 0;
	this->mbIsRunning = false;
}
