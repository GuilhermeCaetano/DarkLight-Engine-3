// cBtSliderConstraint.cpp

#include <BulletConstraints\cBtSliderConstraint.h>
#include <BulletDynamics\ConstraintSolver\btSliderConstraint.h>
#include <btBulletDynamicsCommon.h>
#include <sBtRigidbodyComponent.h>


namespace nPhysics
{
	cBtSliderConstraint::cBtSliderConstraint(sBtRigidbodyComponent* rbA, sBtRigidbodyComponent* rbB, const btTransform& frameInA, const btTransform& frameInB, bool referenceFrameInA) : iBulletConstraint(CONSTRAINT_SLIDER)
	{
		btTypedConstraint* slider = new btSliderConstraint(*rbA->GetBtRigidbody(), *rbB->GetBtRigidbody(), frameInA, frameInB, true);

		this->_btConstraint = slider;
		this->_rbA = rbA;
		this->_rbB = rbB;

		return;
	}

	cBtSliderConstraint::~cBtSliderConstraint()
	{
		return;
	}

	btTypedConstraint* cBtSliderConstraint::GetTypedConstraint()
	{
		return this->_btConstraint;
	}

	iRigidbody* cBtSliderConstraint::GetRigidbodyA()
	{
		return this->_rbA;
	}

	iRigidbody* cBtSliderConstraint::GetRigidbodyB()
	{
		return this->_rbB;
	}

	inline eConstraintTypes cBtSliderConstraint::GetConstraintType()
	{
		return eConstraintTypes();
	}
}