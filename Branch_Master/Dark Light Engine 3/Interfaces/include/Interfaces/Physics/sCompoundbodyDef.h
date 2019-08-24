#ifndef _sCompoundbodyDef_HG_
#define _sCompoundbodyDef_HG_

#include <OpenGL\OpenGLHeaders.h>
#include "iPhysicsDef.h"
#include "iConstraint.h"


namespace nPhysics
{
	struct sCompoundbodyDef : public iPhysicsDef
	{
		sCompoundbodyDef()
		{
			this->defType = "Compoundbody";
		}

		~sCompoundbodyDef() {};

		eConstraintTypes constrainType = CONSTRAINT_BALL_AND_SOCKET;
	};
}

#endif // !_sCompoundbodyDef_HG_
