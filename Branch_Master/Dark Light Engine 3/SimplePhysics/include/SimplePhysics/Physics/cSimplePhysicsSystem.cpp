// cSimplePhysicsSystem.cpp

#include <Interfaces\Physics\iEntity.h>
#include <Interfaces\Physics\iCollisionBody.h>
#include <SimplePhysics\Physics\sSimpleRigidbodyComponent.h>
#include <SimplePhysics\Physics\sSimpleSoftBodyComponent.h>
#include <SimplePhysics\Physics\cSimplePhysicsSystem.h>
#include <Interfaces\Physics\iPlaneShape.h>
#include <Interfaces\Physics\iSphereShape.h>
#include <SimplePhysics\Physics\cPlaneShape.h>
#include <SimplePhysics\Physics\cSphereShape.h>
#include <SimplePhysics\Physics\nCollide.h>

#include <SimplePhysics\Physics\cSphereShape.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm\gtx\projection.hpp>


namespace nPhysics
{
	cSimplePhysicsSystem* cSimplePhysicsSystem::callbackPointer = nullptr;

	cSimplePhysicsSystem::cSimplePhysicsSystem()
	{
		this->SetGravity(glm::vec3(0.0f, -9.81f, 0.0f));
		this->_controllingEntity = 2;
		this->previousPress = false;

		this->callbackPointer = this;

		return;
	}

	cSimplePhysicsSystem::~cSimplePhysicsSystem()
	{
		return;
	}

	void cSimplePhysicsSystem::InitializeSystem()
	{

	}

	void cSimplePhysicsSystem::SetGravity(const glm::vec3& gravity)
	{
		this->_gravity = gravity;
	}

	void cSimplePhysicsSystem::SetDebugRenderer(iDebugRenderer* pDebugRenderer)
	{
	}

	bool cSimplePhysicsSystem::ScheduleEntity(iEntity* entity, iCollisionBody* body)
	{
		sPhysicsEntity* newEntity = new sPhysicsEntity();
		newEntity->entity = entity;
		newEntity->body = body;

		std::vector<sPhysicsEntity*>::iterator iter;

		if (body->GetBodyType() == BODY_TYPE_RIGIDBODY)
		{
			this->_vecRigidbodies.push_back(newEntity);
		}
		else if (body->GetBodyType() == BODY_TYPE_SOFTBODY)
		{
			this->_vecSoftbodies.push_back(newEntity);
		}

		return true;
	}

	bool cSimplePhysicsSystem::UnscheduleEntity(iEntity* entityToSchedule, iCollisionBody* rigidbodyComp)
	{
		return true;
	}

