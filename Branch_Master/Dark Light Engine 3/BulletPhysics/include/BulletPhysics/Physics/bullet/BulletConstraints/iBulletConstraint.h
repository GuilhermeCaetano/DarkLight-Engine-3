#ifndef _iBulletConstraint_HG_
#define _iBulletConstraint_HG_

#include <Interfaces\Physics\iConstraint.h>
#include <BulletDynamics\ConstraintSolver\btTypedConstraint.h>
#include <BulletDynamics\ConstraintSolver\btHingeConstraint.h>
#include <BulletDynamics\ConstraintSolver\btSliderConstraint.h>
#include <BulletDynamics\ConstraintSolver\btFixedConstraint.h>
#include <BulletDynamics\ConstraintSolver\btConeTwistConstraint.h>


namespace nPhysics
{
	class cBulletPhysicsWorld;
	struct sBtRigidbodyComponent;

	class iBulletConstraint : public iConstraint
	{
	public:
		virtual ~iBulletConstraint() {};
		virtual btTypedConstraint* GetTypedConstraint() = 0;

	protected:
		iBulletConstraint(eConstraintTypes constraintType) : iConstraint(constraintType) {};
		iBulletConstraint(const iBulletConstraint& other) : iConstraint(other) {};
		iBulletConstraint& operator=(const iBulletConstraint& other) { return *this; };

	};
}


#endif // !_iBulletConstraint_HG_

