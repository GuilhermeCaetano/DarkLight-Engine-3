// cAISystem.cpp

#include <Entity\Components\AI\cAISystem.h>
#include <Entity\Components\AI\sFlockComponent.h>


cAISystem::cAISystem()
{
	return;
}

cAISystem::~cAISystem()
{
	return;
}

void cAISystem::ScheduleEntity(sEntity* newEntity, iComponent* mainComponent)
{
	sFlockComponent* flockC = static_cast<sFlockComponent*>(mainComponent);

	sFlockInfo* newFlock = new sFlockInfo(newEntity, flockC);
	this->_vecFlocks.push_back(newFlock);
}

void cAISystem::UnscheduleEntity(sEntity* entity)
{
	for (unsigned int i = 0; i < this->_vecFlocks.size(); i++)
	{
		if (this->_vecFlocks[i]->entity == entity)
		{
			this->_vecFlocks[i] = nullptr;
		}
	}
}

void cAISystem::UpdateAISystem(GLFWwindow* window, double deltaTime)
{
	this->_UpdateFlocking(window, deltaTime);
}

void cAISystem::_UpdateFlocking(GLFWwindow* window, double deltaTime)
{
	for (unsigned int i = 0; i < this->_vecFlocks.size(); i++)
	{
		sFlockInfo* flockInfo = this->_vecFlocks[i];

		// Make flock update all the boids
		flockInfo->flock->CalculateSteering();

		if (glfwGetKey(window, GLFW_KEY_I))
		{
			flockInfo->flock->_cohesionWeight += 0.1f;
			if (flockInfo->flock->_cohesionWeight > 1.0f) { flockInfo->flock->_cohesionWeight = 1.0f; }
		}
		if (glfwGetKey(window, GLFW_KEY_J))
		{
			flockInfo->flock->_cohesionWeight -= 0.1f;
			if (flockInfo->flock->_cohesionWeight < 0.0f) { flockInfo->flock->_cohesionWeight = 0.0f; }
		}
		if (glfwGetKey(window, GLFW_KEY_O))
		{
			flockInfo->flock->_separationWeight += 0.1f;
			if (flockInfo->flock->_separationWeight > 1.0f) { flockInfo->flock->_separationWeight = 1.0f; }
		}
		if (glfwGetKey(window, GLFW_KEY_K))
		{
			flockInfo->flock->_separationWeight -= 0.1f;
			if (flockInfo->flock->_separationWeight < 0.0f) { flockInfo->flock->_separationWeight = 0.0f; }
		}
		if (glfwGetKey(window, GLFW_KEY_P))
		{
			flockInfo->flock->_alignmentWeight += 0.1f;
			if (flockInfo->flock->_alignmentWeight > 1.0f) { flockInfo->flock->_alignmentWeight = 1.0f; }
		}
		if (glfwGetKey(window, GLFW_KEY_L))
		{
			flockInfo->flock->_alignmentWeight -= 0.1f;
			if (flockInfo->flock->_alignmentWeight < 0.0f) { flockInfo->flock->_alignmentWeight = 0.0f; }
		}

		if (flockInfo->flock->masterFlock == nullptr)
		{
			if (glfwGetKey(window, GLFW_KEY_1))
			{
				flockInfo->flock->SetFormation(sFlockComponent::CIRCLE, glm::vec3(0.0f), glm::vec3(20.0f, 0.0f, 0.0f));
			}
			else if (glfwGetKey(window, GLFW_KEY_2))
			{
				flockInfo->flock->SetFormation(sFlockComponent::V, glm::vec3(0.0f), glm::vec3(10.0f, 0.0f, 0.0f));
			}
			else if (glfwGetKey(window, GLFW_KEY_3))
			{
				flockInfo->flock->SetFormation(sFlockComponent::SQUARE, glm::vec3(0.0f), glm::vec3(10.0f, 0.0f, 0.0f));
			}
			else if (glfwGetKey(window, GLFW_KEY_4))
			{
				flockInfo->flock->SetFormation(sFlockComponent::LINE, glm::vec3(0.0f), glm::vec3(10.0f, 0.0f, 0.0f));
			}
			else if (glfwGetKey(window, GLFW_KEY_5))
			{
				flockInfo->flock->SetFormation(sFlockComponent::TWOROWS, glm::vec3(0.0f), glm::vec3(10.0f, 0.0f, 0.0f));
			}
			else if (glfwGetKey(window, GLFW_KEY_6))
			{
				flockInfo->flock->SetFormation(sFlockComponent::FLOCK, glm::vec3(0.0f), glm::vec3(10.0f, 0.0f, 0.0f));
			}
			else if (glfwGetKey(window, GLFW_KEY_7))
			{
				flockInfo->flock->RecallFormation();
			}
			else if (glfwGetKey(window, GLFW_KEY_8))
			{
				flockInfo->flock->SetPathFollow(true, 0);
			}
			else if (glfwGetKey(window, GLFW_KEY_9))
			{
				flockInfo->flock->SetPathFollow(true, 1);
			}
			else if (glfwGetKey(window, GLFW_KEY_0))
			{
				flockInfo->flock->SetPathFollow(false, 0);
			}
		}
	}

	for (unsigned int i = 0; i < this->_vecFlocks.size(); i++)
	{
		sFlockInfo* flockInfo = this->_vecFlocks[i];

		if (flockInfo->flock->masterFlock != nullptr)
		{
			if (flockInfo->flock->masterFlock->flockFormation == flockInfo->flock->FLOCK)
			{
				// Steer boids
				sEntity* boid = flockInfo->flock->flockEntity;
				/*glm::quat fuckthisshit = glm::quat(glm::inverse(glm::lookAt(boid->transform->position, boid->transform->position - flockInfo->flock->GetFlockSteering(), glm::vec3(0.0f, 1.0f, 0.0f))));
				float val = glm::dot(boid->transform->direction, flockInfo->flock->GetFlockSteering());

				glm::quat fuckcrapshitfuck = glm::mix(boid->transform->qOrientation, fuckthisshit, val);*/
				glm::quat targetRotation = glm::quatLookAt(-flockInfo->flock->GetFlockSteering(), glm::vec3(0.0f, 1.0f, 0.0f));
				boid->transform->SetWorldRotation(targetRotation);

				boid->transform->SetWorldPosition(boid->transform->GetWorldPosition() + flockInfo->flock->GetFlockSteering() * flockInfo->flock->flockSpeed * (float)deltaTime);
			}
		}
		else
		{
			if (flockInfo->flock->followPath)
			{
				flockInfo->flock->PathFollow(deltaTime);
			}
			if (flockInfo->flock->flockFormation != flockInfo->flock->FLOCK)
			{
				flockInfo->flock->MakeFormation();
			}
		}
	}

	

	//for (unsigned int i = 0; i < this->_vecFlocks.size(); i++)
	//{
	//	sFlockInfo* flockInfo = this->_vecFlocks[i];

	//	// Make flock update all the boids
	//	flockInfo->flock->CalculateSteering();

	//	if (flockInfo->flock->masterFlock != nullptr)
	//	{
	//		// Steer boids
	//		sEntity* boid = flockInfo->flock->flockEntity;
	//		boid->transform->position += flockInfo->flock->GetFlockSteering() * flockInfo->flock->flockSpeed * (float)deltaTime;
	//	}
	//	else
	//	{
	//		if (glfwGetKey(window, GLFW_KEY_W))
	//		{
	//			flockInfo->entity->transform->position.z += (float)deltaTime * flockInfo->flock->flockSpeed;
	//		}
	//	}
	//}
}
