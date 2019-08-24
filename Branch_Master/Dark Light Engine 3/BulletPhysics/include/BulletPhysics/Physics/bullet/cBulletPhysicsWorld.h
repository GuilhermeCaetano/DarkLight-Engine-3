#ifndef _cBulletPhysicsWorld_HG_
#define _cBulletPhysicsWorld_HG_

#include "iBulletCompoundBody.h"
#include "btBulletDynamicsCommon.h"
#include "BulletSoftBody\btSoftRigidDynamicsWorld.h"
#include "cBtDebugRenderer.h"
#include <Interfaces\Physics\iPhysicsSystem.h>
#include <Interfaces\Physics\iCollisionBody.h>
#include <vector>

struct iEntity;


namespace nPhysics
{
	class cBulletPhysicsWorld : public iPhysicsSystem
	{
	public:
		cBulletPhysicsWorld();
		virtual ~cBulletPhysicsWorld();

		virtual void SetGravity(const glm::vec3& gravity);
		virtual void SetDebugRenderer(iDebugRenderer* pDebugRenderer);

		virtual bool ScheduleEntity(iEntity* entityToSchedule, iCollisionBody* collisionBody);
		virtual bool UnscheduleEntity(iEntity* entityToSchedule, iCollisionBody* collisionBody);

		virtual void InitializeSystem();
		virtual void UpdateMatView(const glm::mat4& matView);
		virtual void UpdateSystem(double deltaTime);

		btDynamicsWorld* GetWorld();

	protected:
		cBulletPhysicsWorld(const cBulletPhysicsWorld& other) : iPhysicsSystem(other) {};
		cBulletPhysicsWorld& operator-(const cBulletPhysicsWorld& other) { return *this; }

	private:
		std::vector<struct sBtRigidbodyComponent*> _vecBulletRb;
		std::vector<struct sBtSoftbodyComponent*> _vecBulletSb;
		std::vector<iBulletCompoundBody*> _vecBulletCb;
		std::vector<btTypedConstraint*> _vecConstraints;

		btTransform _matView;

		//collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
		btDefaultCollisionConfiguration* _collisionConfiguration;

		//use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
		btCollisionDispatcher* _dispatcher;

		//btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
		btBroadphaseInterface* _overlappingPairCache;

		//the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
		btSequentialImpulseConstraintSolver* _solver;

		btDiscreteDynamicsWorld* _dynamicsWorld;

		btSoftRigidDynamicsWorld* _softRigidDynamicsWorld;

		cBtDebugRenderer* _btDebugRenderer;

	};
}



#endif // !_cBulletPhysicsWorld_HG_

