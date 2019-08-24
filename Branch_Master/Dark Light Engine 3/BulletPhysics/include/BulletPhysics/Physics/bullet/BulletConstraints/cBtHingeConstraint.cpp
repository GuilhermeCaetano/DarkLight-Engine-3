// cBtHingeConstraint.cpp

#include <BulletConstraints\cBtHingeConstraint.h>
#include <BulletDynamics\ConstraintSolver\btHingeConstraint.h>
#include <sBtRigidbodyComponent.h>


namespace nPhysics
{
	cBtHingeConstraint::cBtHingeConstraint(sBtRigidbodyComponent* rbA, const btVector3& pivotInA, const btVector3& axisInA) : iBulletConstraint(CONSTRAINT_HINGE)
	{
		btTypedConstraint* slider = new btHingeConstraint(*rbA->GetBtRigidbody(), pivotInA, axisInA);
		this->_btConstraint = slider;
		this->_rbA = rbA;

		return;
	}

	cBtHingeConstraint::~cBtHingeConstraint()
	{
		return;
	}

	btTypedConstraint* cBtHingeConstraint::GetTypedConstraint()
	{
		return this->_btConstraint;
	}

	iRigidbody* cBtHingeConstraint::GetRigidbodyA()
	{
		return this->_rbA;
	}

	iRigidbody* cBtHingeConstraint::GetRigidbodyB()
	{
		return this->_rbB;
	}
	inline eConstraintTypes cBtHingeConstraint::GetConstraintType()
	{
		return eConstraintTypes();
	}
}
