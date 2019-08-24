// cBulletPhysicsWorld.cpp

#include <cBulletPhysicsWorld.h>
#include <nConvert.h>
#include <sBtRigidbodyComponent.h>
#include <sBtSoftbodyComponent.h>
#include <Entity\Components\sLocationComponent.h>
#include <Entity\Components\sMeshComponent.h>
#include <sBtCompoundBodyComponent.h>
#include <BulletSoftBody\btSoftBody.h>
#include "BulletCollision\CollisionDispatch\btGhostObject.h"
#include "cBtDebugRenderer.h"

// btcollisiondispatcher
// override check collision for both kinematics or kineatic and static

namespace nPhysics
{
	cBulletPhysicsWorld::cBulletPhysicsWorld() : iPhysicsSystem()
	{
		//collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
		this->_collisionConfiguration = new btDefaultCollisionConfiguration();

		//use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
		this->_dispatcher = new btCollisionDispatcher(this->_collisionConfiguration);

		//btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
		this->_overlappingPairCache = new btDbvtBroadphase();
		
		// Set ghost objects callback
		this->_overlappingPairCache->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());

		//the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
		this->_solver = new btSequentialImpulseConstraintSolver;

		this->_dynamicsWorld = new btDiscreteDynamicsWorld(this->_dispatcher, this->_overlappingPairCache, this->_solver, this->_collisionConfiguration);

		this->_dynamicsWorld->setGravity(btVector3(0, -9.81f, 0));

