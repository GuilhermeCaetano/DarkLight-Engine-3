#ifndef _cBtBallAndSocketConstraint_HG_
#define _cBtBallAndSocketConstraint_HG_

#include <BulletConstraints\iBulletConstraint.h>


namespace nPhysics
{
	class cBtBallAndSocketConstraint : public iBulletConstraint
	{
	public:
		cBtBallAndSocketConstraint(sBtRigidbodyComponent* rb, const btVector3& pivot);
		cBtBallAndSocketConstraint(sBtRigidbodyComponent* rbA, sBtRigidbodyComponent* rbB, const btVector3& pivotInA, const btVector3& pivotInB);
		~cBtBallAndSocketConstraint();

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


#endif // !_cBtBallAndSocketConstraint_HG_

