// cBehaviourSystem.cpp

#include <Entity\Systems\cBehaviourSystem.h>
#include <Entity\Components\Behaviours\sSeekBehaviourComponent.h>
#include <Entity\Components\Behaviours\sFleeBehaviourComponent.h>
#include <Entity\Components\Behaviours\sPursueBehaviourComponent.h>
#include <Entity\Components\Behaviours\sEvadeBehaviourComponent.h>
#include <Entity\Components\Behaviours\sApproachBehaviourComponent.h>
#include <Entity\Components\Behaviours\sWanderBehaviourComponent.h>
#include <Entity\Components\Behaviours\sIdleBehaviourComponent.h>

#include <Entity\Components\sLocationComponent.h>
#include <SimplePhysics\Physics\sSimpleRigidbodyComponent.h>
#include <Pools\cPoolManager.h>
#include <Math\nQuaternionHelper.h>

#include <Math\nMathHelper.h>

cBehaviourSystem::cBehaviourSystem()
{
	return;
}

cBehaviourSystem::~cBehaviourSystem()
{
	return;
}

cBehaviourSystem::sAgent* cBehaviourSystem::_AgentAlreadyExists(sEntity* newAgent)
{
	// Check if the agent has already been set
	for (unsigned int i = 0; i != this->_vecAgents.size(); i++)
	{
		if (this->_vecAgents[i]->agentEntity == newAgent)
		{
			// Found it
			return this->_vecAgents[i];
		}
	}

	// Didn't find any, it's a new agent
	return nullptr;
}

bool cBehaviourSystem::ScheduleEntity(sEntity* entity, iComponent* component)
{
	// Check if agent is already there
	sAgent* newAgent = this->_AgentAlreadyExists(entity);

	if (newAgent == nullptr)
	{
		// Create a new agent
		newAgent = new sAgent();

		// Get location and rigidbody components
		sLocationComponent* locationC = entity->GetComponent<sLocationComponent>();
		//nPhysics::iRigidbody* rigidbodyC = entity->GetComponent<>();

		// Save components to avoid overhead
		newAgent->agentEntity = entity;
		newAgent->agentLocatC = locationC;
		//newAgent->agentRigidbC = rigidbodyC;
		this->_vecAgents.push_back(newAgent);
	}
	
	// Add new behaviour. If the behaviour is already there, overwrite it
	this->_SetBehaviour(newAgent, component);

	return true;
}

bool cBehaviourSystem::UnscheduleEntity(sEntity* entity, iComponent* component)
{
	return true;
}

void cBehaviourSystem::_SetBehaviour(sAgent* agent, iComponent* newBehavComponent)
{
	float randomDistance = 0.0f;
	glm::vec3 newPoint = glm::vec3(0.0f);
	/*switch (newBehavComponent->GeteComponentType())
	{
	case eComponentTypes::SEEKBEHAVIOUR_COMPONENT:
		agent->seekBehavC = static_cast<sSeekBehaviourComponent*>(newBehavComponent);
		break;
	case eComponentTypes::FLEEBEHAVIOUR_COMPONENT:
		agent->fleeBehavC = static_cast<sFleeBehaviourComponent*>(newBehavComponent);
		break;
	case eComponentTypes::PURSUEBEHAVIOUR_COMPONENT:
		agent->pursueBehavC = static_cast<sPursueBehaviourComponent*>(newBehavComponent);
		break;
	case eComponentTypes::EVADEBEHAVIOUR_COMPONENT:
		agent->evadeBehavC = static_cast<sEvadeBehaviourComponent*>(newBehavComponent);
		break;
	case eComponentTypes::APPROACHBEHAVIOUR_COMPONENT:
		agent->approachBehavC = static_cast<sApproachBehaviourComponent*>(newBehavComponent);
		break;
	case eComponentTypes::WANDERBEHAVIOUR_COMPONENT:
		agent->wanderBehavC = static_cast<sWanderBehaviourComponent*>(newBehavComponent);
		randomDistance = nMathHelper::GetRandInRange<float>(1.0f, agent->wanderBehavC->radius);
		newPoint = agent->wanderBehavC->center + glm::vec3(randomDistance, 0.0f, randomDistance);
		agent->wanderBehavC->wanderPoint = newPoint;
		break;
	case eComponentTypes::IDLEBEHAVIOUR_COMPONENT:
		agent->idleBehavC = static_cast<sIdleBehaviourComponent*>(newBehavComponent);
		break;
	default:
		break;
	}*/
}

