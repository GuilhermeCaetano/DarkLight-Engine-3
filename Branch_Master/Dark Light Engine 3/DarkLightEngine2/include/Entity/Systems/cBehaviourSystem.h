#ifndef _cBehaviourSystem_HG_
#define _cBehaviourSystem_HG_

#include "iSystem.h"
#include <Entity\sEntity.h>
#include <OpenGL\OpenGLHeaders.h>
#include <Interfaces\Physics\iRigidbody.h>
#include <vector>

/*  BEHAVIOUR SYSTEM
- Adds an agent entity when a behaviour component is added to the entity.
- Adds location component and rigidbody component of an agent added as well
- 

*/

class cBehaviourSystem : public iSystem
{
public:
	cBehaviourSystem();
	~cBehaviourSystem();

	// Dissecting the behaviour...
	//float GetDotOfDirections(); // Use dot product to determine the directions relation
	//float GetAngleBetweenAgentTarget(); // Use acos to get the angle between the agent and the target
	//float GetDistanceBetweenAgentTarget(); // Check the current distance between the agent and the target
	void SetAgentMovement(); // Sets the agent rigidbody stats of movementation
	void SetAgentRotation(); // Sets the agent rigidbody stats of rotation

	// Updates the system
	virtual void UpdateSystem(double deltaTime);

	// Schedules and unschedule specific components
	// in order to save searching memory
	virtual bool ScheduleEntity(sEntity* entity, iComponent* component);
	virtual bool UnscheduleEntity(sEntity* entity, iComponent* component);

private:
	struct sAgent
	{
		sEntity* agentEntity = nullptr;
		//HACK: later it would be nice to implement a brain component sBehaviourTypes currentBehaviour = sBehaviourTypes::;
		struct sLocationComponent* agentLocatC = nullptr;
		nPhysics::iRigidbody* agentRigidbC = nullptr;

		struct sSeekBehaviourComponent* seekBehavC = nullptr;
		struct sFleeBehaviourComponent* fleeBehavC = nullptr;
		struct sPursueBehaviourComponent* pursueBehavC = nullptr;
		struct sEvadeBehaviourComponent* evadeBehavC = nullptr;
		struct sApproachBehaviourComponent* approachBehavC = nullptr;
		struct sWanderBehaviourComponent* wanderBehavC = nullptr;
		struct sIdleBehaviourComponent* idleBehavC = nullptr;

		float currentIdleTime = 0.0f;
		sEntity* closestThreat = nullptr;
	};


	std::vector<sAgent*> _vecAgents;

	sAgent* _AgentAlreadyExists(sEntity* newAgent);
	void _SetBehaviour(sAgent* agent, iComponent* newBehavComponent);

	void _BehaviourFSM(sAgent* agent, double deltaTime);

	
	int _UpdateSeekBehaviour(sAgent* agent); // Seeks a target. Returns 1 on ongoing behaviour and 0 on hibernation
	int _UpdateFleeBehaviour(sAgent* agent); // Flees a target. Returns 1 on ongoing behaviour and 0 on hibernation
	int _UpdatePursueBehaviour(sAgent* agent);
	int _UpdateEvadeBehaviour(sAgent* agent);
	int _UpdateApproachBehaviour(sAgent* agent, double deltaTime);
	int _UpdateWanderBehaviour(sAgent* agent);
	int _UpdateIdleBehaviour(sAgent* agent, double deltaTime);

	glm::vec3 _GetMoveDirection(); // Get the direction to move and rotate to

	sEntity* GetClosestThreatByDistance(glm::vec3 pointToCompare, std::string threatTag);

	//std::vector<sEntity*> _vecAgents;
	//std::vector<iComponent*> _vecAgentLocationComponents;
	//std::vector<iComponent*> _vecAgentRigidbodyComponents;
};




#endif // !_cBehaviourSystem_HG_
