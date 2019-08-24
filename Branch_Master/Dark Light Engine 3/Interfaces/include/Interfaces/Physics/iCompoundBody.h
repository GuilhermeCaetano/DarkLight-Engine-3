#ifndef _iCompoundBody_HG_
#define _iCompoundBody_HG_

#include <OpenGL\OpenGLHeaders.h>
#include <Interfaces\Physics\iCollisionBody.h>
#include <Interfaces\Physics\iRigidbody.h>
#include <Interfaces\Physics\iConstraint.h>
#include <vector>


namespace nPhysics
{
	struct iCompoundBody : public iCollisionBody
	{
	public:
		virtual ~iCompoundBody() {};

		// Get an AABB based on position calculation
		//virtual void GetAABB(glm::vec3& minBoundsOut, glm::vec3& maxBoundsOut) = 0;

		// Have a ghost shape that the character gets in and the door opens and closes when it leaves
		// constraint->setPoweredLinMotor(true)
		// constraint->setMaxLinMotorForce(60.0f)
		// constraint->setTargetLinMotorVelocity(20.0f)

		virtual void AddCompoundBody(iRigidbody* body, bool addID = true) = 0;
		virtual void AddConstraint(eConstraintTypes constraintType, unsigned int indexA, const glm::vec3& pivotInA, const glm::vec3& axisInA) = 0;
		virtual void AddConstraint(eConstraintTypes constraintType, unsigned int indexA, unsigned int indexB, const glm::vec3& pivotInA, const glm::vec3& pivotInB) = 0;
		virtual void AddConstraint(eConstraintTypes constraintType, unsigned int indexA, unsigned int indexB, const glm::vec3& pivotInA, const glm::vec3& pivotInB,
							const glm::vec3& axisInA, const glm::vec3& axisInB) = 0;

		virtual unsigned int GetNumRigidbodies() = 0;
		virtual unsigned int GetNumConstraints() = 0;

		virtual iRigidbody* GetRigidbody(unsigned int index) = 0;

		//virtual glm::vec3 GetControlBodyPosition() = 0;
		//virtual void SetControlBodyPosition(const glm::vec3& controlPosition) = 0;

		//virtual unsigned long GetNumberOfChainNodes() = 0;
		//virtual glm::mat4 GetChainNodeTransform(unsigned long index) = 0;

		std::vector<unsigned long long> _vecEntityIDs;

	protected:
		iCompoundBody() : iCollisionBody(BODY_TYPE_COMPOUNDBODY) {};
		iCompoundBody(const iCompoundBody& other) : iCollisionBody(BODY_TYPE_COMPOUNDBODY) {};

	};
}


#endif // !_iCompoundBody_HG_

