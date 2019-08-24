#ifndef _cBtSliderConstraint_HG_
#define _cBtSliderConstraint_HG_

#include <BulletConstraints\iBulletConstraint.h>


namespace nPhysics
{
	class cBtSliderConstraint : public iBulletConstraint
	{
	public:
		cBtSliderConstraint(sBtRigidbodyComponent* rbA, sBtRigidbodyComponent* rbB, const btTransform& frameInA, const btTransform& frameInB, bool referenceFrameInA);
		~cBtSliderConstraint();

		virtual btTypedConstraint* GetTypedConstraint();

		virtual iRigidbody* GetRigidbodyA();
		virtual iRigidbody* GetRigidbodyB();

		// Constraint type
		inline virtual eConstraintTypes GetConstraintType();

	private:
		sBtRigidbodyComponent* _rbA;
		sBtRigidbodyComponent* _rbB;
		btTypedConstraint* _btConstraint;

	};
}


#endif // !_cBtSliderConstraint_HG_
