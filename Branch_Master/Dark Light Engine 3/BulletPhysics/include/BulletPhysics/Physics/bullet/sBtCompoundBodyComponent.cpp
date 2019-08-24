// sBulletCompoundBody.cpp

#include "sBtCompoundBodyComponent.h"
#include "sBtRigidbodyComponent.h"
#include "BulletConstraints\cBtBallAndSocketConstraint.h"
#include "BulletConstraints\cBtHingeConstraint.h"
#include "nConvert.h"


namespace nPhysics
{
	sBtCompoundBodyComponent::sBtCompoundBodyComponent(const sCompoundbodyDef& def, unsigned long long entityID)
	{
		this->_parentID = entityID;
		this->bodyType = BODY_TYPE_COMPOUNDBODY;
		this->_collisionLayerMask = eCollisionLayerMask::DEFAULT;

		return;
	}

	sBtCompoundBodyComponent::~sBtCompoundBodyComponent()
	{
		return;
	}

	void sBtCompoundBodyComponent::Start()
	{
	}

	void sBtCompoundBodyComponent::AddCompoundBody(iRigidbody* body, bool addID)
	{
		sBtRigidbodyComponent* rb = static_cast<sBtRigidbodyComponent*>(body);
		this->_vecRigidbodies.push_back(rb);

		if (addID)
		{
			this->_vecEntityIDs.push_back(rb->GetParentID());
		}
	}

	void sBtCompoundBodyComponent::AddConstraint(eConstraintTypes constraintType, unsigned int indexA, const glm::vec3& pivotInA, const glm::vec3& axisInA)
	{
		// Get rigidbodies to connect
		sBtRigidbodyComponent* rbA = this->_vecRigidbodies[indexA];

		if (constraintType == CONSTRAINT_HINGE)
		{
			btVector3 btPivotInA = nConvert::ToBullet(pivotInA);
			btVector3 btAxisInA = nConvert::ToBullet(axisInA);
			cBtHingeConstraint* constraint = new cBtHingeConstraint(rbA, btPivotInA, btAxisInA);
			btHingeConstraint* btConstraint = static_cast<btHingeConstraint*>(constraint->GetTypedConstraint());
			btConstraint->setLimit(btScalar(glm::radians(-30.0f)), btScalar(glm::radians(30.0f)));
			this->_vecConstraints.push_back(constraint);
		}
	}

	void sBtCompoundBodyComponent::AddConstraint(eConstraintTypes constraintType, unsigned int indexA, unsigned int indexB, const glm::vec3& pivotInA, const glm::vec3& pivotInB)
	{
		// Get rigidbodies to connect
		sBtRigidbodyComponent* rbA = this->_vecRigidbodies[indexA];
		sBtRigidbodyComponent* rbB = this->_vecRigidbodies[indexB];

		if (constraintType == CONSTRAINT_BALL_AND_SOCKET)
		{
			// Ball and Socket
			btVector3 btPivotInA = nConvert::ToBullet(pivotInA);
			btVector3 btPivotInB = nConvert::ToBullet(pivotInB);
			cBtBallAndSocketConstraint* constraint = new cBtBallAndSocketConstraint(rbA, rbB, btPivotInA, btPivotInB);
			this->_vecConstraints.push_back(constraint);
		}
	}

	void sBtCompoundBodyComponent::AddConstraint(eConstraintTypes constraintType, unsigned int indexA, unsigned int indexB, const glm::vec3 & pivotInA,
										const glm::vec3 & pivotInB, const glm::vec3 & axisInA, const glm::vec3 & axisInB)
	{

	}

	unsigned int sBtCompoundBodyComponent::GetNumRigidbodies()
	{
		return this->_vecRigidbodies.size();
	}

	unsigned int sBtCompoundBodyComponent::GetNumConstraints()
	{
		return this->_vecConstraints.size();
	}

	glm::mat4 sBtCompoundBodyComponent::GetTransform()
	{
		return glm::mat4();
	}

	void sBtCompoundBodyComponent::SetTransform(const glm::mat4 & transform)
	{
	}

	sLocationComponent* sBtCompoundBodyComponent::GetTransformComponent()
	{
		return this->transform;
	}

	void sBtCompoundBodyComponent::SetTransformComponent(sLocationComponent* locationC)
	{
		this->transform = locationC;
	}

	eCollisionLayerMask sBtCompoundBodyComponent::GetCollisionLayerMask()
	{
		return this->_collisionLayerMask;
	}

	void sBtCompoundBodyComponent::SetCollisionLayerMask(eCollisionLayerMask layerMask)
	{
		this->_collisionLayerMask = layerMask;
	}

	iRigidbody* sBtCompoundBodyComponent::GetRigidbody(unsigned int index)
	{
		return this->_vecRigidbodies[index];
	}

	btTypedConstraint* sBtCompoundBodyComponent::GetBtConstraint(unsigned int index)
	{
		return this->_vecConstraints[index]->GetTypedConstraint();
	}

	void sBtCompoundBodyComponent::GetAABB(glm::vec3& minBoundsOut, glm::vec3& maxBoundsOut)
	{

	}

	void sBtCompoundBodyComponent::_SerializeToJson(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& prettyWriter)
	{

	}

	void sBtCompoundBodyComponent::_DeserializeFromJson(rapidjson::Value::ConstValueIterator iter)
	{

	}
}
