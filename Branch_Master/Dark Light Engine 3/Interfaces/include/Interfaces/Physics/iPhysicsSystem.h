#ifndef _iPhysicsSystem_HG_
#define _iPhysicsSystem_HG_

#include <OpenGL\OpenGLHeaders.h>
#include <Interfaces\iDebugRenderer.h>
#include <Interfaces\Physics\iEntity.h>
#include <Interfaces\Physics\iCollisionBody.h>


namespace nPhysics
{
	class iPhysicsSystem
	{
	public:
		virtual ~iPhysicsSystem() {};

		virtual void SetGravity(const glm::vec3& gravity) = 0;
		virtual void SetDebugRenderer(iDebugRenderer* pDebugRenderer) = 0;

		virtual bool ScheduleEntity(iEntity* entityToSchedule, iCollisionBody* rigidbodyC) = 0;
		virtual bool UnscheduleEntity(iEntity* entityToSchedule, iCollisionBody* rigidbodyComp) = 0;

		virtual void InitializeSystem() = 0;
		virtual void UpdateSystem(double deltaTime) = 0;

	protected:
		iPhysicsSystem() {};
		iPhysicsSystem(const iPhysicsSystem& other) {};
	};
}


#endif // !_iPhysicsSystem_HG_
