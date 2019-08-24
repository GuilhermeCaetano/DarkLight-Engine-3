#ifndef _iBulletCompoundBody_HG_
#define _iBulletCompoundBody_HG_

#include <Interfaces\Physics\iCompoundBody.h>
#include <BulletDynamics\ConstraintSolver\btTypedConstraint.h>


namespace nPhysics
{
	struct iBulletCompoundBody : public iCompoundBody
	{
	public:
		virtual ~iBulletCompoundBody() {};

		virtual btTypedConstraint* GetBtConstraint(unsigned int index) = 0;

	protected:
		iBulletCompoundBody() : iCompoundBody() {};

	};
}



#endif // !_iBulletCompoundBody_HG_