void cBehaviourSystem::UpdateSystem(double deltaTime)
{
	for (unsigned int i = 0; i != this->_vecAgents.size(); i++)
	{
		sAgent* agent = this->_vecAgents[i];
		if (!agent->agentEntity->GetIsActive())
		{
			return;
		}
		this->_BehaviourFSM(agent, deltaTime);
	}
}

void cBehaviourSystem::_BehaviourFSM(sAgent* agent, double deltaTime)
{
	int result = 0;

	// First check for follow/flee types of commands.
	// Priorities are approach > pursue/evade > seek/flee
	if (agent->approachBehavC != nullptr)
	{
		result = this->_UpdateApproachBehaviour(agent, deltaTime);
		return;
	}

	// Check Evade/Flee behaviours
	if (agent->approachBehavC == nullptr && agent->evadeBehavC != nullptr)
	{
		result = this->_UpdateEvadeBehaviour(agent);
	}
	else if (agent->approachBehavC == nullptr && agent->fleeBehavC != nullptr)
	{
		result = this->_UpdateFleeBehaviour(agent);
	}

	if (result != 0)
	{
		return;
	}
	// Check Pursue/Seek behaviours
	if (agent->approachBehavC == nullptr && agent->pursueBehavC != nullptr)
	{
		result = this->_UpdatePursueBehaviour(agent);
	}
	else if (agent->approachBehavC == nullptr && agent->seekBehavC != nullptr)
	{
		result = this->_UpdateSeekBehaviour(agent);
	}

	// Only check this if other behaviours aren't doing anything
	if (result == 0)
	{
		if (agent->wanderBehavC != nullptr)
		{
			if (agent->idleBehavC == nullptr || !agent->idleBehavC->idle)
			{
				result = this->_UpdateWanderBehaviour(agent);
			}
		}
		// Check Wander/Idle behaviours
		if (agent->idleBehavC != nullptr)
		{
			result = this->_UpdateIdleBehaviour(agent, deltaTime);
		}
	}
}

int cBehaviourSystem::_UpdateSeekBehaviour(sAgent* agent)
{
	if (agent->seekBehavC->target == nullptr)
	{
		return 0;
	}

	// Dissecting the method...

	// Getting variables
	sSeekBehaviourComponent* seekC = static_cast<sSeekBehaviourComponent*>(agent->seekBehavC);
	sLocationComponent* agentLocationC = static_cast<sLocationComponent*>(agent->agentLocatC);
	nPhysics::sSimpleRigidbodyComponent* agentRigidbC = static_cast<nPhysics::sSimpleRigidbodyComponent*>(agent->agentRigidbC);
	sLocationComponent* targetLocationC = static_cast<sLocationComponent*>(seekC->target);

	// Get the agent and the target's normalized directions
	glm::vec3 agentDir = agentLocationC->GetForwardVector();
	glm::vec3 targetDir = targetLocationC->GetForwardVector();

	// Find the distance between the agent and the target
	glm::vec3 lookVec = targetLocationC->GetWorldPosition() - agentLocationC->GetWorldPosition();
	float sqrDistance = nMathHelper::SquareMagnitude(lookVec);

	// Get the look direction
	glm::vec3 lookDir = glm::normalize(lookVec);

	// Get the right direction, so we can check the angle
	glm::vec3 rightDir = agentLocationC->GetRightVector();

	// Dot the directions to find their relations
	float dotDirection = glm::dot(agentDir, lookDir);

	// Dot the right direction so we can find which side the angle is on
	float dotRight = glm::dot(rightDir, lookDir);

	// Find the real angle between the agent and the target
	float finalAngle = glm::acos(dotDirection);

	int result = 0;

	if (seekC->behaviourType == eBehaviourTypes::SIMPLE)
	{
		// Move the agent
		if (sqrDistance > 10.0f)
		{
			agentRigidbC->AddForce(lookDir * 3.0f, nPhysics::ForceMode::Force);
			result = 1;
		}
		else
		{
			agentRigidbC->SetVelocity(glm::vec3(0.0f));
			result = 0;
		}
	}
	else if (seekC->behaviourType == eBehaviourTypes::BOO)
	{
		// Dot the look direction and the target direction to find out
		// if the target is looking in the agent's direction.
		float dotLookView = glm::dot(lookDir, targetDir);

		// Target will be looking at the agent at a value -1 <= v <= -0.7
		// giving it a "range" to the view frustrum (like a cone)
		if (dotLookView <= -0.7f && dotLookView >= -1.0f)
		{
			// Target is looking this way, get out
			result = 0;
		}
		else
		{
			// Target is not looking this way enough, get it
			if (sqrDistance > 50.0f)
			{
				agentRigidbC->AddRelativeForce(glm::vec3(0.0f, 0.0f, 3.0f), nPhysics::ForceMode::Force);
				result = 1;
			}
			else
			{
				agentRigidbC->SetVelocity(glm::vec3(0.0f));
				result = 0;
			}
		}
	}
	

	// Rotate the agent
	float multiplier = ((dotRight >= 0.0f) ? 1.0f : -1.0f);
	printf("FinalAngle: %.1f\n", finalAngle);
	if (finalAngle > 0.1f)
	{
		glm::quat orientation = nQuaternionHelper::AdjOrientationEulerAngles(agentLocationC->GetWorldRotation(), glm::vec3(0.0f, 2.0f * multiplier, 0.0f), true);
		agentLocationC->SetWorldRotation(orientation);
		//agentRigidbC->angularAcceleration.y = 1.0f * multiplier;
	}
	else
	{
		//agentRigidbC->angularAcceleration.y = 0.0f;
	}

	return result;
}