	void cSimplePhysicsSystem::UpdateSystem(double deltaTime)
	{
		// Check delta time
		const double largestDeltaTime = 0.060; // 60 FPS

		if (deltaTime > largestDeltaTime)
		{
			deltaTime = largestDeltaTime;
		}

		//this->_ControlWind(pIOManager);

		// Integrate
		// Rigidbodies
		for (unsigned int i = 0; i < this->_vecRigidbodies.size(); i++)
		{
			sPhysicsEntity* entityA = this->_vecRigidbodies[i];
			sSimpleRigidbodyComponent* rigidbodyA = static_cast<sSimpleRigidbodyComponent*>(entityA->body);

			// Go to next if kinematic
			if (rigidbodyA->GetIsKinematic() || !entityA->entity->GetIsActive())
			{
				rigidbodyA->StopRigidbody();
				continue;
			}

			// First check for the addition of extra forces
			if (rigidbodyA->GetUseGravity())
			{
				rigidbodyA->SetAcceleration(rigidbodyA->GetAcceleration() + this->_gravity);
			}

			// Clamp velocity and add drag
			this->ClampAccelAndVel(rigidbodyA);
			this->CalculateDrag(rigidbodyA, rigidbodyA->GetDrag());
			entityA->entity->GetTransform()->GetLastPosition() = entityA->entity->GetTransform()->GetWorldPosition();

			// Integrate
			sState state;
			state.x = entityA->entity->GetTransform()->GetWorldPosition();
			state.v = rigidbodyA->GetVelocity();
			this->RK4Integration(entityA, state, rigidbodyA->GetAcceleration(), deltaTime);

			glm::vec3 finalVel = rigidbodyA->GetVelocity();
			finalVel *= glm::pow(0.95f, deltaTime);
			rigidbodyA->SetVelocity(finalVel);
		}

		// Softbodies
		for (unsigned int i = 0; i < this->_vecSoftbodies.size(); i++)
		{
			sPhysicsEntity* entityA = this->_vecSoftbodies[i];
			sSimpleSoftBodyComponent* softbodyA = static_cast<sSimpleSoftBodyComponent*>(entityA->body);

			// Go to next if kinematic
			if (!entityA->entity->GetIsActive())
			{
				//softbodyA->Reset();
				continue;
			}

			// Integrate
			softbodyA->UpdateInternal(deltaTime, this->_gravity);
		}

		// Collision Detection - Rigidbodies
		std::vector<std::pair<sPhysicsEntity*, sPhysicsEntity*>> vecCollisions;
		for (unsigned int i = 0; i < this->_vecRigidbodies.size() - 1; i++)
		{
			sPhysicsEntity* entityA = this->_vecRigidbodies[i];
			glm::vec3 moveVectorA = entityA->entity->GetTransform()->GetWorldPosition() - entityA->entity->GetTransform()->GetLastPosition();
			for (unsigned int j = i + 1; j < this->_vecRigidbodies.size(); j++)
			{
				sPhysicsEntity* entityB = this->_vecRigidbodies[j];

				// Create collision results
				sCollisionStats collisionStatA;
				sCollisionStats collisionStatB;

				glm::vec3 moveVectorB = entityB->entity->GetTransform()->GetWorldPosition() - entityB->entity->GetTransform()->GetLastPosition();

				// Check collisions
				int hasCollided = this->_CollisionStatus(entityA, moveVectorA, entityB, moveVectorB, collisionStatA, collisionStatB);
				if (hasCollided != 0)
				{
					entityA->other = entityB->body;
					entityB->other = entityA->body;
					vecCollisions.push_back(std::make_pair(entityA, entityB));
				}
			}
		}

		// Collision Response
		for (unsigned int i = 0; i != vecCollisions.size(); i++)
		{
			this->SimpleCollisionReaction(vecCollisions[i].first, deltaTime);
			this->SimpleCollisionReaction(vecCollisions[i].second, deltaTime);
		}

		// Reset accelerations
		for (unsigned int i = 0; i != this->_vecRigidbodies.size(); i++)
		{
			sSimpleRigidbodyComponent* rigidbodyA = static_cast<sSimpleRigidbodyComponent*>(this->_vecRigidbodies[i]->body);
			rigidbodyA->GetAcceleration() = glm::vec3(0.0f);
		}

		// Collision Detection - Rigidbodies/Softbodies
		for (unsigned int indexSoft = 0; indexSoft < this->_vecSoftbodies.size(); indexSoft++)
		{
			iSoftbody* softbody = static_cast<iSoftbody*>(this->_vecSoftbodies[indexSoft]->body);
			for (unsigned int indexRigid = 0; indexRigid < this->_vecRigidbodies.size(); indexRigid++)
			{
				iRigidbody* rigidbody = static_cast<iRigidbody*>(this->_vecRigidbodies[indexRigid]->body);
				this->_CollideRigidSoftBody(rigidbody, softbody);
			}
		}
	}

	void cSimplePhysicsSystem::ClampAccelAndVel(sSimpleRigidbodyComponent* rb)
	{
		// Clamp velocity
		glm::vec3 vel = rb->GetVelocity();
		glm::vec2 minMaxVelocity = rb->GetMinMaxVelocity();
		if (nPhyMath::SquareMagnitude(vel) >= (minMaxVelocity.y * minMaxVelocity.y))
		{
			nPhyMath::NormalizeVector(vel);
			vel *= minMaxVelocity.y;
			rb->SetVelocity(vel);
		}

		// Clamp angular velocity
		/*if (nPhyMath::SquareMagnitude(rigidbodyC->angularVelocity) >= (rigidbodyC->minMaxAngularVelocity.y * rigidbodyC->minMaxAngularVelocity.y))
		{
			nPhyMath::NormalizeVector(rigidbodyC->angularVelocity);
			rigidbodyC->velocity *= rigidbodyC->minMaxAngularVelocity.y;
		}*/
	}

