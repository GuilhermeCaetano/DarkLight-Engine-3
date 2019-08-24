#ifndef _cBtHingeConstraint_HG_
#define _cBtHingeConstraint_HG_

#include <BulletConstraints\iBulletConstraint.h>


namespace nPhysics
{
	class cBtHingeConstraint : public iBulletConstraint
	{
	public:
		cBtHingeConstraint(sBtRigidbodyComponent* rbA, const btVector3& pivotInA, const btVector3& axisInA);
		~cBtHingeConstraint();

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


#endif // !_cBtHingeConstraint_HG_