int cBehaviourSystem::_UpdateFleeBehaviour(sAgent* agent)
{
	if (agent->fleeBehavC->target == nullptr)
	{
		return 0;
	}

	// Dissecting the method...

	// Getting variables
	sFleeBehaviourComponent* fleeC = static_cast<sFleeBehaviourComponent*>(agent->fleeBehavC);
	sLocationComponent* agentLocationC = static_cast<sLocationComponent*>(agent->agentLocatC);
	nPhysics::sSimpleRigidbodyComponent* agentRigidbC = static_cast<nPhysics::sSimpleRigidbodyComponent*>(agent->agentRigidbC);
	sLocationComponent* targetLocationC = static_cast<sLocationComponent*>(fleeC->target);

	// Get the agent and the target's normalized directions
	glm::vec3 agentDir = agentLocationC->GetForwardVector();
	glm::vec3 targetDir = targetLocationC->GetForwardVector();

	// Find the distance between the agent and the target
	glm::vec3 lookVec = targetLocationC->GetWorldPosition() - agentLocationC->GetWorldPosition();
	float sqrDistance = nMathHelper::SquareMagnitude(lookVec);

	// Get the look direction
	glm::vec3 lookDir = glm::normalize(lookVec);

	// Get the right direction, so we can check the angle
	glm::vec3 rightDir = agentLocationC->GetRightVector();

	// Dot the directions to find their relations
	float dotDirection = glm::dot(agentDir, lookDir);

	// Dot the right direction so we can find which side the angle is on
	float dotRight = glm::dot(rightDir, lookDir);

	// Find the real angle between the agent and the target
	float finalAngle = glm::acos(dotDirection);

	int result = 0;

	if (fleeC->behaviourType == eBehaviourTypes::SIMPLE)
	{
		// Move the agent
		if (sqrDistance < 50.0f)
		{
			agentRigidbC->AddRelativeForce(glm::vec3(0.0f, 0.0f, -3.0f), nPhysics::ForceMode::Force);
			result = 1;
			//agentRigidbC->AddRelativeForce(glm::vec3(0.0f, 0.0f, 1.0f), nPhysics::ForceMode::Force);
			//agentRigidbC->acceleration.z = 1.0f;
		}
		else
		{
			agentRigidbC->SetVelocity(glm::vec3(0.0f));
			result = 0;
			//agentRigidbC->acceleration.z = 0.0f;
		}
	}
	else if(fleeC->behaviourType == eBehaviourTypes::BOO)
	{
		// Dot the look direction and the target direction to find out
		// if the target is looking in the agent's direction.
		float dotLookView = glm::dot(lookDir, targetDir);

		// Target will not be looking at the agent at a value v > -0.7
		// giving it a "range" to the view frustrum (like a cone)
		if (dotLookView > -0.7f)
		{
			// Target is not looking this way, stop fleeing
			result = 0;
		}
		else
		{
			// Target is looking this way enough, flee
			if (sqrDistance < 200.0f)
			{
				agentRigidbC->AddRelativeForce(glm::vec3(0.0f, 0.0f, -3.0f), nPhysics::ForceMode::Force);
				result = 1;
			}
			else
			{
				agentRigidbC->SetVelocity(glm::vec3(0.0f));
				result = 0;
			}
		}
	}
	

	// Rotate the agent
	float multiplier = ((dotRight >= 0.0f) ? 1.0f : -1.0f);
	printf("FinalAngle: %.1f\n", finalAngle);
	if (finalAngle > 0.1f)
	{
		glm::quat orientation = nQuaternionHelper::AdjOrientationEulerAngles(agentLocationC->GetWorldRotation(), glm::vec3(0.0f, 2.0f * multiplier, 0.0f), true);
		agentLocationC->SetWorldRotation(orientation);
		//agentRigidbC->angularAcceleration.y = 1.0f * multiplier;
	}
	else
	{
		//agentRigidbC->angularAcceleration.y = 0.0f;
	}

	return result;
}

