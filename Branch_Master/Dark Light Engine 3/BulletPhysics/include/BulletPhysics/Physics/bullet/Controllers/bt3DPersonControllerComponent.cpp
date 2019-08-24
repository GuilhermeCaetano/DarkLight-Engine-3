// bt3DPersonControllerComponent.cpp

#include "bt3DPersonControllerComponent.h"

#include <cBulletPhysicsWorld.h>
#include <Interfaces\Physics\iEntity.h>
#include <nConvert.h>
#include <BulletCollision\CollisionDispatch\btGhostObject.h>
#include <BulletCollision\CollisionShapes\btMultiSphereShape.h>
#include <BulletCollision\BroadphaseCollision\btOverlappingPairCache.h>
#include <BulletCollision\BroadphaseCollision\btCollisionAlgorithm.h>
#include <BulletCollision\BroadphaseCollision\btAxisSweep3.h>
#include <BulletCollision\CollisionDispatch\btCollisionWorld.h>
#include <LinearMath\btDefaultMotionState.h>
#include <sBtRigidbodyComponent.h>
#include <sBtCompoundBodyComponent.h>
#include <Entity\Components\sLocationComponent.h>


namespace nPhysics
{
	bt3DPersonControllerComponent::bt3DPersonControllerComponent(unsigned long long entityID)
	{
		this->_parentID = entityID;
		this->Start();

		return;
	}

	bt3DPersonControllerComponent::~bt3DPersonControllerComponent()
	{
		if (this->_ghostObject)
		{
			delete this->_ghostObject;
		}
		if (this->_ghostObjectHand)
		{
			delete this->_ghostObjectHand;
		}

		return;
	}

	void bt3DPersonControllerComponent::Start()
	{
		this->_moveState.gravity = glm::vec3(0.0f, -(9.81f * 9.81f), 0.0f);
		this->_bIsControllable = true;
		this->_bTouchingContact = false;
		this->_btDebugRenderer = nullptr;

		this->_idleName = "Idle";
		this->_walkName = "Walk";
		this->_runName = "Run";
		this->_jumpName = "JumpUp";
		this->_fallName = "JumpAir";
		this->_landName = "JumpLand2";
		this->_crouchName = "Crouch";
		this->_flyName = "Fly";
		this->_landHardName = "LandHard";
		this->_attack1Name = "Attack_1";

		this->animationState = "Idle";

		this->currentVertexPos = glm::vec3(0.0f);
	}

	void bt3DPersonControllerComponent::updateAction(btCollisionWorld* collisionWorld, btScalar deltaTimeStep)
	{
		// Upcast to our bullet world implementation
		if (this->_collisionWorld == nullptr)
		{
			this->_collisionWorld = collisionWorld;
		}

		// Check pending objects to be added to the world
		if (this->_vecPendingGhostBodies.size() > 0)
		{
			for (unsigned int i = 0; i < this->_vecPendingGhostBodies.size(); i++)
			{
				this->_collisionWorld->addCollisionObject(this->_vecPendingGhostBodies[i]);
			}

			this->_vecPendingGhostBodies.clear();
		}

		// Go into pre time step to set things for moving.
		this->PreStep(deltaTimeStep);

		// Now move
		this->TimeStep(deltaTimeStep);

		//TODO: Send collision to a collision dispatcher
		//this->CollisionDispatcher(std::make_pair(rayInfo, otherInfo));
	}

	void bt3DPersonControllerComponent::debugDraw(btIDebugDraw* debugDrawer)
	{
		// Bullet has this, but it doesn't even call it itself, so...
	}

	void bt3DPersonControllerComponent::SetCollisionBody(nPhysics::iCollisionBody* colBody)
	{
		//TODO: Should move this into a vector of things to track for, and then
		// the physics world will go through all the controllers and add things
		// to the current physics world, just like with the rigid and soft bodies.

		this->_rb = static_cast<sBtRigidbodyComponent*>(colBody);

		//this->_btWorld = bp->GetWorld();

		this->SetGhostBody(this->_rb->GetShape());

		//bp->GetWorld()->addAction(this);

		//btIDebugDraw* tempDebug = this->_btWorld->getDebugDrawer();
		//this->_btDebugRenderer = static_cast<cBtDebugRenderer*>(this->_btWorld->getDebugDrawer());
	}

	void bt3DPersonControllerComponent::SetGhostBody(nPhysics::iShape* ghostBodyShape)
	{
		iBulletShapes* shape = dynamic_cast<iBulletShapes*>(ghostBodyShape);

		if (this->_ghostObject == nullptr)
		{
			this->_ghostObject = new btPairCachingGhostObject();
			this->_ghostObject->setCollisionShape(shape->GetCollisionShape());

			//bp->GetWorld()->addCollisionObject(this->_ghostObject);
			this->_vecGhostBodies.push_back(this->_ghostObject);
			this->_vecPendingGhostBodies.push_back(this->_ghostObject);
		}
		else
		{
			this->_ghostObjectHand = new btPairCachingGhostObject();
			this->_ghostObjectHand->setCollisionShape(shape->GetCollisionShape());

			//bp->GetWorld()->addCollisionObject(this->_ghostObjectHand);
			this->_vecGhostBodies.push_back(this->_ghostObjectHand);
			this->_vecPendingGhostBodies.push_back(this->_ghostObjectHand);
		}
	}

	void bt3DPersonControllerComponent::AddSocketBody(nPhysics::iCollisionBody* socketBody)
	{
		this->_vecSocketBodies.push_back(socketBody);
	}

	void bt3DPersonControllerComponent::SetSocketObjectTransform(int index, const glm::mat4& transform)
	{
		if (index >= this->_vecSocketBodies.size())
		{
			return;
		}

		iCollisionBody* body = this->_vecSocketBodies[index];
		body->SetTransform(transform);
	}

