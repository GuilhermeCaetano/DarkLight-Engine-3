#ifndef _iHeightfieldTerrainShape_HG_
#define _iHeightfieldTerrainShape_HG_

#include "iShape.h"


namespace nPhysics
{
	class iHeightfieldTerrainShape : public virtual iShape
	{
	public:
		virtual ~iHeightfieldTerrainShape() {};

	protected:
		iHeightfieldTerrainShape() : iShape(SHAPE_TYPE_HEIGHTFIELD_TERRAIN) {};
		iHeightfieldTerrainShape(const iHeightfieldTerrainShape& other) : iShape(other) {};
		iHeightfieldTerrainShape& operator=(const iHeightfieldTerrainShape& other) { return *this; };

	};
}



#endif // !_iHeightfieldTerrainShape_HG_

