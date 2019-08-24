// cBtFixedConstraint.cpp

#include <BulletConstraints\cBtFixedConstraint.h>
#include <BulletDynamics\ConstraintSolver\btFixedConstraint.h>
#include <sBtRigidbodyComponent.h>


namespace nPhysics
{
	cBtFixedConstraint::cBtFixedConstraint(sBtRigidbodyComponent* rbA, sBtRigidbodyComponent* rbB, const btTransform& frameInA, const btTransform& frameInB) : iBulletConstraint(CONSTRAINT_FIXED)
	{
		btTypedConstraint* fixed = new btFixedConstraint(*rbA->GetBtRigidbody(), *rbB->GetBtRigidbody(), frameInA, frameInB);
		this->_btConstraint = fixed;
		this->_rbA = rbA;
		this->_rbB = rbB;

		return;
	}

	cBtFixedConstraint::~cBtFixedConstraint()
	{
		return;
	}

	btTypedConstraint* cBtFixedConstraint::GetTypedConstraint()
	{
		return this->_btConstraint;
	}

	iRigidbody* cBtFixedConstraint::GetRigidbodyA()
	{
		return this->_rbA;
	}

	iRigidbody* cBtFixedConstraint::GetRigidbodyB()
	{
		return this->_rbB;
	}

	inline eConstraintTypes cBtFixedConstraint::GetConstraintType()
	{
		return eConstraintTypes();
	}
}