	void bt3DPersonControllerComponent::SetSocketObjectPosition(int index, const glm::vec3& position, const glm::quat& rotation)
	{
		if (index >= this->_vecSocketBodies.size())
		{
			return;
		}

		iCollisionBody* body = this->_vecSocketBodies[index];
		sBtRigidbodyComponent* rb = static_cast<sBtRigidbodyComponent*>(body);
		
		if (this->_flyState._state == sFlyState::YIELD)
		{
			rb->SetPosition(position + rb->GetTransformComponent()->GetUpVector() * rb->GetShapeOffset().y);
			rb->SetRotation(rotation);
		}
		else
		{
			rb->SetPosition(this->_rb->GetPosition() + this->_rb->GetTransformComponent()->GetForwardVector() * 3.0f + this->_rb->GetTransformComponent()->GetUpVector() * 3.0f);
			btQuaternion swordRot = btQuaternion(0.0f, AI_MATH_PI, 0.0f);
			rb->SetRotation(this->_rb->GetRotation() * nConvert::ToSimple(swordRot));
		}
	}

	void bt3DPersonControllerComponent::SetHandPosition(const glm::vec3& position, const glm::quat& rotation)
	{
		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(nConvert::ToBullet(position));
		transform.setRotation(nConvert::ToBullet(rotation));

		this->_ghostObjectHand->setWorldTransform(transform);

		this->SetSocketObjectPosition(0, position, rotation);
	}

	void bt3DPersonControllerComponent::SetGravity(const glm::vec3& gravity)
	{
		this->_moveState.gravity = gravity;
	}

	void bt3DPersonControllerComponent::SetMoveDirection(const glm::vec3& moveDirection)
	{
		// Set move directions to be used next time step
		if (moveDirection != glm::vec3(0.0f))
		{
			this->_moveState.bMoved = true;
			this->_moveState.stickDirection = moveDirection;
			this->_moveState.normStickDirection = glm::normalize(moveDirection);
			this->_moveState.normStickDirection *= this->_moveState.moveSpeed;
		}
	}

	void bt3DPersonControllerComponent::SetJumpDirection(const glm::vec3& jumpDirection)
	{
		// Set jump directions to be used next time step
		if (jumpDirection != glm::vec3(0.0f) && this->_jumpState.state == sJumpState::GROUNDED && this->_flyState._state == sFlyState::YIELD)
		{
			this->_jumpState.bJumped = true;
			this->_jumpState.jumpSpeed = jumpDirection.y;
			this->_jumpState.jumpDirection = jumpDirection;
			this->_jumpState.normJumpDirection = glm::normalize(jumpDirection);
			this->_moveState.verticalVelocity = this->_jumpState.jumpSpeed;
		}
	}

	void bt3DPersonControllerComponent::SetFlyDirection(const glm::vec3& flyDirection)
	{
		// We should only start the flying process if grounded
		if (this->_jumpState.state == sJumpState::GROUNDED && this->_flyState._state == sFlyState::YIELD)
		{
			// Set variables for the flying behaviour
			this->_moveState.bCanMove = false;
			this->_flyState.bFlying = true;
			this->_flyState.curTime = this->_flyState.maxCrouchTime;
			this->_flyState.flyDirection = this->_rb->GetTransformComponent()->GetForwardVector();
			this->_flyState.flyDirection.y = 1.0f;
			this->_flyState.normFlyDirection = glm::normalize(flyDirection);
			this->animationState = this->_crouchName;
		}
	}

	void bt3DPersonControllerComponent::SetAttack()
	{
		if (!this->_attackState.bIsAttacking)
		{
			this->_attackState.bIsAttacking = true;
			this->animationState = this->_attack1Name;
		}
	}

	void bt3DPersonControllerComponent::CollisionDispatcher(std::pair<sBtRaycastHitInfo, sBtRaycastHitInfo> collisionPair)
	{
		//TODO: this
	}

	bool bt3DPersonControllerComponent::GetIsControllable()
	{
		return this->_bIsControllable;
	}

	void bt3DPersonControllerComponent::SetIsControllable(bool isControllable)
	{
		this->_bIsControllable = isControllable;
	}

