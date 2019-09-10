#ifndef _iSystem_HG_
#define _iSystem_HG_

#include <Entity\sEntity.h>
#include <Entity\Components\iComponent.h>


class iSystem
{
public:
	virtual ~iSystem() {};

	// Updates the system
	virtual void UpdateSystem(double deltaTime) = 0;

	// Schedules and unschedule specific components
	// in order to save searching memory
	virtual bool ScheduleEntity(sEntity* entity, iComponent* component) = 0;
	virtual bool UnscheduleEntity(sEntity* entity, iComponent* component) = 0;

protected:
	virtual void _Cleanup() = 0;
};


#endif // !_iSystem_HG_