	glm::vec3 cSimplePhysicsSystem::CalculateDrag(sSimpleRigidbodyComponent* rb, float rbDrag)
	{
		// Drag = (own coefficient) * (Area) * ((density) * 0.5 * (velocity squared))
		glm::vec3 vel = rb->GetVelocity();
		float dragAmount = (vel.length() * vel.length())  /*velocity squared*/
				* 0.5f * 1.0f /*half density*/
				* 1.0f /*area*/
				* 0.01f /*coeficient*/
				* rbDrag; /*user alterations*/

		// Apply drag to inverse of velocity
		glm::vec3 dragVec = vel;
		nPhyMath::NormalizeVector(dragVec);
		dragVec *= dragAmount;

		// Save signs to avoid reversion
		glm::vec3 vecSigns = glm::vec3(vel.x > 0.0f, vel.y > 0.0f, vel.z > 0.0f);
		vel -= dragVec;

		// If the drag ever inverted the sign, it should have stopped at 0
		if (vel.x <= 0.0f && vecSigns.x == true || vel.x >= 0.0f && vecSigns.x == false) { vel.x = 0.0f; }
		if (vel.y <= 0.0f && vecSigns.y == true || vel.y >= 0.0f && vecSigns.y == false) { vel.y = 0.0f; }
		if (vel.z <= 0.0f && vecSigns.z == true || vel.z >= 0.0f && vecSigns.z == false) { vel.z = 0.0f; }

		rb->SetVelocity(vel);

		return dragVec;
	}

