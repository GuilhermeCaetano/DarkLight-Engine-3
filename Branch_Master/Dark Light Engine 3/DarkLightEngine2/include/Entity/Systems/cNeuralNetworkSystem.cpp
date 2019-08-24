// cNeuralNetworkSystem.cpp

#include "cNeuralNetworkSystem.h"
#include <Pools\cPoolManager.h>
#include <Math\nMathHelper.h>
#include <Math\nQuaternionHelper.h>


cNeuralNetworkSystem::cNeuralNetworkSystem()
{
	return;
}

cNeuralNetworkSystem::~cNeuralNetworkSystem()
{
	return;
}

bool cNeuralNetworkSystem::CastDetectors(const glm::vec3& position, const glm::vec3& forward, sEntity* obj, sNeuralNetworkComponent* neuralC, std::vector<float>& collisions)
{
	cPoolManager* tempPool = cPoolManager::GetInstance();

	// Create left and right 4 detectors. Forward itself is the center one.
	glm::vec3 dirLeft1 = forward;
	glm::vec3 dirLeft2 = forward;
	glm::vec3 dirRight1 = forward;
	glm::vec3 dirRight2 = forward;

	// Rotate vector to appropriate position
	dirLeft1 = nMathHelper::GetRelativeAxis(dirLeft1, glm::quat(glm::radians(glm::vec3(0.0f, 30.0f, 0.0f))));
	dirLeft2 = nMathHelper::GetRelativeAxis(dirLeft2, glm::quat(glm::radians(glm::vec3(0.0f, 60.0f, 0.0f))));
	dirRight1 = nMathHelper::GetRelativeAxis(dirRight1, glm::quat(glm::radians(glm::vec3(0.0f, -30.0f, 0.0f))));
	dirRight2 = nMathHelper::GetRelativeAxis(dirRight2, glm::quat(glm::radians(glm::vec3(0.0f, -60.0f, 0.0f))));

	// Cast ray and check for collision
	float colForward = this->CheckDetectors(obj, position, forward, neuralC->maxDistance);
	float colLeft_1 = this->CheckDetectors(obj, position, dirLeft1, neuralC->maxDistance);
	float colLeft_2 = this->CheckDetectors(obj, position, dirLeft2, neuralC->maxDistance);
	float colRight_1 = this->CheckDetectors(obj, position, dirRight1, neuralC->maxDistance);
	float colRight_2 = this->CheckDetectors(obj, position, dirRight2, neuralC->maxDistance);

	collisions.push_back(colForward);
	collisions.push_back(colLeft_1);
	collisions.push_back(colLeft_2);
	collisions.push_back(colRight_1);
	collisions.push_back(colRight_2);

	neuralC->detectorPoints[0] = position + forward * colForward * neuralC->maxDistance;
	neuralC->detectorPoints[1] = position + dirLeft1 * colLeft_1 * neuralC->maxDistance;
	neuralC->detectorPoints[2] = position + dirLeft2 * colLeft_2 * neuralC->maxDistance;
	neuralC->detectorPoints[3] = position + dirRight1 * colRight_1 * neuralC->maxDistance;
	neuralC->detectorPoints[4] = position + dirRight2 * colRight_2 * neuralC->maxDistance;

	return true;
}

float cNeuralNetworkSystem::CheckDetectors(sEntity* obj, const glm::vec3& position, const glm::vec3& direction, float maxDistance)
{
	cPoolManager* tempPool = cPoolManager::GetInstance();

	int itterations = 5;
	float averageDist = maxDistance / itterations;  

	for (unsigned int i = 0; i < itterations + 1; i++) // Since we also want to check the current pos
	{
		for (unsigned int j = 0; j < tempPool->vecStaticEntities.size(); j++)
		{
			sEntity* entity = tempPool->vecStaticEntities[j];
			if (entity->GetTag() == "Obstacle" && entity != obj)
			{
				glm::vec3 checkPoint = position + (direction * (i * averageDist));
				float distance = fabs(glm::distance(entity->transform->GetWorldPosition(), checkPoint));
				if (distance <= maxDistance)
				{
					distance = glm::min(maxDistance - 0.1f, distance);
					return distance / maxDistance;
				}
			}
		}
	}

	return 1.0f;
}

