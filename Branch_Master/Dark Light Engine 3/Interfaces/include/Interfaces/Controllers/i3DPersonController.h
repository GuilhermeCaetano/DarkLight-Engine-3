#ifndef _i3DPersonController_HG_
#define _i3DPersonController_HG_

#include <string>
#include <OpenGL\OpenGLHeaders.h>
#include <Entity\Components\iComponent.h>
#include <Interfaces\Physics\iShape.h>
#include <Interfaces\Physics\iCollisionBody.h>
#include <Interfaces\Physics\iPhysicsSystem.h>


struct i3DPersonController : public iComponent
{
public:
	virtual ~i3DPersonController() {};

	struct sMoveState
	{
		glm::vec3 initialPosition = glm::vec3(0.0f);
		glm::vec3 curPosition = glm::vec3(0.0f);
		glm::vec3 targetPosition = glm::vec3(0.0f);
		glm::vec3 lastPosition = glm::vec3(0.0f);

		glm::quat curOrientation = glm::quat();
		glm::quat targetOrientation = glm::quat();

		glm::vec3 stickDirection = glm::vec3(0.0f);
		glm::vec3 normStickDirection = glm::vec3(0.0f);

		float currentFrameYPos = 0.0f;

		glm::vec3 gravity = glm::vec3(0.0f);

		float verticalVelocity = 0.0f;
		float horizontalVelocity = 0.0f;
		float curVerticalOffset = 0.0f;
		float curHorizontalOffset = 0.0f;

		float maxVerticalVelocity = 90.0f;

		float moveSpeed = 1.0f;
		float rotationSpeed = 5.0f;

		bool bMoved = false;
		bool bCanMove = true;
	};

	struct sJumpState
	{
		enum eStates
		{
			UP,
			FALL,
			LAND,
			GROUNDED
		};

		eStates state = eStates::GROUNDED;

		float jumpStep = 1.0f;
		float jumpSpeed = 90.0f;
		float fallSpeed = -960.0f;
		float maxLandTime = 0.1f;
		float curLandTime = 0.1f;

		glm::vec3 initJumpPosition = glm::vec3(0.0f);
		glm::vec3 curJumpPosition = glm::vec3(0.0f);
		glm::vec3 targetJumpPosition = glm::vec3(0.0f);

		glm::vec3 jumpDirection = glm::vec3(0.0f);
		glm::vec3 normJumpDirection = glm::vec3(0.0f);
		glm::vec3 normJumpMomentum = glm::vec3(0.0f);

		glm::vec3 jumpPosition;
		glm::vec3 directionVelocity;

		bool wasGrounded = true;
		bool bJumped = false;
	};

	struct sFlyState
	{
		enum eStates
		{
			YIELD,
			CROUCH,
			FLY,
			LANDHARD
		};

		eStates _state = YIELD;

		float flySpeed = 20.0f;
		float maxCrouchTime = 0.15f;
		float maxLandTime = 0.5f;
		float curTime = 0.0f;
		float waitTime = 2.0f;

		glm::vec3 initFlyPosition = glm::vec3(0.0f);
		glm::vec3 curFlyPosition = glm::vec3(0.0f);
		glm::vec3 targetFlyPosition = glm::vec3(0.0f);

		glm::vec3 flyDirection = glm::vec3(0.0f);
		glm::vec3 normFlyDirection = glm::vec3(0.0f);

		bool wasGrounded = false;
		bool bFlying = false;

		glm::vec3 rotValue = glm::vec3(0.0f);
	};

	struct sAttackState
	{
		float maxAttackTime = 3.0f;
		float currentAttackTime = 0.0f;

		bool bIsAttacking = false;
	};

	// Manage controller jump state
	//virtual std::string CheckForGrounded(double deltaTime) = 0;
	//virtual sJumpState GetJumpState() = 0;
	//virtual std::string Jump() = 0;

	virtual void SetCollisionBody(nPhysics::iCollisionBody* colBody) = 0;
	virtual void SetGhostBody(nPhysics::iShape* ghostBodyShape) = 0;

	virtual void AddSocketBody(nPhysics::iCollisionBody* socketBody) = 0;
	virtual void SetSocketObjectTransform(int index, const glm::mat4& transform) = 0;
	virtual void SetSocketObjectPosition(int index, const glm::vec3& position, const glm::quat& rotation) = 0;
	virtual void SetHandPosition(const glm::vec3& position, const glm::quat& rotation) = 0;

	// Getters and setters
	virtual void SetGravity(const glm::vec3& gravity) = 0;
	virtual void SetMoveDirection(const glm::vec3& moveDirection) = 0;
	virtual void SetJumpDirection(const glm::vec3& jumpDirection) = 0;
	virtual void SetFlyDirection(const glm::vec3& flyDirection) = 0;
	virtual void SetAttack() = 0;

	virtual void SetFlyX(float X) = 0;
	virtual void SetFlyY(float Y) = 0;

	virtual bool GetIsControllable() = 0;
	virtual void SetIsControllable(bool isControllable) = 0;

	virtual void DropHeldObject() = 0;

	virtual std::string GetAnimationName() = 0;

	//virtual void SetLinearVelocity(const glm::vec3& velocity) = 0;
	//virtual glm::vec3& GetLinearVelocity() const = 0;
	//virtual void SetAngularVelocity(const glm::vec3& velocity) = 0;
	//virtual const glm::vec3& SetAngularVelocity() const = 0;

	//virtual float GetMaxMoveSpeed() = 0;
	//virtual void SetMaxMoveSpeed(float maxSpeed) = 0;
	//virtual float GetMaxAngularSpeed() = 0;
	//virtual void SetMaxAngularSpeed(float maxSpeed) = 0;

	//virtual bool GetIsRunning() = 0;
	//virtual void SetIsRunning(bool isRunning) = 0;

	virtual bool GetIsAttacking() = 0;

	//virtual bool GetIsStrafing() = 0;
	//virtual void SetIsStrafing(bool isStrafing) = 0;

	virtual bool GetIsGrouded() = 0;
	virtual void SetIsGrouded(bool isGrounded) = 0;

	//virtual bool GetIsJumping() = 0;
	//virtual void SetIsJumping(bool isJumping) = 0;

	// Dividing the normal and flight modes will be more organized and will avoid 
	// one part interfering with the other.
	virtual void LandBehaviour(double deltaTime) = 0;
	virtual void FlyBehaviour(double deltaTime) = 0;

	// Control steps
	// StepUp will calculate the start of the jump process before initiating the time steps.
	// The same way, StepDown will calculate the start of the landing process before initiating the time steps.
	// StepForward is what actually moves the object on the direction of the currently set move direction.
	virtual void StepUp(double deltaTime) = 0;
	virtual void StepDown(double deltaTime) = 0;
	virtual void StepLand(double deltaTime) = 0;
	virtual void StepForward(double deltaTime) = 0;
	virtual void StepFly(double deltaTime) = 0;
	virtual void StepFlyLand(const glm::vec3& hitPoint, double deltaTime) = 0;

	

	// Time steps
	// PreStep will set everything in place, like the ghost objects
	// before beggining the process of calculating the actual current time step.
	virtual void PreStep(double deltaTime) = 0;
	virtual void TimeStep(double deltaTime) = 0;

	//virtual void Warp(const glm::vec3& movePosition) = 0;

protected:
	i3DPersonController() {};
	i3DPersonController(const i3DPersonController& other) {};
	i3DPersonController& operator=(const i3DPersonController& other) { return *this; };

};


#endif // !_i3DPersonController_HG_

