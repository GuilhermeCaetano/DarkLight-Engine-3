#ifndef _iConstraint_HG_
#define _iConstraint_HG_


namespace nPhysics
{
	struct iRigidbody;

	enum eConstraintTypes
	{
		CONSTRAINT_HINGE,
		CONSTRAINT_SLIDER,
		CONSTRAINT_CONE_TWIST,
		CONSTRAINT_BALL_AND_SOCKET,
		CONSTRAINT_6_DOF,
		CONSTRAINT_FIXED,
		CONSTRAINT_GEAR
	};

	class iConstraint
	{
	public:
		virtual ~iConstraint() {};

		virtual iRigidbody* GetRigidbodyA() = 0;
		virtual iRigidbody* GetRigidbodyB() = 0;

		// Constraint type
		inline virtual eConstraintTypes GetConstraintType() = 0;

	protected:
		iConstraint(eConstraintTypes constraintType) {};
		iConstraint(const iConstraint& other) {};
		iConstraint& operator=(const iConstraint& other) { return *this; };

	private:
		eConstraintTypes _constraintType;

	};
}



#endif // !_iConstraints_HG_