	int cSimplePhysicsSystem::_CollisionStatus(sPhysicsEntity* entityA, glm::vec3 moveVectorA, sPhysicsEntity* entityB, glm::vec3 moveVectorB, sCollisionStats& collisionStatA, sCollisionStats& collisionStatB)
	{
		// Check if they are the same entity
		if (entityA == entityB)
		{
			return 0;
		}

		int hasCollided = 0;
		float contactTime = 0.0f;
		glm::vec3 contactPoint = glm::vec3(0.0f);

		sSimpleRigidbodyComponent* rigidbodyA = static_cast<sSimpleRigidbodyComponent*>(entityA->body);
		sSimpleRigidbodyComponent* rigidbodyB = static_cast<sSimpleRigidbodyComponent*>(entityB->body);
		if (rigidbodyA->GetShape()->GetShapeType() == eShapeTypes::SHAPE_TYPE_SPHERE && rigidbodyB->GetShape()->GetShapeType() == eShapeTypes::SHAPE_TYPE_PLANE)
		{
			// In case A is a sphere and B is a plane
			hasCollided = this->_CheckPlaneSphereCollision(rigidbodyA, moveVectorA, entityB->body, contactTime, contactPoint);

			glm::vec3 updatedNormal;
			nPhysics::iPlaneShape* iPlane = dynamic_cast<nPhysics::iPlaneShape*>(rigidbodyB->GetShape());
			nPhyMath::GetRelativeAxis(iPlane->GetNormal(), entityB->entity->GetTransform()->GetWorldRotation(), updatedNormal);

			nPhysics::iSphereShape* iSphere = dynamic_cast<iSphereShape*>(rigidbodyA->GetShape());

			if (hasCollided == 0) // No collisions
			{
				return hasCollided;
			}
			else
			{
   				float updatedConst = glm::dot(entityB->entity->GetTransform()->GetWorldPosition(), updatedNormal);
				glm::vec3 closestPoint = _ClosestPointPlane(entityB->entity->GetTransform()->GetWorldPosition(), updatedNormal, updatedConst);
				glm::vec3 finalPoint = closestPoint + updatedNormal * iSphere->GetRadius();

				collisionStatA.collisionState = hasCollided;
				collisionStatA.contactPoint = finalPoint;
				collisionStatA.contactTime = contactTime;
				collisionStatA.constant = updatedConst;
				collisionStatB = collisionStatA;

				collisionStatA.entity = entityA->entity;
				collisionStatA.other = entityB->entity;
				collisionStatA.moveVector = moveVectorA;
				collisionStatA.reflectVector = updatedNormal;
				collisionStatA.resultMass = 1.0f / rigidbodyA->GetMass();
				collisionStatB.entity = entityB->entity;
				collisionStatB.other = entityA->entity;
				collisionStatB.moveVector = moveVectorB;
			}
		}
		else if (rigidbodyA->GetShape()->GetShapeType() == eShapeTypes::SHAPE_TYPE_SPHERE && rigidbodyB->GetShape()->GetShapeType() == eShapeTypes::SHAPE_TYPE_SPHERE)
		{
			// In case A is a sphere and B is also a sphere

			hasCollided = this->_CheckSphereSphereCollision(entityB, entityA, moveVectorB, moveVectorA, contactTime, contactPoint);
			
			

			glm::vec3 reflectA = entityA->entity->GetTransform()->GetWorldPosition() - entityB->entity->GetTransform()->GetWorldPosition();
			glm::vec3 reflectB = entityB->entity->GetTransform()->GetWorldPosition() - entityA->entity->GetTransform()->GetWorldPosition();
			reflectA = glm::normalize(reflectA);
			reflectB = glm::normalize(reflectB);


			if (hasCollided == 0) // No collisions
			{
				return hasCollided;
			}
			else
			{
				iSphereShape* iSphereA = dynamic_cast<iSphereShape*>(rigidbodyA->GetShape());
				iSphereShape* iSphereB = dynamic_cast<iSphereShape*>(rigidbodyB->GetShape());

				glm::vec3 normMoveVecA = moveVectorA;
				nPhyMath::NormalizeVector(normMoveVecA);
				glm::vec3 normMoveVecB = moveVectorB;
				nPhyMath::NormalizeVector(normMoveVecB);
				
				glm::vec3 closestPointA = entityB->entity->GetTransform()->GetWorldPosition() + (reflectA * (iSphereA->GetRadius() + iSphereB->GetRadius() + 2.0f));
				glm::vec3 closestPointB = entityB->entity->GetTransform()->GetWorldPosition();
				//glm::vec3 finalPointA = closestPointA + reflectA * iSphereA->GetRadius();

				collisionStatA.collisionState = hasCollided;
				collisionStatA.contactTime = contactTime;
				collisionStatB = collisionStatA;

				collisionStatA.entity = entityA->entity;
				collisionStatA.other = entityB->entity;
				collisionStatA.moveVector = moveVectorA;
				collisionStatA.contactPoint = closestPointA;
				collisionStatA.reflectVector = reflectA;
				collisionStatA.resultMass = 1.0f / rigidbodyA->GetMass();
				collisionStatB.entity = entityB->entity;
				collisionStatB.other = entityA->entity;
				collisionStatB.moveVector = moveVectorB;
				collisionStatB.reflectVector = reflectB;
				collisionStatB.resultMass = 1.0f / rigidbodyB->GetMass();
				collisionStatB.contactPoint = closestPointB;
			}
		}

		return hasCollided;
	}

