// cBtBallAndSocketConstraint.cpp

#include <BulletConstraints\cBtBallAndSocketConstraint.h>
#include <BulletDynamics\ConstraintSolver\btPoint2PointConstraint.h>
#include <sBtRigidbodyComponent.h>


namespace nPhysics
{
	cBtBallAndSocketConstraint::cBtBallAndSocketConstraint(sBtRigidbodyComponent* rb, const btVector3& pivot) : iBulletConstraint(CONSTRAINT_BALL_AND_SOCKET)
	{
		btTypedConstraint* p2p = new btPoint2PointConstraint(*rb->GetBtRigidbody(), pivot);
		this->_btConstraint = p2p;
		this->_rbA = rb;

		return;
	}

	cBtBallAndSocketConstraint::cBtBallAndSocketConstraint(sBtRigidbodyComponent* rbA, sBtRigidbodyComponent* rbB, const btVector3& pivotInA, const btVector3& pivotInB) : iBulletConstraint(CONSTRAINT_BALL_AND_SOCKET)
	{
		btTypedConstraint* p2p = new btPoint2PointConstraint(*rbA->GetBtRigidbody(), *rbB->GetBtRigidbody(), pivotInA, pivotInB);
		this->_btConstraint = p2p;
		this->_rbA = rbA;
		this->_rbB = rbB;

		return;
	}

	cBtBallAndSocketConstraint::~cBtBallAndSocketConstraint()
	{
		return;
	}

	btTypedConstraint* cBtBallAndSocketConstraint::GetTypedConstraint()
	{
		return this->_btConstraint;
	}

	iRigidbody* cBtBallAndSocketConstraint::GetRigidbodyA()
	{
		return this->_rbA;
	}

	iRigidbody* cBtBallAndSocketConstraint::GetRigidbodyB()
	{
		return this->_rbB;
	}

	inline eConstraintTypes cBtBallAndSocketConstraint::GetConstraintType()
	{
		return eConstraintTypes();
	}
}