	void bt3DPersonControllerComponent::DropHeldObject()
	{
		// Check if there's something to begin with
		if (this->_vecSocketBodies.size() <= 0)
		{
			return;
		}

		// Remove object from the hand
		sBtRigidbodyComponent* rb = static_cast<sBtRigidbodyComponent*>(this->_vecSocketBodies[0]);
		this->_vecSocketBodies.erase(this->_vecSocketBodies.begin());

		// Move away sllightly from the body to avoid issues
		glm::vec3 direction = rb->GetPosition() - this->_rb->GetPosition();
		rb->SetPosition(rb->GetPosition() + direction * 2.0f);

		// Turn it into kinematic safely
		//this->_btWorld->removeRigidBody(rb->GetBtRigidbody());

		rb->SetMass(1.0f);
		btVector3 localInertia(0, 0, 0);
		rb->GetBtRigidbody()->setRestitution(rb->GetBounciness());
		rb->GetBtRigidbody()->getCollisionShape()->calculateLocalInertia(rb->GetMass(), localInertia);
		rb->GetBtRigidbody()->setMassProps(rb->GetMass(), localInertia);
		rb->SetUseGravity(true);
		rb->GetBtRigidbody()->setActivationState(DISABLE_DEACTIVATION);
		rb->GetBtRigidbody()->setCollisionFlags(rb->GetBtRigidbody()->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
		rb->_isPickable = true;
		rb->GetBtRigidbody()->setLinearVelocity(nConvert::ToBullet(glm::vec3(0.0f)));
		rb->SetIsKinematic(false);
		rb->GetBtRigidbody()->updateInertiaTensor();

		//this->_btWorld->addRigidBody(rb->GetBtRigidbody());
	}

	std::string bt3DPersonControllerComponent::GetAnimationName()
	{
		return this->animationState;
	}

	// The pre-step is important so we can set the variables that will be used later
	// unless these variabe=les are properly initialized, the process will become unmanagable
	void bt3DPersonControllerComponent::PreStep(double deltaTime)
	{
		// Move the ghost object to the body position for correct collision sweep calculation.
		btTransform transform = this->_rb->GetBtTransform();
		this->_ghostObject->setWorldTransform(transform);

		// Set current and targets for this time step
		this->_moveState.initialPosition = nConvert::ToSimple(transform.getOrigin());
		this->_moveState.curPosition = nConvert::ToSimple(transform.getOrigin());
		this->_moveState.curOrientation = nConvert::ToSimple(transform.getRotation());
		this->_moveState.targetPosition = this->_moveState.curPosition;
		this->_moveState.targetOrientation = this->_moveState.curOrientation;
		this->_moveState.curVerticalOffset = 0.0f;

		float curDistance = fabs(btDistance(nConvert::ToBullet(this->_moveState.initialPosition), nConvert::ToBullet(this->_moveState.lastPosition)));
		if (curDistance > 1.0f)
		{
			this->_moveState.lastPosition = this->_moveState.initialPosition;
		}
	}

	// The actual time step that will manage the controller behaviour
	void bt3DPersonControllerComponent::TimeStep(double deltaTime)
	{
		// Only perform land movement when not in the process of flying
		if (this->_flyState._state == sFlyState::YIELD && !this->_flyState.bFlying) 
		{
			this->LandBehaviour(deltaTime);
		}
		else
		{
			this->FlyBehaviour(deltaTime);
		}
	}

	bool bt3DPersonControllerComponent::GetIsAttacking()
	{
		return this->_attackState.bIsAttacking;
	}

	bool bt3DPersonControllerComponent::GetIsGrouded()
	{
		return this->_jumpState.wasGrounded;
	}

	void bt3DPersonControllerComponent::SetIsGrouded(bool isGrounded)
	{
		
	}

	// Controller behaviour for land based motion
	void bt3DPersonControllerComponent::LandBehaviour(double deltaTime)
	{
		//TODO: Damping here.

		// Quick check for motion. Yes, would be cool to stop mid jump.
		if (this->_attackState.bIsAttacking)
		{
			this->_attackState.currentAttackTime += (float)deltaTime;
			if (this->_attackState.currentAttackTime >= this->_attackState.maxAttackTime)
			{
				this->_attackState.currentAttackTime = 0.0f;
				this->animationState = this->_idleName;
				this->_attackState.bIsAttacking = false;
			}
			return;
		}

		// Check for grounded
		btCollisionWorld::ClosestRayResultCallback rayCallback(nConvert::ToBullet(this->_rb->GetPosition()), nConvert::ToBullet(this->_rb->GetPosition() - this->_rb->GetShapeOffset()) + btVector3(0.0f, -10.0f, 0.0f));
		this->_collisionWorld->rayTest(nConvert::ToBullet(this->_rb->GetPosition()), nConvert::ToBullet(this->_rb->GetPosition() - this->_rb->GetShapeOffset()) + btVector3(0.0f, -10.0f, 0.0f), rayCallback);
		this->_jumpState.wasGrounded = rayCallback.hasHit();

		this->_btDebugRenderer->drawLine(nConvert::ToBullet(this->_rb->GetPosition()), nConvert::ToBullet(this->_rb->GetPosition() - this->_rb->GetShapeOffset()) + btVector3(0.0f, -10.0f, 0.0f), btVector3(0.0f, 0.0f, 1.0f));

		if (rayCallback.hasHit())
		{
			printf("Rayhit!\n");
			float dist = fabs(btDistance(nConvert::ToBullet(this->_rb->GetPosition() - this->_rb->GetShapeOffset()), rayCallback.m_hitPointWorld));
			if (dist < 1.5f)
			{
				this->_rb->SetPosition(nConvert::ToSimple(rayCallback.m_hitPointWorld));
				this->_jumpState.wasGrounded = !this->_jumpState.bJumped;
			}
			else
			{
				this->_jumpState.wasGrounded = false;
			}

			// This could help with slopes, but it can be tricky
			/*btVector3 pos = nConvert::ToBullet(this->_rb->GetPosition());
			pos.setY(rayCallback.m_hitPointWorld.getY() + 11.0f);
			this->_moveState.curPosition = nConvert::ToSimple(pos);*/
		}
		else
		{
			// The raycast may in some cases fail due to math fluctuations,
			// so we should make a second ray just in case to make sure it wasn't a mistake
			btCollisionWorld::ClosestRayResultCallback safeRayCallback(nConvert::ToBullet(this->_rb->GetPosition()), nConvert::ToBullet(this->_rb->GetPosition() - this->_rb->GetShapeOffset()) + btVector3(0.0f, -10.0f, 0.0f));
			this->_collisionWorld->rayTest(nConvert::ToBullet(this->_rb->GetPosition()), nConvert::ToBullet(this->_rb->GetPosition() - this->_rb->GetShapeOffset()) + btVector3(0.0f, -10.0f, 0.0f), safeRayCallback);
			this->_jumpState.wasGrounded = safeRayCallback.hasHit();
			if (!safeRayCallback.hasHit())
			{
				// Apply gravity to fall velocity
				printf("FAILED HIT!\n");
				this->_moveState.verticalVelocity += this->_moveState.gravity.y * 4.0f * (float)deltaTime;
				if (this->_moveState.verticalVelocity > this->_jumpState.jumpSpeed)
				{
					this->_moveState.verticalVelocity = this->_jumpState.jumpSpeed;
				}
				else if (this->_moveState.verticalVelocity < this->_jumpState.fallSpeed)
				{
					this->_moveState.verticalVelocity = this->_jumpState.fallSpeed;
				}
			}
		}

		this->_moveState.curVerticalOffset = this->_moveState.verticalVelocity * 4.0f * (float)deltaTime;

		// Calculate jump
		this->StepUp(deltaTime);

		// Calculate movement
		if (this->_moveState.bMoved && this->_jumpState.state != sJumpState::LAND)
		{
			this->StepForward(deltaTime);
			this->_moveState.bMoved = false;
			this->_moveState.stickDirection = glm::vec3(0.0f);
			this->_moveState.normStickDirection = glm::vec3(0.0f);
		}
		else
		{
			if (this->_jumpState.state == sJumpState::GROUNDED)
			{
				this->animationState = this->_idleName;
			}
		}

		// Calculate landing
		this->StepDown(deltaTime);

		this->StepLand(deltaTime);

		btTransform rbTransform = nConvert::ToBullet(this->_rb->GetTransform());
		rbTransform.setOrigin(nConvert::ToBullet(this->_moveState.curPosition));
		this->_rb->SetPosition(this->_moveState.curPosition);
		this->_ghostObject->setWorldTransform(rbTransform);

		int numPenetrationLoops = 0;
		this->_bTouchingContact = false;
		while (this->RecoverFromPenetration())
		{
			numPenetrationLoops++;
			this->_bTouchingContact = true;
			if (numPenetrationLoops > 4)
			{
				//printf("character could not recover from penetration = %d\n", numPenetrationLoops);
				break;
			}
		}


		// Now check ghost trigger collisions
		this->CheckGhostCollision(1);
	}

	void bt3DPersonControllerComponent::FlyBehaviour(double deltaTime)
	{
		// Check for grounded
		btCollisionWorld::AllHitsRayResultCallback rayCallback(nConvert::ToBullet(this->_rb->GetPosition()), nConvert::ToBullet(this->_rb->GetPosition() + glm::vec3(0.0f, -20.0f, 0.0f)));
		this->_collisionWorld->rayTest(nConvert::ToBullet(this->_rb->GetPosition()), nConvert::ToBullet(this->_rb->GetPosition() + glm::vec3(0.0f, -20.0f, 0.0f)), rayCallback);
		this->_btDebugRenderer->drawLine(nConvert::ToBullet(this->_rb->GetPosition()), nConvert::ToBullet(this->_rb->GetPosition()) + btVector3(0.0f, -20.0f, 0.0f), btVector3(0.0f, 0.0f, 1.0f));
		
		bool hasHit = false;
		int collisionIndex = 0;
		btVector3 landPoint = btVector3(0.0f, 0.0f, 0.0f);
		
		// Check raycast, in this case, since we have rotation, 
		// we might have cases of other attached bodies interfering with
		// the raycast, so we need to check them.
		if (rayCallback.hasHit())
		{
			for (unsigned int i = 0; i < rayCallback.m_collisionObjects.size(); i++)
			{
				bool bail = false;
				if (rayCallback.m_collisionObjects[i] == this->_rb->GetBtRigidbody())
				{
					continue;
				}

				for (unsigned int j = 0; j < this->_vecSocketBodies.size(); j++)
				{
					sBtRigidbodyComponent* bRb = static_cast<sBtRigidbodyComponent*>(this->_vecSocketBodies[j]);
					if (rayCallback.m_collisionObjects[i] == bRb->GetBtRigidbody())
					{
						bail = true;
						break;
					}
				}

				if (bail)
				{
					continue;
				}

				for (unsigned int j = 0; j < this->_vecGhostBodies.size(); j++)
				{
					if (rayCallback.m_collisionObjects[i] == this->_vecGhostBodies[j])
					{
						bail = true;
						break;
					}
				}

				if (bail)
				{
					continue;
				}

				collisionIndex = i;
				hasHit = true;
			}
		}
		
		this->_jumpState.wasGrounded = false;
		if (hasHit)
		{
			float dist = fabs(btDistance(nConvert::ToBullet(this->_rb->GetPosition()), rayCallback.m_hitPointWorld[collisionIndex]));
			printf("Fly hit: %.1f\n", dist);
			if (dist < 15.0f)
			{
				this->_jumpState.wasGrounded = !this->_flyState.bFlying;

				if (this->_jumpState.wasGrounded)
				{
					landPoint = rayCallback.m_hitPointWorld[collisionIndex];
				}
			}
			else
			{
				this->_jumpState.wasGrounded = false;
			}
		}

		this->StepFly(deltaTime);

		this->StepFlyLand(nConvert::ToSimple(landPoint), deltaTime);

		btTransform rbTransform = nConvert::ToBullet(this->_rb->GetTransform());
		rbTransform.setOrigin(nConvert::ToBullet(this->_moveState.curPosition));
		this->_rb->SetPosition(this->_moveState.curPosition);
		this->_ghostObject->setWorldTransform(rbTransform);
	}

	void bt3DPersonControllerComponent::StepUp(double deltaTime)
	{
		this->_jumpState.curJumpPosition = this->_moveState.curPosition;
		this->_jumpState.targetJumpPosition = this->_moveState.curPosition;

		if (this->_jumpState.state == sJumpState::GROUNDED && this->_jumpState.bJumped)
		{
			this->_jumpState.bJumped = false;
			this->_jumpState.state = sJumpState::UP;
		}

		if (this->_jumpState.state == sJumpState::UP)
		{
			if (this->_moveState.verticalVelocity >= 0.0f)
			{
				this->_jumpState.targetJumpPosition += glm::vec3(0.0f, 1.0f, 0.0f) * this->_moveState.curVerticalOffset;
				this->animationState = this->_jumpName;
			}
			else
			{
				this->_jumpState.state = sJumpState::FALL;
				this->animationState = this->_fallName;
			}

			this->_moveState.targetPosition = this->_jumpState.targetJumpPosition;
			this->_moveState.curPosition = this->_jumpState.targetJumpPosition;

			//TODO: collision calculation for the head?

		}

		this->_jumpState.targetJumpPosition.x += this->_jumpState.normJumpDirection.x;
		this->_jumpState.targetJumpPosition.z += this->_jumpState.normJumpDirection.z;

		this->_moveState.curPosition = this->_jumpState.targetJumpPosition;
		this->_moveState.targetPosition = this->_moveState.curPosition;
	}

	void bt3DPersonControllerComponent::StepForward(double deltaTime)
	{
		// Create the next move position and check for collisions
		this->_moveState.targetPosition = this->_moveState.curPosition + this->_moveState.normStickDirection;
		this->_moveState.targetPosition.y = this->_moveState.curPosition.y;

		glm::quat finalRotation = this->_moveState.curOrientation;
		if (this->_jumpState.state == sJumpState::GROUNDED)
		{
			// Calculate interpolated rotation based on stick direction
			glm::quat newRot = glm::quatLookAt(-this->_moveState.normStickDirection, glm::vec3(0.0f, 1.0f, 0.0f));

			float angle = glm::acos(glm::min(1.0f, glm::dot(this->_rb->GetTransformComponent()->GetForwardVector(), this->_moveState.normStickDirection)));
			float angleDeg = glm::degrees(angle);
			if (angleDeg > 1.0f)
			{
				float timeElapsed = 1.0f / angle;

				// Call Slerp
				finalRotation = glm::slerp(this->_moveState.curOrientation, newRot, 0.1f * this->_moveState.rotationSpeed);
			}

			this->_moveState.curOrientation = finalRotation;
			this->_rb->SetRotation(finalRotation);

			this->animationState = this->_walkName;
		}

		// Temp transforms for reuse in the sweep test
		btTransform tempStart;
		btTransform tempEnd;

		// Do sweep test using the ghost object
		btScalar sweepFraction = 1.0f;
		btScalar closestHitFraction = 0.0f;
		btScalar distance2 = (nConvert::ToBullet(this->_moveState.targetPosition - this->_moveState.curPosition)).length2();
		int numSweeps = 10;

		while (sweepFraction > btScalar(0.01f) && numSweeps > 0)
		{
			tempStart.setOrigin(nConvert::ToBullet(this->_moveState.curPosition));
			tempEnd.setOrigin(nConvert::ToBullet(this->_moveState.targetPosition));

			btManifoldArray overlapObjPairs;
			float curDistance = std::numeric_limits<float>::max();
			btBroadphasePairArray arr = this->_ghostObject->getOverlappingPairCache()->getOverlappingPairArray();
			btVector3 normal = btVector3(0.0f, 0.0f, 0.0f);
			int numPairs = arr.size();
			int finalNumContacts = 0;

			for (int i = 0; i < numPairs; i++)
			{
				btBroadphasePair pair = arr[i];
				btBroadphasePair* collidedPair = this->_collisionWorld->getPairCache()->findPair(pair.m_pProxy0, pair.m_pProxy1);

				overlapObjPairs.clear();
				if (collidedPair == nullptr)
				{
					// No collisions found...
					//printf("No collisions...\n");
					return;
				}

				collidedPair->m_algorithm->getAllContactManifolds(overlapObjPairs);

				for (unsigned int j = 0; j < overlapObjPairs.size(); j++)
				{
					btPersistentManifold* curPair = overlapObjPairs[j];

					const btCollisionObject* obj0 = curPair->getBody0();
					const btCollisionObject* obj1 = curPair->getBody1();

					if (this->_vecSocketBodies.size() != 0)
					{
						sBtRigidbodyComponent* body = static_cast<sBtRigidbodyComponent*>(this->_vecSocketBodies[0]);
						if (obj0 == body->GetBtRigidbody() || obj1 == body->GetBtRigidbody())
						{
							continue;
						}
					}

					sBtRigidbodyComponent* rb0 = static_cast<sBtRigidbodyComponent*>(obj0->getCollisionShape()->getUserPointer());
					sBtRigidbodyComponent* rb1 = static_cast<sBtRigidbodyComponent*>(obj1->getCollisionShape()->getUserPointer());
					if (obj0 == this->_rb->GetBtRigidbody() || obj1 == this->_rb->GetBtRigidbody())
					{
						continue;
					}

					if ((obj0 == this->_ghostObject && obj1 == this->_ghostObjectHand) || (obj0 == this->_ghostObjectHand && obj1 == this->_ghostObject))
					{
						continue;
					}

					int numContacts = curPair->getNumContacts();

					for (int c = 0; c < numContacts; c++) // GET IT??? c++???? LOL
					{
						btManifoldPoint p = curPair->getContactPoint(c);

						/*if (rb1->GetShapeType() == eShapeTypes::SHAPE_TYPE_BVH_TRIANGLE_MESH)
						{
							cBtBvhTriangleMeshShape* shape = dynamic_cast<cBtBvhTriangleMeshShape*>(rb1->GetShape());
							glm::vec3 colour = glm::vec3(shape->_modelInfo->pVerticesToVBO[p.m_index1]., shape->_modelInfo->pVerticesToVBO[p.m_index1].g, shape->_modelInfo->pVerticesToVBO[p.m_index1].b);
							this->currentVertexColour = colour;
							printf("Colour: %.1f, %.1f, %.1f\n", colour.r, colour.g, colour.b);
						}*/

						if (p.getDistance() < curDistance /*&& p.m_positionWorldOnB.getY() > this->_moveState.curPosition.y*/)
						{
							curDistance = p.getDistance();
							normal = p.m_normalWorldOnB;
							closestHitFraction = curDistance;
							finalNumContacts = numContacts;
						}
					}
				}
			}

			if (finalNumContacts > 0)
			{
				sweepFraction -= closestHitFraction;

				btVector3 currentDir = nConvert::ToBullet(this->_moveState.targetPosition - this->_moveState.curPosition);
				distance2 = currentDir.length2();
				if (distance2 > SIMD_EPSILON)
				{
					currentDir.normalize();
					/* See Quake2: "If velocity is against original velocity, stop ead to avoid tiny oscilations in sloping corners." */
					if (currentDir.dot(nConvert::ToBullet(this->_moveState.normStickDirection)) <= btScalar(0.0))
					{
						break;
					}

					//this->UpdateTargetPositionBasedOnCollision(normal);
				}
				else
				{
					break;
				}
			}
			//this->_ghostObject->convexSweepTest((this->_rb->GetBtRigidbody()->getCollisionShape()), tempStart, tempEnd, callback, collisionWorld->getDispatchInfo().m_allowedCcdPenetration);

			// This might come in handy in slopes...
			float tempY = this->_moveState.curPosition.y;
			this->_moveState.curPosition = this->_moveState.targetPosition;

			this->_moveState.curPosition.y = tempY;
			if (this->_moveState.curPosition.y < tempY)
			{

			}

			numSweeps--;
		}
	}

	void bt3DPersonControllerComponent::StepFly(double deltaTime)
	{
		if (this->_flyState._state == sFlyState::YIELD && this->_flyState.bFlying)
		{
			this->_flyState.bFlying = false;
			this->_flyState._state = sFlyState::CROUCH;
		}

		if (this->_flyState._state == sFlyState::CROUCH)
		{
			this->_flyState.curTime -= (float)deltaTime;
			if (this->_flyState.curTime <= 0.0f)
			{
				this->_flyState.curTime = 0.0f;
				this->_flyState.waitTime = 2.0f;
				this->_moveState.curPosition += glm::vec3(0.0f, 10.0f, 0.0f);
				
				// Calculate rotation for a diagonal launch
				glm::quat adjustQuat = glm::quat(glm::vec3(glm::radians(-45.0f), glm::radians(180.0f), 0.0f));
				glm::quat newRot = this->_rb->GetRotation() * adjustQuat;

				this->_moveState.curOrientation = newRot;
				this->_rb->SetRotation(newRot);

				this->animationState = this->_flyName;
				this->_flyState.bFlying = false;
				this->_flyState.curTime = 0.0f;
				this->_flyState._state = sFlyState::FLY;
				this->_rb->SetPosition(this->_moveState.curPosition);
			}
		}

		if (this->_flyState._state == sFlyState::FLY)
		{
			if (this->_moveState.bMoved)
			{
				this->_moveState.bMoved = false;

				// Calculate interpolated rotation based on stick direction
				glm::vec3 rad;
				if (this->_flyState.rotValue.x > 0)
				{
					rad = glm::vec3(glm::radians(1.0f), glm::radians(0.0f), glm::radians(0.0f));

					// Step 1: make a quaternion that represents the angle we want to rotate
					glm::quat rotationAdjust(rad);

					// Step 2: Multiply this quaternion by the existing quaternion. This "adds" the angle we want.
					glm::quat resultQuaternion = this->_rb->GetRotation() * rotationAdjust;

					this->_moveState.curOrientation = resultQuaternion;
					this->_rb->SetRotation(resultQuaternion);
				}
				else if (this->_flyState.rotValue.x < 0)
				{
					rad = glm::vec3(glm::radians(-1.0f), glm::radians(0.0f), glm::radians(0.0f));
					
					// Step 1: make a quaternion that represents the angle we want to rotate
					glm::quat rotationAdjust(rad);

					// Step 2: Multiply this quaternion by the existing quaternion. This "adds" the angle we want.
					glm::quat resultQuaternion = this->_rb->GetRotation() * rotationAdjust;

					this->_moveState.curOrientation = resultQuaternion;
					this->_rb->SetRotation(resultQuaternion);
				}

				if (this->_flyState.rotValue.y > 0)
				{
					rad = glm::vec3(glm::radians(0.0f), glm::radians(0.0f), glm::radians(1.0f));

					// Step 1: make a quaternion that represents the angle we want to rotate
					glm::quat rotationAdjust(rad);

					// Step 2: Multiply this quaternion by the existing quaternion. This "adds" the angle we want.
					glm::quat resultQuaternion = this->_rb->GetRotation() * rotationAdjust;

					this->_moveState.curOrientation = resultQuaternion;
					this->_rb->SetRotation(resultQuaternion);
				}
				else if (this->_flyState.rotValue.y < 0)
				{
					rad = glm::vec3(glm::radians(0.0f), glm::radians(0.0f), glm::radians(-1.0f));

					// Step 1: make a quaternion that represents the angle we want to rotate
					glm::quat rotationAdjust(rad);

					// Step 2: Multiply this quaternion by the existing quaternion. This "adds" the angle we want.
					glm::quat resultQuaternion = this->_rb->GetRotation() * rotationAdjust;

					this->_moveState.curOrientation = resultQuaternion;
					this->_rb->SetRotation(resultQuaternion);
				}
			}

			this->_moveState.curPosition += this->_rb->GetTransformComponent()->GetUpVector() * this->_flyState.flySpeed * (float)deltaTime;
			this->_moveState.targetPosition = this->_moveState.curPosition;
		}
	}

	void bt3DPersonControllerComponent::StepDown(double deltaTime)
	{
		if (this->_jumpState.wasGrounded)
		{
			return;
		}

		glm::vec3 tempPosition = this->_moveState.targetPosition;
		glm::quat tempOrientation = this->_moveState.targetOrientation;

		glm::vec3 stepDrop = glm::vec3(0.0f);

		if (this->_moveState.verticalVelocity < 0.0f)
		{
			float downVelocity = this->_moveState.verticalVelocity * (float)deltaTime;
			stepDrop = glm::vec3(0.0f, 1.0f, 0.0f) * downVelocity;
			tempPosition += stepDrop;
		}
		this->_moveState.targetPosition = tempPosition;
		this->_moveState.curPosition = this->_moveState.targetPosition;
	}

	void bt3DPersonControllerComponent::StepLand(double deltaTime)
	{
		if (!this->_jumpState.wasGrounded || this->_jumpState.state == sJumpState::GROUNDED)
		{
			return;
		}

		this->_moveState.verticalVelocity = 0.0f;
		this->_moveState.curVerticalOffset = 0.0f;

		if (this->_jumpState.state == sJumpState::FALL)
		{
			this->_jumpState.state = sJumpState::LAND;
		}

		if (this->_jumpState.state == sJumpState::LAND)
		{
			this->_jumpState.curLandTime -= (float)deltaTime;
			this->animationState = this->_landName;
			if (this->_jumpState.curLandTime <= 0.0f)
			{
				this->_jumpState.curLandTime = this->_jumpState.maxLandTime;
				this->_jumpState.state = sJumpState::GROUNDED;
			}
		}
		else
		{
			this->_jumpState.state = sJumpState::GROUNDED;
		}
	}

	void bt3DPersonControllerComponent::StepFlyLand(const glm::vec3& hitPoint, double deltaTime)
	{
		if (this->_flyState.waitTime > 0.0f)
		{
			this->_flyState.waitTime -= (float)deltaTime;

			if (this->_flyState.waitTime <= 0.0f)
			{
				this->_flyState.waitTime = 0.0f;
			}

			return;
		}

		if (this->_jumpState.wasGrounded)
		{
			if (this->_flyState._state == sFlyState::FLY)
			{
				this->_flyState._state = sFlyState::LANDHARD;
				this->_flyState.curTime = this->_flyState.maxLandTime;

				glm::quat newRot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
				glm::vec3 upVec = ((hitPoint + glm::vec3(0.0f, 1.0f, 0.0f)) - hitPoint);

				this->_moveState.curPosition = hitPoint + this->_rb->GetShapeOffset();
				this->_moveState.curOrientation = newRot;
				this->_rb->SetRotation(newRot);
			}
		}

		if (!this->_flyState._state == sFlyState::LANDHARD)
		{
			if (this->_flyState.bFlying || this->_flyState._state == sFlyState::YIELD)
			{
				return;
			}
		}

		this->_moveState.verticalVelocity = 0.0f;
		this->_moveState.curVerticalOffset = 0.0f;
		if (this->_flyState._state == sFlyState::LANDHARD)
		{
			this->_flyState.curTime -= (float)deltaTime;
			this->animationState = this->_landHardName;

			if (this->_flyState.curTime <= 0.0f)
			{
				this->_flyState.curTime = 0.0f;
				this->_flyState._state = sFlyState::YIELD;

				this->animationState = this->_idleName;
			}
		}
	}

	void bt3DPersonControllerComponent::SetFlyX(float X)
	{
		this->_flyState.rotValue.x = X;
	}

	void bt3DPersonControllerComponent::SetFlyY(float Y)
	{
		this->_flyState.rotValue.y = Y;
	}

	void bt3DPersonControllerComponent::UpdateTargetPositionBasedOnCollision(btVector3 worldNormal, btScalar tangentMag, btScalar normalMag)
	{
		btVector3 movementDirection = nConvert::ToBullet(this->_moveState.targetPosition - this->_moveState.curPosition);
		btScalar movementLength = movementDirection.length();
		if (movementLength > SIMD_EPSILON)
		{
			movementDirection.normalize();
			
			btVector3 reflectDir = this->ComputeReflection(movementDirection, worldNormal);
			reflectDir.normalize();

			btVector3 parallelDir, perpendicularDir;

			parallelDir = this->ComputeParallel(reflectDir, worldNormal);
			perpendicularDir = this->ComputePerpendicular(reflectDir, worldNormal);

			this->_moveState.targetPosition = this->_moveState.curPosition;
			if (tangentMag != 0.0)
			{
				btVector3 parComponent = parallelDir * btScalar(tangentMag * movementLength);
				this->_moveState.targetPosition += nConvert::ToSimple(parComponent) * 10.0f;
			}

			if (normalMag != 0.0)
			{
				btVector3 perpComponent = perpendicularDir * btScalar(normalMag * movementLength);
				this->_moveState.targetPosition += nConvert::ToSimple(perpComponent) * 10.0f;
			}
		}
	}

	bool bt3DPersonControllerComponent::RecoverFromPenetration()
	{
		// Here we must refresh the overlapping paircache as the penetrating movement itself or the
		// previous recovery iteration might have used setWorldTransform and pushed us into an object
		// that is not in the previous cache contents from the last timestep, as will happen if we
		// are pushed into a new AABB overlap. Unhandled this means the next convex sweep gets stuck.
		//
		// Do this by calling the broadphase's setAabb with the moved AABB, this will update the broadphase
		// paircache and the ghostobject's internal paircache at the same time.    /BW

		btVector3 minAabb, maxAabb;
		this->_rb->GetBtRigidbody()->getAabb(minAabb, maxAabb);
		this->_collisionWorld->getBroadphase()->setAabb(this->_ghostObject->getBroadphaseHandle(),
			minAabb, maxAabb, this->_collisionWorld->getDispatcher());

		bool penetration = false;

		this->_collisionWorld->getDispatcher()->dispatchAllCollisionPairs(this->_ghostObject->getOverlappingPairCache(), this->_collisionWorld->getDispatchInfo(), this->_collisionWorld->getDispatcher());

		this->_moveState.curPosition = nConvert::ToSimple(this->_ghostObject->getWorldTransform().getOrigin());

		for (int i = 0; i < this->_ghostObject->getOverlappingPairCache()->getNumOverlappingPairs(); i++)
		{
			btManifoldArray arr;

			btBroadphasePair* collisionPair = &this->_ghostObject->getOverlappingPairCache()->getOverlappingPairArray()[i];

			btCollisionObject* obj0 = static_cast<btCollisionObject*>(collisionPair->m_pProxy0->m_clientObject);
			btCollisionObject* obj1 = static_cast<btCollisionObject*>(collisionPair->m_pProxy1->m_clientObject);

			if (this->_vecSocketBodies.size() != 0)
			{
				sBtRigidbodyComponent* body = static_cast<sBtRigidbodyComponent*>(this->_vecSocketBodies[0]);
				if (obj0 == body->GetBtRigidbody() || obj1 == body->GetBtRigidbody())
				{
					continue;
				}
			}
			
			if (obj0 == this->_rb->GetBtRigidbody() || obj1 == this->_rb->GetBtRigidbody())
			{
				continue;
			}

			if ((obj0 == this->_ghostObject && obj1 == this->_ghostObjectHand) || (obj0 == this->_ghostObjectHand && obj1 == this->_ghostObject))
			{
				continue;
			}

			if ((obj0 && !obj0->hasContactResponse()) || (obj1 && !obj1->hasContactResponse()))
				continue;

			/*if (!needsCollision(obj0, obj1))
				continue;*/

			if (collisionPair->m_algorithm)
				collisionPair->m_algorithm->getAllContactManifolds(arr);
			//printf("Has %d contacts.\n", this->_ghostObject->getOverlappingPairCache()->getNumOverlappingPairs());
			//printf("Has %d many contact points.\n", arr.size());
			for (int j = 0; j < arr.size(); j++)
			{
				
				btPersistentManifold* manifold = arr[j];
				btScalar directionSign = manifold->getBody0() == this->_ghostObject ? btScalar(-1.0) : btScalar(1.0);
				for (int p = 0; p < manifold->getNumContacts(); p++)
				{
					const btManifoldPoint& pt = manifold->getContactPoint(p);
					//printf("index 0: %d\n", pt.m_index1);
					btScalar dist = pt.getDistance();

					if (dist < -0.2f)
					{
						// TODO: cause problems on slopes, not sure if it is needed
						//if (dist < maxPen)
						//{
						//	maxPen = dist;
						//	m_touchingNormal = pt.m_normalWorldOnB * directionSign;//??

						//}

						float preY = this->_moveState.curPosition.y;
						this->_moveState.curPosition += nConvert::ToSimple(pt.m_normalWorldOnB * directionSign * dist * 1.0f);
						if (this->_moveState.curPosition.y < preY)
						{
							this->_moveState.curPosition.y = preY;
						}
						penetration = true;
					}
					else
					{
						//printf("touching %f\n", dist);
					}
				}

				//manifold->clearManifold();
			}
		}

		// Set new positions for both the ghost and the rigidbody
		float finalY = this->_moveState.initialPosition.y + this->_moveState.curVerticalOffset;
		if (this->_moveState.curPosition.y < finalY)
		{
			this->_moveState.curPosition.y = finalY;
		}

		btTransform newTrans = this->_ghostObject->getWorldTransform();
		newTrans.setOrigin(nConvert::ToBullet(this->_moveState.curPosition));
		this->_rb->SetPosition(this->_moveState.curPosition);
		this->_ghostObject->setWorldTransform(newTrans);
		return penetration;
	}

	bool bt3DPersonControllerComponent::CheckGhostCollision(int ghostIndex)
	{
		// Get current ghost body to check
		btPairCachingGhostObject* ghostBody = this->_vecGhostBodies[ghostIndex];


		// Loop through the overlaps
		for (int i = 0; i < ghostBody->getOverlappingPairCache()->getNumOverlappingPairs(); i++)
		{
			
			btBroadphasePair* collisionPair = &ghostBody->getOverlappingPairCache()->getOverlappingPairArray()[i];

			// Get colliding objects
			btCollisionObject* obj0 = static_cast<btCollisionObject*>(collisionPair->m_pProxy0->m_clientObject);
			btCollisionObject* obj1 = static_cast<btCollisionObject*>(collisionPair->m_pProxy1->m_clientObject);

			/*if ((obj0 && !obj0->hasContactResponse()) || (obj1 && !obj1->hasContactResponse()))
			{
				continue;
			}*/
			/*if (!needsCollision(obj0, obj1))
				continue;*/

			// Check object
			sBtRigidbodyComponent* rb = (sBtRigidbodyComponent*)obj0->getCollisionShape()->getUserPointer();
			if (rb == nullptr)
			{
				rb = static_cast<sBtRigidbodyComponent*>(obj1->getCollisionShape()->getUserPointer());
			}

			if (rb != nullptr && rb->_isPickable)
			{
				eShapeTypes shape = rb->GetShapeType();
				if (shape == eShapeTypes::SHAPE_TYPE_BVH_TRIANGLE_MESH)
				{
					continue;
				}

				// Turn it into kinematic safely
				//this->_btWorld->removeRigidBody(rb->GetBtRigidbody());

				rb->SetMass(0.0f);
				btVector3 localInertia(0, 0, 0);
				rb->GetBtRigidbody()->setRestitution(0.0f);
				rb->SetUseGravity(true);
				rb->GetBtRigidbody()->setActivationState(DISABLE_DEACTIVATION);
				rb->GetBtRigidbody()->setCollisionFlags(rb->GetBtRigidbody()->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
				rb->_isPickable = false;
				rb->SetIsKinematic(true);

				//this->_btWorld->addRigidBody(rb->GetBtRigidbody());

				this->AddSocketBody(rb);

				return true;

				btManifoldArray arr;
				for (int j = 0; j < arr.size(); j++)
				{

					btPersistentManifold* manifold = arr[j];
					btScalar directionSign = manifold->getBody0() == this->_ghostObject ? btScalar(-1.0) : btScalar(1.0);
					for (int p = 0; p < manifold->getNumContacts(); p++)
					{
						const btManifoldPoint& pt = manifold->getContactPoint(p);

						
						btScalar dist = pt.getDistance();

						if (dist < -0.2f)
						{
							// TODO: cause problems on slopes, not sure if it is needed
							//if (dist < maxPen)
							//{
							//	maxPen = dist;
							//	m_touchingNormal = pt.m_normalWorldOnB * directionSign;//??

							//}

							float preY = this->_moveState.curPosition.y;
							this->_moveState.curPosition += nConvert::ToSimple(pt.m_normalWorldOnB * directionSign * dist * 0.2f);
							if (this->_moveState.curPosition.y < preY)
							{
								this->_moveState.curPosition.y = preY;
							}
							//penetration = true;
						}
						else
						{
							//printf("touching %f\n", dist);
						}
					}

					//manifold->clearManifold();
				}

				/*glm::mat4 tempTransform = this->_vecSocketBodies[0]->GetTransform();
				this->_vecSocketBodies[0]->SetTransform(rb->GetTransform());
				this->_vecSocketBodies[0] = rb;
				this->_vecSocketBodies[0]->SetTransform(tempTransform);*/
			}
		}
	}

	btVector3 bt3DPersonControllerComponent::ComputeReflection(const btVector3& direction, const btVector3& normal)
	{
		return direction - (btScalar(2.0) * direction.dot(normal)) * normal;
	}

	btVector3 bt3DPersonControllerComponent::ComputeParallel(const btVector3& direction, const btVector3& normal)
	{
		btScalar magnitude = direction.dot(normal);
		return normal * magnitude;
	}

	btVector3 bt3DPersonControllerComponent::ComputePerpendicular(const btVector3& direction, const btVector3& normal)
	{
		return direction - ComputeParallel(direction, normal);
	}

	void bt3DPersonControllerComponent::_SerializeToJson(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& prettyWriter)
	{

	}

	void bt3DPersonControllerComponent::_DeserializeFromJson(rapidjson::Value::ConstValueIterator iter)
	{

	}
}
