// Sort.cpp

#include <Math\Sort.h>
#include <Entity\Components\sLocationComponent.h>

void BubbleSort(std::vector<sEntity*>& copyVecObjectsToDraw, glm::vec3 comparePosition)
{
	for (unsigned int i = 0; i != copyVecObjectsToDraw.size(); i++)
	{
		if (i + 1 != copyVecObjectsToDraw.size())
		{
			sLocationComponent* locationComponent1 = copyVecObjectsToDraw[i]->GetComponent<sLocationComponent>();
			sLocationComponent* locationComponent2 = copyVecObjectsToDraw[i + 1]->GetComponent<sLocationComponent>();
			float distance1 = glm::distance(locationComponent1->GetWorldPosition(), comparePosition);
			float distance2 = glm::distance(locationComponent2->GetWorldPosition(), comparePosition);
			if (abs(distance2) > abs(distance1))
			{
				sEntity* temp = copyVecObjectsToDraw[i];
				copyVecObjectsToDraw[i] = copyVecObjectsToDraw[i + 1];
				copyVecObjectsToDraw[i + 1] = temp;
			}
		}
	}
}