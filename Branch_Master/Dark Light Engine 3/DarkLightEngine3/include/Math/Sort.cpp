// Sort.cpp

#include <Math\Sort.h>


void BubbleSort(std::vector<std::pair<sEntity*, sMeshComponent*>>& copyVecObjectsToDraw, glm::vec3 comparePosition)
{
	for (unsigned int i = 0; i != copyVecObjectsToDraw.size(); i++)
	{
		if (i + 1 != copyVecObjectsToDraw.size())
		{
			glm::vec3 position1 = copyVecObjectsToDraw[i].first->GetTransform()->GetWorldPosition();
			glm::vec3 position2 = copyVecObjectsToDraw[i + 1].first->GetTransform()->GetWorldPosition();
			float distance1 = glm::distance(position1, comparePosition);
			float distance2 = glm::distance(position2, comparePosition);
			if (abs(distance2) > abs(distance1))
			{
				std::pair<sEntity*, sMeshComponent*> temp = copyVecObjectsToDraw[i];
				copyVecObjectsToDraw[i] = copyVecObjectsToDraw[i + 1];
				copyVecObjectsToDraw[i + 1] = temp;
			}
		}
	}
}