	int cSimplePhysicsSystem::_CheckPlaneSphereCollision(iCollisionBody* sphere, glm::vec3 moveVector, iCollisionBody* plane, float& time, glm::vec3& collisionPoint)
	{
		nPhysics::sSimpleRigidbodyComponent* rbSphere = static_cast<nPhysics::sSimpleRigidbodyComponent*>(sphere);
		nPhysics::sSimpleRigidbodyComponent* rbPlane = static_cast<nPhysics::sSimpleRigidbodyComponent*>(plane);

		nPhysics::iPlaneShape* iPlane = dynamic_cast<nPhysics::iPlaneShape*>(rbPlane->GetShape());
		nPhysics::iSphereShape* iSphere = dynamic_cast<nPhysics::iSphereShape*>(rbSphere->GetShape());

		//HACK: doing this here, since I have no idea how the future engine will connect
		glm::vec3 positionNormal = iPlane->GetNormal();
		glm::vec3 updatedNormal;
		nPhyMath::GetRelativeAxis(positionNormal, rbPlane->GetTransformComponent()->GetWorldRotation(), updatedNormal);

		//HACK: here too
		// We can get the plane constant by dividing the
		// plane equation x + 2y + 3z + 6 = 0 by the lenght of the normal
		float updatedConst = glm::dot(rbPlane->GetTransformComponent()->GetWorldPosition(), updatedNormal);

		glm::vec3 planeNormal = updatedNormal;
		float sphereRadius = iSphere->GetRadius();

		// Compute distance of sphere center to plane
		float distance = glm::dot(planeNormal, rbSphere->GetTransformComponent()->GetWorldPosition()) - updatedConst;
		if (abs(distance) <= sphereRadius)
		{
			// The sphere is already overlapping the plane.
			// Set time of intersection to zero and the return point to the sphere center.
			time = 0.0f;
			collisionPoint = rbSphere->GetTransformComponent()->GetWorldPosition();
			return -1;
		}
		else
		{
			float denom = glm::dot(planeNormal, moveVector);
			if ((denom * distance) >= 0.0f)
			{
				// no intersection as the sphere is moving parallel to or away from the plane
				return 0;
			}
			else
			{
				// Sphere is moving towards the plane

				// use +r in computations if sphere in front of plane, else -r
				float r = distance > 0.0f ? sphereRadius : -sphereRadius;
				time = (r - distance) / denom;
				collisionPoint = rbSphere->GetTransformComponent()->GetWorldPosition() + (time * moveVector) - (r * planeNormal);
				
				if (time <= 1.0f && time > 0.0f)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
		}
	}

	int cSimplePhysicsSystem::_CheckSphereSphereCollision(sPhysicsEntity* sphere0, sPhysicsEntity* sphere1, glm::vec3 moveVector0, glm::vec3 moveVector1, float& contactTime, glm::vec3& collisionPoint)
	{
		sSimpleRigidbodyComponent* rigidbodyA = static_cast<sSimpleRigidbodyComponent*>(sphere0->body);
		sSimpleRigidbodyComponent* rigidbodyB = static_cast<sSimpleRigidbodyComponent*>(sphere1->body);

		iSphereShape* sphereShape0 = dynamic_cast<iSphereShape*>(rigidbodyA->GetShape());
		iSphereShape* sphereShape1 = dynamic_cast<iSphereShape*>(rigidbodyB->GetShape());

		// Get vector between the centers and the average movement vector
		glm::vec3 vecCenters = sphere1->entity->GetTransform()->GetWorldPosition() - sphere0->entity->GetTransform()->GetWorldPosition();
		glm::vec3 vecMovement = moveVector1 - moveVector0;

		// Get total radius of the spheres
		float totalRadius = sphereShape0->GetRadius() + sphereShape1->GetRadius();

		// Spheres are already colliding if the center distance is lower than the radii
		float distanceCR = glm::dot(vecCenters, vecCenters) - (totalRadius * totalRadius);
		if (distanceCR < 0.0f)
		{
			contactTime = 0.0f;
			return -1;
		}

		float dotMove = glm::dot(vecMovement, vecMovement);
		if (dotMove < DBL_EPSILON)  // Sphere's are not moving relative to each other
		{
			return 0;
		}

		float dotMoveCenter = glm::dot(vecMovement, vecCenters);
		if (dotMoveCenter >= 0.0f) // Sphere's are not moving towards each other
		{
			return 0;
		}

		float numRoots = (dotMoveCenter * dotMoveCenter) - (dotMove * distanceCR);
		if (numRoots < 0.0f) // No real valued root, spheres will not intersect
		{
			return 0;
		}

		contactTime = (-dotMoveCenter - glm::sqrt(numRoots)) / dotMove;
		//collisionPoint = sphere0->locationC->position + (time * moveVector) - (r * planeNormal);

		if (contactTime <= 1.0f && contactTime >= 0.0f)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	int cSimplePhysicsSystem::_CheckSphereSphereCollision(iCollisionBody* bodyA, iSphereShape* sphere0, iCollisionBody* bodyB, iSphereShape* sphere1, unsigned int indexB, unsigned int indexA)
	{
		sSimpleRigidbodyComponent* rigidbodyA = static_cast<sSimpleRigidbodyComponent*>(bodyA);
		sSimpleSoftBodyComponent* softbodyB = static_cast<sSimpleSoftBodyComponent*>(bodyB);
		sSimpleSoftBodyComponent::cNode* node = softbodyB->_vecNodes[indexB];

		glm::vec3 moveVectorA = rigidbodyA->GetTransformComponent()->GetWorldPosition() - rigidbodyA->GetTransformComponent()->GetLastPosition();
		glm::vec3 moveVectorB = node->position - node->lastPosition;

		float contactTime = 0.0f;
		int result = nCollide::CollideMovingSphereSphere(rigidbodyA->GetTransformComponent()->GetLastPosition(), moveVectorA, sphere0->GetRadius(), node->lastPosition, moveVectorB, node->radius, contactTime);

		return result;
	}

	void cSimplePhysicsSystem::_CollideRigidSoftBody(iRigidbody* rigidbody, iSoftbody* softbody)
	{
		sSimpleRigidbodyComponent* rigidbodyC = static_cast<sSimpleRigidbodyComponent*>(rigidbody);
		sSimpleSoftBodyComponent* softbodyC = static_cast<sSimpleSoftBodyComponent*>(softbody);

		// AABB broad phase check
		glm::vec3 rbMin, rbMax;
		glm::vec3 sbMin, sbMax;

		rigidbodyC->GetAABB(rbMin, rbMax);
		softbodyC->GetAABB(sbMin, sbMax);

		if (!(rbMin.x > sbMin.x || rbMin.y > sbMin.y || rbMin.z > sbMin.z
			|| rbMax.x < sbMax.x || rbMax.y < sbMax.y || rbMax.z < sbMax.z))
		{
			return;
		}

		for (unsigned int i = 0; i < softbodyC->_vecNodes.size(); i++)
		{
			// Check node vs rigidbody
			if (rigidbodyC->GetShape()->GetShapeType() == SHAPE_TYPE_SPHERE)
			{
				sSimpleSoftBodyComponent::cNode* node = softbodyC->_vecNodes[i];
				cSphereShape* sphere0 = dynamic_cast<cSphereShape*>(rigidbodyC->GetShape());
				cSphereShape* sphere1 = dynamic_cast<cSphereShape*>(node->sphere);

				// Sphere sphere collision
				int result = this->_CheckSphereSphereCollision(rigidbody, sphere0, softbody, sphere1, i);

				if (result != 0)
				{
					// Get separation between bodies
					glm::vec3 separation = glm::normalize(node->position - rigidbodyC->GetTransformComponent()->GetWorldPosition());
					glm::vec3 fullSeparation = separation * (sphere0->GetRadius() + sphere1->GetRadius());
					glm::vec3 moveToPoint = rigidbodyC->GetTransformComponent()->GetWorldPosition() + fullSeparation;
					
					node->position = moveToPoint;

					// Make projection of velocity into separation and alter velocity
					// Plane projection: B = A - (dot(A, n))*n
					glm::vec3 velProjection = node->velocity - (glm::dot(node->velocity, separation)) * separation;
					node->velocity -= velProjection * node->mass;
				}
			}
		}
	}

	void cSimplePhysicsSystem::SimpleCollisionReaction(sPhysicsEntity* physicsEntity, double deltaTime)
	{
		// No collision, bail
		sCollisionStats collisionStat = physicsEntity->collisionInfo;
		if (collisionStat.collisionState == 0)
		{
			return;
		}
		
		this->OnCollisionEnter(collisionStat);
		sSimpleRigidbodyComponent* rigidbodyA = static_cast<sSimpleRigidbodyComponent*>(physicsEntity->body);
		sSimpleRigidbodyComponent* rigidbodyB = static_cast<sSimpleRigidbodyComponent*>(physicsEntity->other);
		
		// Don't calculate collision response for kinematic objects
		if (rigidbodyA->GetIsKinematic())
		{
			return;
		}

		if (collisionStat.collisionState == -1) // Was already colliding
		{
			//printf("Already colliding\n");
			// Readjust to previous position, adjusting back in
			// the move vector by the collision time percentage.
			//collisionStat.entity->rigidbodyC->velocity = glm::reflect(collisionStat.entity->rigidbodyC->velocity, collisionStat.reflectVector);
			if (rigidbodyB->GetShape()->GetShapeType() == SHAPE_TYPE_SPHERE)
			{
				iSphereShape* sShape = dynamic_cast<iSphereShape*>(rigidbodyA->GetShape());
				//glm::vec3 closestPoint = _ClosestPointPlane(collisionStat.entity->locationC->position, collisionStat.reflectVector, collisionStat.constant);
				glm::vec3 backPosition = collisionStat.contactPoint + collisionStat.reflectVector * sShape->GetRadius();
				collisionStat.entity->GetTransform()->GetWorldPosition() = backPosition;
			}
			else
			{
				iSphereShape* sShape = dynamic_cast<iSphereShape*>(rigidbodyA->GetShape());
				glm::vec3 closestPoint = _ClosestPointPlane(collisionStat.entity->GetTransform()->GetWorldPosition(), collisionStat.reflectVector, collisionStat.constant);
				collisionStat.entity->GetTransform()->GetWorldPosition() = closestPoint + collisionStat.reflectVector * sShape->GetRadius();
			}
			
			return;
		}
		else if (collisionStat.collisionState == 1) // New collision happened
		{
			//printf("Collided\n");
			// Find the reflection vector from the move vector and the normal
			rigidbodyA->SetVelocity(glm::reflect(rigidbodyA->GetVelocity(), collisionStat.reflectVector));

			// Plane projection: B = A - (dot(A, n))*n
			glm::vec3 planeProjec = rigidbodyA->GetVelocity() - (glm::dot(rigidbodyA->GetVelocity(), collisionStat.reflectVector)) * collisionStat.reflectVector;
			rigidbodyA->SetVelocity(rigidbodyA->GetVelocity() - planeProjec * 0.2f * collisionStat.resultMass);
			collisionStat.entity->GetTransform()->GetWorldPosition() = (collisionStat.entity->GetTransform()->GetLastPosition() + (collisionStat.moveVector * collisionStat.contactTime));
		}

		// Integrate
		sState state;
		state.x = collisionStat.entity->GetTransform()->GetWorldPosition();
		state.v = rigidbodyA->GetVelocity();
	}

	void cSimplePhysicsSystem::EulerCromerIntegration(sPhysicsEntity* entity, double deltaTime)
	{
		sSimpleRigidbodyComponent* rigidbodyA = static_cast<sSimpleRigidbodyComponent*>(entity->body);

		// Update velocity from acceleration
		glm::vec3 vel = rigidbodyA->GetVelocity() * (float)deltaTime;
		rigidbodyA->SetVelocity(vel);

		// Clamp velocity and add drag
		//this->ClampAccelAndVel(rigidbodyC->acceleration, rigidbodyC->velocity, rigidbodyC->minMaxVelocity);
		//this->CalculateDrag(rigidbodyC->velocity, rigidbodyC->drag);

		// Update position from velocity
		entity->entity->GetTransform()->GetWorldPosition() += rigidbodyA->GetVelocity() * (float)deltaTime;
	}

	void cSimplePhysicsSystem::RK4Integration(sPhysicsEntity* entity, const sState& state, glm::vec3 acceleration, double deltaTime)
	{
		sSimpleRigidbodyComponent* rigidbodyA = nullptr;
		sSimpleSoftBodyComponent* softbody = nullptr;
		if (entity->body->GetBodyType() == BODY_TYPE_RIGIDBODY)
		{
			rigidbodyA = static_cast<sSimpleRigidbodyComponent*>(entity->body);
		}
		else if (entity->body->GetBodyType() == BODY_TYPE_SOFTBODY)
		{
			softbody = static_cast<sSimpleSoftBodyComponent*>(entity->body);
		}
		

		glm::vec3 dxdt = glm::vec3(0.0f);
		glm::vec3 dvdt = glm::vec3(0.0f);
		
		sDerivatives a, b, c, d, x;

		a = this->RK4EvaluateDerivative(state, acceleration, 0.0f, x);
		b = this->RK4EvaluateDerivative(state, acceleration, deltaTime * 0.5f, a);
		c = this->RK4EvaluateDerivative(state, acceleration, deltaTime * 0.5f, b);
		d = this->RK4EvaluateDerivative(state, acceleration, deltaTime, c);

		dxdt = (1.0f / 6.0f) * (a.dx + 2.0f * (b.dx + c.dx) + d.dx);
		dvdt = (1.0f / 6.0f) * (a.dv + 2.0f * (b.dv + c.dv) + d.dv);
		
		entity->entity->GetTransform()->GetWorldPosition() += dxdt * (float)deltaTime;

		if (rigidbodyA != nullptr)
		{
			rigidbodyA->SetVelocity(rigidbodyA->GetVelocity() + dvdt * (float)deltaTime);
		}
	}

	cSimplePhysicsSystem::sDerivatives cSimplePhysicsSystem::RK4EvaluateDerivative(const sState& initial, glm::vec3 acceleration, double deltaTime, sDerivatives& derivative)
	{
		// Calculate new variables based on future time
		sState state;
		state.x = initial.x + derivative.dx * (float)deltaTime;
		state.v = initial.v + derivative.dv * (float)deltaTime;

		sDerivatives output;
		output.dx = state.v;
		output.dv = acceleration;

		return output;
	}

	glm::vec3 cSimplePhysicsSystem::_ClosestPointPlane(const glm::vec3& point, const glm::vec3& n, float d)
	{
		float t = glm::dot(n, point) - d;
		return point - t * n;
	}

	void cSimplePhysicsSystem::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		
	}

	//void cSimplePhysicsSystem::_ControlWind(cIOManager* pIOManager)
	//{
	//	for (unsigned int i = 0; i < this->_vecSoftbodies.size(); i++)
	//	{
	//		sSimpleSoftBodyComponent* softbody = static_cast<sSimpleSoftBodyComponent*>(this->_vecSoftbodies[i]->body);

	//		// Wind Force
	//		if (pIOManager->key9Down)
	//		{
	//			softbody->_windForce = glm::min(softbody->_windForce + 0.1f, 3.0f);
	//		}
	//		else if (pIOManager->key7Down)
	//		{
	//			softbody->_windForce = glm::max(softbody->_windForce - 0.1f, 0.0f);
	//		}

	//		// Wind Pitch
	//		if (pIOManager->key8Down)
	//		{
	//			softbody->_windPitch = glm::min(softbody->_windPitch + 0.1f, 1.0f);
	//		}
	//		else if (pIOManager->key2Down)
	//		{
	//			softbody->_windPitch = glm::max(softbody->_windPitch - 0.1f, -1.0f);
	//		}

	//		// Wind Yaw
	//		if (pIOManager->key4Down)
	//		{
	//			softbody->_windYaw = glm::min(softbody->_windYaw + 0.1f, 1.0f);
	//		}
	//		else if (pIOManager->key6Down)
	//		{
	//			softbody->_windYaw = glm::max(softbody->_windYaw - 0.1f, -1.0f);
	//		}
	//	}
	//}

	// This is more for organization purposes...
	bool cSimplePhysicsSystem::OnCollisionEnter(const sCollisionStats& info)
	{
   		//info.entity->OnCollisionEnter(info);

		return true;
	}
}

