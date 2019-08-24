#ifndef _iRigidbody_HG_
#define _iRigidbody_HG_

#include <string>
#include <OpenGL\OpenGLHeaders.h>
#include <Interfaces\Physics\iCollisionBody.h>
#include <Interfaces\Physics\iShape.h>


namespace nPhysics
{
	enum ForceMode
	{
		Force,
		Acceleration,
		Impulse,
		VelocityChange
	};

	struct iRigidbody : public iCollisionBody
	{
	public:
		virtual ~iRigidbody() {};

		virtual void InitRigidbody() = 0;

		// Returns the type of the rigidbody.
		virtual std::string GetRigidbodyType() = 0;

		// Manage rigidbody's collision shape
		virtual iShape* GetShape() = 0;
		virtual void SetShape(iShape* shape) = 0;

		// Manage rigidbody's shape offset.
		virtual glm::vec3 GetShapeOffset() = 0;
		virtual void SetShapeOffset(const glm::vec3& offset) = 0;

		virtual eShapeTypes GetShapeType() = 0;
		virtual void SetShapeType(eShapeTypes shapeType) = 0;

		// Manage rigidbody's transform

		// Sets initial position for the rigidbody on the world.
		virtual void SetInitialPosition(const glm::vec3& position) = 0;

		// Manage rigidbody's direct position and rotation.
		virtual glm::vec3 GetPosition() = 0;
		virtual glm::quat GetRotation() = 0;
		virtual void SetPosition(const glm::vec3& position) = 0;
		virtual void SetRotation(const glm::quat& rotation) = 0;

		// Returns the minimum and maximum bounds of the collision shape.
		virtual void GetAABB(glm::vec3& minBoundsOut, glm::vec3& maxBoundsOut) = 0;

		// Manage rigidbody's kinematic and gravity properties
		virtual bool GetUseGravity() = 0;
		virtual bool GetIsKinematic() = 0;
		virtual void SetUseGravity(bool useGravity) = 0;
		virtual void SetIsKinematic(bool isKinematic) = 0;

		// Manage rigidbody's other parameters
		virtual float GetMass() = 0;
		virtual float GetDrag() = 0;
		virtual float GetBounciness() = 0;
		virtual void SetMass(float mass) = 0;
		virtual void SetDrag(float drag) = 0;
		virtual void SetBounciness(float bounciness) = 0;

		// Adds acceleration to the rigidbody:
		// Either directly, relative to a vector, or at a certain position.
		virtual void AddForce(const glm::vec3& force, ForceMode mode) = 0;
		virtual void AddRelativeForce(const glm::vec3& force, ForceMode mode) = 0;
		virtual void AddForceAtPosition(const glm::vec3& force, const glm::vec3& position, ForceMode mode) = 0;

		// Manage rigidbody's acceleration and velocity directly
		virtual glm::vec3 GetAcceleration() = 0;
		virtual void SetAcceleration(const glm::vec3& acceleration) = 0;
		virtual glm::vec3 GetVelocity() = 0;
		virtual void SetVelocity(const glm::vec3& velocity) = 0;

		// Manage rigidbody's freeze stats
		virtual glm::vec3 GetFreezePosition() = 0;
		virtual glm::vec3 GetFreezeRotation() = 0;

		// Zeroes rigidbody's acceleration and velocitly immediately.
		virtual void StopRigidbody() = 0;

	protected:
		iRigidbody() : iCollisionBody(eBodyType::BODY_TYPE_RIGIDBODY) {};
		iRigidbody(const iRigidbody& other) : iCollisionBody(eBodyType::BODY_TYPE_RIGIDBODY) {};

		eShapeTypes _shapeType;
	};
}


#endif // !_iRigidbody_HG_
