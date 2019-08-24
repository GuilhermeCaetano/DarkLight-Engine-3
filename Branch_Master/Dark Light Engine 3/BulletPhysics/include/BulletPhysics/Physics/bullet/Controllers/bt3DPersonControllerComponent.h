#ifndef _bt3DPersonControllerComponent_HG_
#define _bt3DPersonControllerComponent_HG_

#include <Interfaces\Controllers\i3DPersonController.h>
#include <BulletDynamics\Dynamics\btActionInterface.h>
#include <sBtRigidbodyComponent.h>
#include <Interfaces\Physics\iPhysicsSystem.h>
#include <sBtRaycastHitInfo.h>
#include <cBtDebugRenderer.h>


class btPairCachingGhostObject;
namespace nPhysics
{
	class bt3DPersonControllerComponent : public i3DPersonController, public btActionInterface
	{
	public:
		bt3DPersonControllerComponent(unsigned long long entityID);
		virtual ~bt3DPersonControllerComponent();

		// Start method for the component
		virtual void Start();

		// This must be overriden on the derived components, otherwise it will return an error!
		static std::string GetStaticType() { return "3DPERSONCONTROLLER_COMPONENT"; };

		virtual void updateAction(btCollisionWorld* collisionWorld, btScalar deltaTimeStep);

		virtual void debugDraw(btIDebugDraw* debugDrawer);

		// Manage controller jump state
		//virtual std::string CheckForGrounded(double deltaTime);
		//virtual sJumpState GetJumpState();
		//virtual std::string Jump();

		virtual void SetCollisionBody(nPhysics::iCollisionBody* colBody);
		virtual void SetGhostBody(nPhysics::iShape* ghostBodyShape);

		virtual void AddSocketBody(nPhysics::iCollisionBody* socketBody);
		virtual void SetSocketObjectTransform(int index, const glm::mat4& transform);
		virtual void SetSocketObjectPosition(int index, const glm::vec3& position, const glm::quat& rotation);
		virtual void SetHandPosition(const glm::vec3& position, const glm::quat& rotation);

		virtual void SetGravity(const glm::vec3& gravity);
		virtual void SetMoveDirection(const glm::vec3& moveDirection);
		virtual void SetJumpDirection(const glm::vec3& moveDirection);
		virtual void SetFlyDirection(const glm::vec3& flyDirection);
		virtual void SetAttack();

		void CollisionDispatcher(std::pair<sBtRaycastHitInfo, sBtRaycastHitInfo> collisionPair);

		// Getters ans setters
		virtual bool GetIsControllable();
		virtual  void SetIsControllable(bool isControllable);

		virtual void DropHeldObject();

		virtual std::string GetAnimationName();

		//virtual float GetMaxMoveSpeed();
		//virtual void SetMaxMoveSpeed(float maxSpeed);
		//virtual float GetMaxAngularSpeed();
		//virtual void SetMaxAngularSpeed(float maxSpeed);

		//virtual bool GetIsRunning();
		//virtual void SetIsRunning(bool isRunning);

		virtual bool GetIsAttacking();

		//virtual bool GetIsStrafing();
		//virtual void SetIsStrafing(bool isStrafing);

		virtual bool GetIsGrouded();
		virtual void SetIsGrouded(bool isGrounded);

		//virtual bool GetIsJumping();
		//virtual void SetIsJumping(bool isJumping);

		// Dividing the normal and flight modes will be more organized 
		// and will avoid one part interfering with the other.
		virtual void LandBehaviour(double deltaTime);
		virtual void FlyBehaviour(double deltaTime);

		// Control steps
		// StepUp will calculate the start of the jump process before initiating the time steps.
		// The same way, StepDown will calculate the start of the landing process before initiating the time steps.
		// StepForward is what actually moves the object on the direction of the currently set move direction.
		virtual void StepUp(double deltaTime);
		virtual void StepDown(double deltaTime);
		virtual void StepLand(double deltaTime);
		virtual void StepForward(double deltaTime);
		virtual void StepFly(double deltaTime);
		virtual void StepFlyLand(const glm::vec3& hitPoint, double deltaTime);

		virtual void SetFlyX(float X);
		virtual void SetFlyY(float Y);

		// Time steps
		// PreStep will set everything in place, like the ghost objects
		// before beggining the process of calculating the actual current time step.
		virtual void PreStep(double deltaTime);
		virtual void TimeStep(double deltaTime);
		
		// These are all the collision and reposition related parts
		// that will prevent the objects from being inside others
		// or simply to verify collisions with other bodies.
		void UpdateTargetPositionBasedOnCollision(btVector3 worldNormal, btScalar tangentMag = 0.0f, btScalar normalMag = 1.0f);
		bool RecoverFromPenetration();
		bool CheckGhostCollision(int ghostIndex);

		btVector3 ComputeReflection(const btVector3& direction, const btVector3& normal);
		btVector3 ComputeParallel(const btVector3& direction, const btVector3& normal);
		btVector3 ComputePerpendicular(const btVector3& direction, const btVector3& normal);

		glm::vec3 currentVertexPos;

	protected:
		// Saves and Loads the component in Json format
		virtual void _SerializeToJson(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& prettyWriter);
		virtual void _DeserializeFromJson(rapidjson::Value::ConstValueIterator iter);

		sMoveState _moveState;
		sJumpState _jumpState;
		sFlyState _flyState;
		sAttackState _attackState;
		std::string animationState;

		std::string _idleName;
		std::string _walkName;
		std::string _runName;
		std::string _jumpName;
		std::string _fallName;
		std::string _landName;
		std::string _crouchName;
		std::string _flyName;
		std::string _landHardName;
		std::string _attack1Name;

		bool _bIsControllable;
		bool _bTouchingContact;

		sBtRigidbodyComponent* _rb;
		btPairCachingGhostObject* _ghostObject;
		btPairCachingGhostObject* _ghostObjectHand;

		std::vector<btPairCachingGhostObject*> _vecGhostBodies;
		std::vector<nPhysics::iCollisionBody*> _vecSocketBodies;

		std::vector<btPairCachingGhostObject*> _vecPendingGhostBodies;

		// HACK?
		btCollisionWorld* _collisionWorld;
		//btDynamicsWorld* _btWorld;
		cBtDebugRenderer* _btDebugRenderer;
	};
}


#endif // !_bt3DPersonControllerComponent_HG_