int cBehaviourSystem::_UpdatePursueBehaviour(sAgent* agent)
{
	if (agent->pursueBehavC->target == nullptr)
	{
		return 0;
	}

	// Dissecting the method...

	// Getting variables
	sPursueBehaviourComponent* pursueC = static_cast<sPursueBehaviourComponent*>(agent->pursueBehavC);
	sLocationComponent* agentLocationC = static_cast<sLocationComponent*>(agent->agentLocatC);
	nPhysics::sSimpleRigidbodyComponent* agentRigidbC = static_cast<nPhysics::sSimpleRigidbodyComponent*>(agent->agentRigidbC);
	sLocationComponent* targetLocationC = static_cast<sLocationComponent*>(pursueC->target);

	// Get the agent and the target's normalized directions
	glm::vec3 agentDir = agentLocationC->GetForwardVector();
	glm::vec3 targetDir = targetLocationC->GetForwardVector();

	// Find the distance between the agent and the targets future position
	glm::vec3 lookVec = (targetLocationC->GetWorldPosition() + targetLocationC->GetForwardVector() * 10.0f) - agentLocationC->GetWorldPosition();
	float sqrDistance = nMathHelper::SquareMagnitude(lookVec);

	// Get the look direction
	glm::vec3 lookDir = glm::normalize(lookVec);

	// Get the right direction, so we can check the angle
	glm::vec3 rightDir = agentLocationC->GetRightVector();

	// Dot the directions to find their relations
	float dotDirection = glm::dot(agentDir, lookDir);

	// Dot the right direction so we can find which side the angle is on
	float dotRight = glm::dot(rightDir, lookDir);

	// Find the real angle between the agent and the target
	float finalAngle = glm::acos(dotDirection);

	int result = 0;

	// Move the agent
	if (sqrDistance > 50.0f)
	{
		agentRigidbC->AddRelativeForce(glm::vec3(0.0f, 0.0f, 3.0f), nPhysics::ForceMode::Force);
		result = 1;
	}
	else
	{
		agentRigidbC->SetVelocity(glm::vec3(0.0f));
		result = 0;
	}

	// Rotate the agent
	float multiplier = ((dotRight >= 0.0f) ? 1.0f : -1.0f);
	printf("FinalAngle: %.1f\n", finalAngle);
	if (finalAngle > 0.1f)
	{
		glm::quat orientation = nQuaternionHelper::AdjOrientationEulerAngles(agentLocationC->GetWorldRotation(), glm::vec3(0.0f, 2.0f * multiplier, 0.0f), true);
		agentLocationC->SetWorldRotation(orientation);
		//agentRigidbC->angularAcceleration.y = 1.0f * multiplier;
	}
	else
	{
		//agentRigidbC->angularAcceleration.y = 0.0f;
	}

	return result;
}

