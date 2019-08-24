#ifndef _cBtFixedConstraint_HG_
#define _cBtFixedConstraint_HG_

#include <BulletConstraints\iBulletConstraint.h>


namespace nPhysics
{
	class cBtFixedConstraint : public iBulletConstraint
	{
	public:
		cBtFixedConstraint(sBtRigidbodyComponent* rbA, sBtRigidbodyComponent* rbB, const btTransform& frameInA, const btTransform& frameInB);
		~cBtFixedConstraint();

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


#endif // !_cBtFixedConstraint_HG_