		return;
	}

	cBulletPhysicsWorld::~cBulletPhysicsWorld()
	{
		if (this->_dynamicsWorld)
		{
			delete this->_dynamicsWorld;
			this->_dynamicsWorld = nullptr;
		}

		if (this->_solver)
		{
			delete this->_solver;
			this->_solver = nullptr;
		}

		if (this->_overlappingPairCache)
		{
			delete this->_overlappingPairCache;
			this->_overlappingPairCache = nullptr;
		}

		if (this->_dispatcher)
		{
			delete this->_dispatcher;
			this->_dispatcher = nullptr;
		}

		if (this->_collisionConfiguration)
		{
			delete this->_collisionConfiguration;
			this->_collisionConfiguration = nullptr;
		}

		if (this->_btDebugRenderer)
		{
			delete this->_btDebugRenderer;
		}

		return;
	}

	void cBulletPhysicsWorld::SetGravity(const glm::vec3& gravity)
	{
		this->_dynamicsWorld->setGravity(nConvert::ToBullet(gravity));
	}

	void cBulletPhysicsWorld::SetDebugRenderer(iDebugRenderer* pDebugRenderer)
	{
		this->_btDebugRenderer = new cBtDebugRenderer(pDebugRenderer);
		btIDebugDraw* tempDebug = static_cast<btIDebugDraw*>(this->_btDebugRenderer);
		this->_dynamicsWorld->setDebugDrawer(tempDebug);
	}

	bool cBulletPhysicsWorld::ScheduleEntity(iEntity* entityToSchedule, iCollisionBody* collisioBody)
	{
		if (collisioBody->GetBodyType() == BODY_TYPE_RIGIDBODY)
		{
			sBtRigidbodyComponent* bulletBody = static_cast<sBtRigidbodyComponent*>(collisioBody);
			if (!bulletBody)
			{
				return false;
			}

			this->_vecBulletRb.push_back(bulletBody);
		}
		else if (collisioBody->GetBodyType() == BODY_TYPE_SOFTBODY)
		{
			sBtSoftbodyComponent* bulletBody = static_cast<sBtSoftbodyComponent*>(collisioBody);
			if (!bulletBody)
			{
				return false;
			}

			this->_vecBulletSb.push_back(bulletBody);
		}
		else if (collisioBody->GetBodyType() == BODY_TYPE_COMPOUNDBODY)
		{
			iBulletCompoundBody* bulletBody = static_cast<iBulletCompoundBody*>(collisioBody);
			if (!bulletBody)
			{
				return false;
			}

			this->_vecBulletCb.push_back(bulletBody);

			//unsigned int numRigidbodies = bulletBody->GetNumRigidbodies();
			//unsigned int numConstraints = bulletBody->GetNumConstraints();

			/*for (unsigned int i = 0; i < numRigidbodies; i++)
			{
				sBulletRigidbodyComponent* rb = bulletBody->GetRigidbody(i);
				this->_vecBulletRb.push_back(rb);
			}*/
			/*for (unsigned int i = 0; i < numConstraints; i++)
			{
				btTypedConstraint* constraint = bulletBody->GetConstraint(i);
				this->_vecConstraints.push_back(constraint);
			}*/
		}
		
		return true;
	}

	bool cBulletPhysicsWorld::UnscheduleEntity(iEntity* entityToSchedule, iCollisionBody* collisioBody)
	{
		if (collisioBody->GetBodyType() == BODY_TYPE_RIGIDBODY)
		{
			sBtRigidbodyComponent* bulletBody = static_cast<sBtRigidbodyComponent*>(collisioBody);
			if (!bulletBody)
			{
				return false;
			}

			btRigidBody* bulletBtBody = bulletBody->GetBtRigidbody();
			this->_dynamicsWorld->removeRigidBody(bulletBtBody);
			return true;
		}
		/*else if (collisioBody->GetBodyType() == BODY_TYPE_SOFTBODY)
		{
			sBulletSoftbodyComponent* bulletBody = static_cast<sBulletSoftbodyComponent*>(collisioBody);
			if (!bulletBody)
			{
				return false;
			}

			btSoftBody* bulletBtBody = bulletBody->GetBulletBody();
			this->_dynamicsWorld->re
			return true;
		}*/

		return false;
	}

	void cBulletPhysicsWorld::InitializeSystem()
	{
		for (unsigned int i = 0; i != this->_vecBulletRb.size(); i++)
		{
			this->_dynamicsWorld->addRigidBody(this->_vecBulletRb[i]->GetBtRigidbody());
		}
		for (unsigned int i = 0; i != this->_vecBulletCb.size(); i++)
		{
			iBulletCompoundBody* bulletBody = this->_vecBulletCb[i];

			unsigned int numConstraints = bulletBody->GetNumConstraints();

			for (unsigned int i = 0; i < numConstraints; i++)
			{
				btTypedConstraint* constraint = bulletBody->GetBtConstraint(i);
				this->_vecConstraints.push_back(constraint);
			}
		}
		for (unsigned int i = 0; i != this->_vecConstraints.size(); i++)
		{
			this->_dynamicsWorld->addConstraint(this->_vecConstraints[i]);
		}
	}

	void cBulletPhysicsWorld::UpdateMatView(const glm::mat4& matView)
	{
		this->_matView = nConvert::ToBullet(matView);
	}

	void cBulletPhysicsWorld::UpdateSystem(double deltaTime)
	{
		this->_dynamicsWorld->stepSimulation(btScalar(deltaTime));

		for (unsigned int i = 0; i != this->_vecBulletRb.size(); i++)
		{
			sBtRigidbodyComponent* bulletB = this->_vecBulletRb[i];
			if (bulletB->GetShape()->GetShapeType() == eShapeTypes::SHAPE_TYPE_PLANE)
			{
				continue;
			}

			// Only get position from the rigidbody if the body is not kinematic
			// otherwise, do the inverse and set from the entity position
			/*if (bulletB->GetIsKinematic())
			{
				continue;
			}*/

			glm::mat4 moveMatrix = bulletB->GetTransform();

			glm::vec3 position = moveMatrix[3];
			glm::quat rotation = glm::quat(moveMatrix);

			//bool freezePosition = (bulletB->GetFreezePosition().x == true && bulletB->GetFreezePosition().y == true && bulletB->GetFreezePosition().z == true);
			//bool freezeRotation = (bulletB->GetFreezeRotation().x == true && bulletB->GetFreezeRotation().y == true && bulletB->GetFreezeRotation().z == true);

			bulletB->GetTransformComponent()->GetWorldPosition() = position - (rotation * bulletB->GetShapeOffset());
			bulletB->GetTransformComponent()->GetWorldRotation() = rotation;
		}
	}

	btDynamicsWorld* cBulletPhysicsWorld::GetWorld()
	{
		return this->_dynamicsWorld;
	}
}
