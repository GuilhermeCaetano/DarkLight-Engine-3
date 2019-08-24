#ifndef _sSoftbodyDef_HG_
#define _sSoftbodyDef_HG_

#include "iPhysicsDef.h"


namespace nPhysics
{
	struct sSoftbodyDef : public iPhysicsDef
	{
		sSoftbodyDef()
		{
			this->defType = "Softbody";
		}

		~sSoftbodyDef() {};

		float springConstant = 100.0f;

		unsigned int numNodesAB = 8;
		unsigned int numNodesAC = 8;

		float cornerA[3] = {-8.0f, 8.0f, 0.0f};
		float cornerB[3] = {8.0f, 8.0f, 0.0f};
		float cornerC[3] = {-8.0f, -8.0f, 0.0f};
	};
}



#endif // !_sSoftbodyDef_HG_