int cBehaviourSystem::_UpdateEvadeBehaviour(sAgent* agent)
{
	// Dissecting the method...

	// Getting variables
	sEvadeBehaviourComponent* evadeC = static_cast<sEvadeBehaviourComponent*>(agent->evadeBehavC);
	sLocationComponent* agentLocationC = static_cast<sLocationComponent*>(agent->agentLocatC);
	nPhysics::sSimpleRigidbodyComponent* agentRigidbC = static_cast<nPhysics::sSimpleRigidbodyComponent*>(agent->agentRigidbC);

	sLocationComponent* targetLocationC = nullptr;

	if (evadeC->behaviourType == eBehaviourTypes::SIMPLE)
	{
		if (agent->evadeBehavC->target == nullptr)
		{
			return 0;
		}
		else
		{
			targetLocationC = static_cast<sLocationComponent*>(evadeC->target);
		}
	}
	else if (evadeC->behaviourType == eBehaviourTypes::CLOSEST)
	{
		// Find closest threat.
		sEntity* closestThreat = this->GetClosestThreatByDistance(agent->agentLocatC->GetWorldPosition(), "PlayerBullet");
		if (closestThreat == nullptr)
		{
			return 0;
		}

		targetLocationC = closestThreat->GetComponent<sLocationComponent>();
		agent->closestThreat = closestThreat;
	}

	// Get the agent and the target's normalized directions
	glm::vec3 agentDir = agentLocationC->GetForwardVector();
	glm::vec3 targetDir = targetLocationC->GetForwardVector();

	// Find the distance between the agent and the target future position
	glm::vec3 lookVec = (targetLocationC->GetWorldPosition()) - agentLocationC->GetWorldPosition();
	float sqrDistance = nMathHelper::SquareMagnitude(lookVec);

	// Get the look direction
	glm::vec3 lookDir = glm::normalize(lookVec);

	// Get the right direction, so we can check the angle
	glm::vec3 rightDir = agentLocationC->GetRightVector();

	// Dot the directions to find their relations
	float dotDirection = glm::dot(agentDir, lookDir);

	// Dot the right direction so we can find which side the angle is on
	float dotRight = glm::dot(rightDir, lookDir);

	// Find the real angle between the agent and the target
	float finalAngle = glm::acos(dotDirection);

	int result = 0;

	// Move the agent
	if (sqrDistance < 100.0f)
	{
		agentRigidbC->AddRelativeForce(glm::vec3(0.0f, 0.0f, -3.0f), nPhysics::ForceMode::Force);
		result = 1;
	}
	else
	{
		agentRigidbC->SetVelocity(glm::vec3(0.0f));
		result = 0;
	}

	// Rotate the agent
	float multiplier = ((dotRight >= 0.0f) ? 1.0f : -1.0f);
	printf("FinalAngle: %.1f\n", finalAngle);
	if (finalAngle > 0.1f)
	{
		glm::quat orientation = nQuaternionHelper::AdjOrientationEulerAngles(agentLocationC->GetWorldRotation(), glm::vec3(0.0f, 2.0f * multiplier, 0.0f), true);
		agentLocationC->SetWorldRotation(orientation);
		//agentRigidbC->angularAcceleration.y = 1.0f * multiplier;
	}
	else
	{
		//agentRigidbC->angularAcceleration.y = 0.0f;
	}
	

	return result;
}