void cNeuralNetworkSystem::UpdateSystem(double deltaTime)
{
	cPoolManager* tempPool = cPoolManager::GetInstance();

	for (unsigned int i = 0; i < tempPool->vecStaticEntities.size(); i++)
	{
		sEntity* entity = tempPool->vecStaticEntities[i];
		sNeuralNetworkComponent* neuralC = entity->GetComponent<sNeuralNetworkComponent>();

		if (neuralC != nullptr)
		{
			std::vector<float> collisions;

			// Cast detectors to retrieve collision information
			bool result = this->CastDetectors(entity->transform->GetWorldPosition(), entity->transform->GetForwardVector(), entity, neuralC, collisions);

			// Check if there was an actual collision
			if (!result)
			{
				return;
			}

			// Steer left or right
			float steerFront = 30.0f * (1.0f - collisions[0]);
			float steerRight_1 = 30.0f * (1.0f - collisions[1]);
			float steerRight_2 = 45.0f * (1.0f - collisions[2]);
			float steerLeft_1 = -30.0f * (1.0f - collisions[3]);
			float steerLeft_2 = -45.0f * (1.0f - collisions[4]);

			glm::quat finalRotation = glm::quat();
			float speed = 1.0f;
			bool shouldRotate = false;

			if (steerFront > 0.0f)
			{
				finalRotation = nQuaternionHelper::AdjOrientationEulerAngles(entity->transform->GetWorldRotation(), glm::vec3(0.0f, steerFront, 0.0f), true);
				speed = 1.0f - collisions[0];
				shouldRotate = true;
			}
			if (steerRight_2 > 0.0f)
			{
				finalRotation = nQuaternionHelper::AdjOrientationEulerAngles(entity->transform->GetWorldRotation(), glm::vec3(0.0f, steerRight_2, 0.0f), true);
				speed = 1.0f - collisions[1];
				shouldRotate = true;
			}
			else if (steerRight_1 > 0.0f)
			{
				finalRotation = nQuaternionHelper::AdjOrientationEulerAngles(entity->transform->GetWorldRotation(), glm::vec3(0.0f, steerRight_1, 0.0f), true);
				speed = 1.0f - collisions[2];
				shouldRotate = true;
			}
			if (steerLeft_2 > 0.0f)
			{
				finalRotation = nQuaternionHelper::AdjOrientationEulerAngles(entity->transform->GetWorldRotation(), glm::vec3(0.0f, steerLeft_2, 0.0f), true);
				speed = 1.0f - collisions[3];
				shouldRotate = true;
			}
			else if (steerLeft_1 > 0.0f)
			{
				finalRotation = nQuaternionHelper::AdjOrientationEulerAngles(entity->transform->GetWorldRotation(), glm::vec3(0.0f, steerLeft_1, 0.0f), true);
				speed = 1.0f - collisions[4];
				shouldRotate = true;
			}

			float curAngle = glm::degrees(glm::angle(finalRotation));

			

			if (shouldRotate)
			{
				/*if (curAngle > 315.0f)
				{
					glm::quat adjRot = nQuaternionHelper::SetOrientationEulerAngles(glm::vec3(0.0f, 315.0f, 0.0f), true);
					finalRotation = adjRot;
				}
				else if (curAngle > 45.0f)
				{
					glm::quat adjRot = nQuaternionHelper::SetOrientationEulerAngles(glm::vec3(0.0f, 45.0f, 0.0f), true);
					finalRotation = adjRot;
				}*/

				entity->transform->SetWorldRotation(finalRotation);
			}
			

			neuralC->SetWeights(collisions);

			// Move forward
			entity->transform->SetWorldPosition(entity->transform->GetWorldPosition() + entity->transform->GetForwardVector() * speed * (float)deltaTime);
		}
	}
}

bool cNeuralNetworkSystem::ScheduleEntity(sEntity* entity, iComponent* component)
{
	return false;
}

bool cNeuralNetworkSystem::UnscheduleEntity(sEntity* entity, iComponent* component)
{
	return false;
}
