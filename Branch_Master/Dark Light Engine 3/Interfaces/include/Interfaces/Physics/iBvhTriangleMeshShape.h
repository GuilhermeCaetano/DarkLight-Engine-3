#ifndef _iBvhTriangleMeshShape_HG_
#define _iBvhTriangleMeshShape_HG_

#include "iShape.h"


namespace nPhysics
{
	class iBvhTriangleMeshShape : public virtual iShape
	{
	public:
		virtual ~iBvhTriangleMeshShape() {};

	protected:
		iBvhTriangleMeshShape() : iShape(SHAPE_TYPE_BVH_TRIANGLE_MESH) {};
		iBvhTriangleMeshShape(const iBvhTriangleMeshShape& other) : iShape(other) {};
		iBvhTriangleMeshShape& operator=(const iBvhTriangleMeshShape& other) { return *this; };

	};
}


#endif // !_iBvhTriangleMeshShape_HG_