int cBehaviourSystem::_UpdateApproachBehaviour(sAgent* agent, double deltaTime)
{
	if (agent->approachBehavC->target == nullptr)
	{
		return 0;
	}

	// Dissecting the method...

	// Getting variables
	sApproachBehaviourComponent* approachC = static_cast<sApproachBehaviourComponent*>(agent->approachBehavC);
	sLocationComponent* agentLocationC = static_cast<sLocationComponent*>(agent->agentLocatC);
	nPhysics::sSimpleRigidbodyComponent* agentRigidbC = static_cast<nPhysics::sSimpleRigidbodyComponent*>(agent->agentRigidbC);
	sLocationComponent* targetLocationC = static_cast<sLocationComponent*>(approachC->target);

	// Get the agent and the target's normalized directions
	glm::vec3 agentDir = agentLocationC->GetForwardVector();
	glm::vec3 targetDir = targetLocationC->GetForwardVector();

	// Find the distance between the agent and the target future position
	glm::vec3 lookVec = targetLocationC->GetWorldPosition() - agentLocationC->GetWorldPosition();
	float sqrDistance = nMathHelper::SquareMagnitude(lookVec);

	// Get the look direction
	glm::vec3 lookDir = glm::normalize(lookVec);

	// Get the right direction, so we can check the angle
	glm::vec3 rightDir = agentLocationC->GetRightVector();

	// Dot the directions to find their relations
	float dotDirection = glm::dot(agentDir, lookDir);

	// Dot the right direction so we can find which side the angle is on
	float dotRight = glm::dot(rightDir, lookDir);

	// Find the real angle between the agent and the target
	float finalAngle = glm::acos(dotDirection);

	int result = 0;

	// Move the agent
	if (sqrDistance > 500.0f)
	{
		agentRigidbC->AddRelativeForce(glm::vec3(0.0f, 0.0f, 3.0f), nPhysics::ForceMode::Force);
		result = 1;
		//agentRigidbC->AddRelativeForce(glm::vec3(0.0f, 0.0f, 1.0f), nPhysics::ForceMode::Force);
		//agentRigidbC->acceleration.z = 1.0f;
	}
	else if (sqrDistance < 200.0f)
	{
		agentRigidbC->AddRelativeForce(glm::vec3(0.0f, 0.0f, -3.0f), nPhysics::ForceMode::Force);
		result = 1;
	}
	else
	{
		printf("Firing...\n");
		agentRigidbC->SetVelocity(glm::vec3(0.0f));

		if (approachC->coolDown <= 0)
		{
			// Get next bullet from the bullet pool
			sEntity* bulletEntity = cPoolManager::GetInstance()->GetEntityByName("EnemyBullet");

			// Set bullet at the muzzle position and orientate it
			//nPhysics::iRigidbody* rigidbodyC = static_cast<nPhysics::sSimpleRigidbodyComponent*>(bulletEntity->GetComponent(eComponentTypes::RIGIDBODY_COMPONENT));
			//sLocationComponent* bulletLocationC = static_cast<sLocationComponent*>(rigidbodyC->GetTransformComponent());
			//bulletLocationC->position = agentLocationC->GetWorldPosition() + agentLocationC->direction * 2.0f;
			//bulletLocationC->qOrientation = agentLocationC->qOrientation;

			// Activate
			//bulletEntity->bIsActive = true;

			// Add force to the bullet
			//rigidbodyC->SetIsKinematic(false);
			//rigidbodyC->AddForce(agentLocationC->direction * 1000.0f, nPhysics::ForceMode::Force);

			// Reset timer for next firing
			approachC->coolDown = 2.0f;
		}
		else
		{
			approachC->coolDown -= (float)deltaTime;
		}
		

		result = 0;
		//agentRigidbC->acceleration.z = 0.0f;
	}

	// Rotate the agent
	float multiplier = ((dotRight >= 0.0f) ? 1.0f : -1.0f);
	printf("FinalAngle: %.1f\n", finalAngle);
	if (finalAngle > 0.1f)
	{
		glm::quat orientation = nQuaternionHelper::AdjOrientationEulerAngles(agentLocationC->GetWorldRotation(), glm::vec3(0.0f, 2.0f * multiplier, 0.0f), true);
		agentLocationC->SetWorldRotation(orientation);
		//agentRigidbC->angularAcceleration.y = 1.0f * multiplier;
	}
	else
	{
		//agentRigidbC->angularAcceleration.y = 0.0f;
	}

	return result;
}

