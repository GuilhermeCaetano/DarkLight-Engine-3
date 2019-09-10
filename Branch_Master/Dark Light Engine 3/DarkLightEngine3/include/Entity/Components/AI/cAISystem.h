#ifndef _cAISystem_HG_
#define _cAISystem_HG_

#include <Entity\sEntity.h>
#include <vector>


class cAISystem
{
public:
	cAISystem();
	~cAISystem();

	void ScheduleEntity(sEntity* newEntity, iComponent* mainComponent);
	void UnscheduleEntity(sEntity* newEntity);

	void UpdateAISystem(GLFWwindow* window, double deltaTime);
	
private:
	struct sFlockInfo
	{
		sFlockInfo(sEntity* newEntity, struct sFlockComponent* flockC)
		{
			entity = newEntity;
			this->flock = flockC;
		}

		sEntity* entity;
		sFlockComponent* flock;
	};

	std::vector<sFlockInfo*> _vecFlocks;

	void _UpdateFlocking(GLFWwindow* window, double deltaTime);

};



#endif // !_cAISystem_HG_

