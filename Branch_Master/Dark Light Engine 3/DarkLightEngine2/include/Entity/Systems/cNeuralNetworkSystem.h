#ifndef _cNeuralNetworkSystem_HG_
#define _cNeuralNetworkSystem_HG_

#include "iSystem.h"
#include <OpenGL\OpenGLHeaders.h>
#include <Entity\Components\AI\sNeuralNetworkComponent.h>
#include <Entity\sEntity.h>
#include <vector>


class cNeuralNetworkSystem : public iSystem
{
public:
	cNeuralNetworkSystem();
	~cNeuralNetworkSystem();

	bool CastDetectors(const glm::vec3& position, const glm::vec3& forward, sEntity* obj, sNeuralNetworkComponent* neuralC, std::vector<float>& collisions);
	float CheckDetectors(sEntity* obj, const glm::vec3& position, const glm::vec3& direction, float maxDistance);

	void MoveForward();

	void SteerLeft();
	void SteerRight();

	void Death();

	// Updates the system
	virtual void UpdateSystem(double deltaTime);

	// Schedules and unschedule specific components
	// in order to save searching memory
	virtual bool ScheduleEntity(sEntity* entity, iComponent* component);
	virtual bool UnscheduleEntity(sEntity* entity, iComponent* component);

};


#endif // !_cNeuralNetworkSystem_HG_