int cBehaviourSystem::_UpdateWanderBehaviour(sAgent* agent)
{
	// No radius means no wandering
	if (agent->wanderBehavC->radius == 0.0f)
	{
		return 0;
	}

	// Dissecting the method...

	// Getting variables
	sLocationComponent* agentLocationC = static_cast<sLocationComponent*>(agent->agentLocatC);
	nPhysics::iRigidbody* agentRigidbC = static_cast<nPhysics::iRigidbody*>(agent->agentRigidbC);

	// Get the agent and the target's normalized directions
	glm::vec3 agentDir = agentLocationC->GetForwardVector();

	// Find the distance between the agent and the target future position
	glm::vec3 lookVec = agent->wanderBehavC->wanderPoint - agentLocationC->GetWorldPosition();
	lookVec.y = 0.0f;
	//float sqrDistance = nMathHelper::SquareMagnitude(lookVec);

	// Get the look direction
	glm::vec3 lookDir = glm::normalize(lookVec);

	// Get the right direction, so we can check the angle
	glm::vec3 rightDir = agentLocationC->GetRightVector();

	// Dot the directions to find their relations
	float dotDirection = glm::dot(agentDir, lookDir);

	// Dot the right direction so we can find which side the angle is on
	float dotRight = glm::dot(rightDir, lookDir);

	// Find the real angle between the agent and the target
	float finalAngle = glm::acos(dotDirection);


	int result = 0;

	// Check min distance
	if (fabs(glm::distance(agentRigidbC->GetPosition(), agent->wanderBehavC->wanderPoint)) < 200.0f)
	{
		// Choose another point or stop wandering

		//agentRigidbC->SetVelocity(glm::vec3(0.0f));

		float randomDistance = nMathHelper::GetRandInRange<float>(1.0f, agent->wanderBehavC->radius);
		glm::vec3 newPoint = agent->wanderBehavC->center + glm::vec3(randomDistance, 0.0f, randomDistance);
		agent->wanderBehavC->wanderPoint = newPoint;
		//agent->idleBehavC->idle = true;

		return 0;
	}
	else
	{
		// Go to that point

		// Get the look direction
		//agentRigidbC->AddRelativeForce(glm::vec3(0.0f, 0.0f, 3.0f), nPhysics::ForceMode::Force);
		agentRigidbC->SetPosition(agentRigidbC->GetPosition() + lookDir * 0.1f);
		result = 1;
	}

	
	// Rotate the agent
	float multiplier = ((dotRight >= 0.0f) ? 1.0f : -1.0f);
	printf("FinalAngle: %.1f\n", finalAngle);
	if (finalAngle > 1.0f)
	{
		glm::quat orientation = nQuaternionHelper::AdjOrientationEulerAngles(agentLocationC->GetWorldRotation(), glm::vec3(0.0f, 3.0f * multiplier, 0.0f), true);
		agentLocationC->SetWorldRotation(orientation);
		agentRigidbC->SetRotation(agentLocationC->GetWorldRotation());
		//agentRigidbC->angularAcceleration.y = 1.0f * multiplier;
	}
	else
	{
		//agentRigidbC->angularAcceleration.y = 0.0f;
	}

	return result;
}

int cBehaviourSystem::_UpdateIdleBehaviour(sAgent* agent, double deltaTime)
{
	// Bail if idling is not wanted
	if (agent->idleBehavC->idle == false)
	{
		return 0;
	}

	// Check current idle state
	if (agent->currentIdleTime < agent->idleBehavC->idleTime)
	{
		agent->currentIdleTime += 1.0f * (float)deltaTime;
		printf("Idling...\n");
		return 1;
	}
	else
	{
		agent->currentIdleTime = 0.0f;
		agent->idleBehavC->idle = false;
		printf("Finished Idling...\n");
		return 0;
	}
}

sEntity* cBehaviourSystem::GetClosestThreatByDistance(glm::vec3 pointToCompare, std::string threatTag)
{
	cPoolManager* pool = cPoolManager::GetInstance();

	sEntity* currentClosestThreat = nullptr;
	glm::vec3 currentPosition = glm::vec3(0.0f);
	for (unsigned int i = 0; i != pool->vecBulletEntities.size(); i++)
	{
		sEntity* bullet = pool->vecBulletEntities[i];

		if (!bullet->GetIsActive() || bullet->GetTag() != threatTag)
		{
			continue;
		}

		sLocationComponent* bulletLoc = bullet->GetComponent<sLocationComponent>();

		// Begin assuming it's the first.
		if (i == 0)
		{
			currentClosestThreat = bullet;
			currentPosition = bulletLoc->GetWorldPosition();
		}
		else
		{
			// Check next bullet's distance for a closer one
			float dist1 = abs(glm::distance(pointToCompare, currentPosition));
			float dist2 = abs(glm::distance(pointToCompare, bulletLoc->GetWorldPosition()));

			if (dist2 < dist1)
			{
				// We have a new closest threat
				currentClosestThreat = bullet;
			}
		}
	}

	return currentClosestThreat;
}
