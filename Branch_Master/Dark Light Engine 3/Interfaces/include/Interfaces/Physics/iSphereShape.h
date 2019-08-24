#ifndef _iSphereShape_HG_
#define _iSphereShape_HG_

#include <Interfaces\Physics\iShape.h>

namespace nPhysics
{
	class iSphereShape : public virtual iShape
	{
	public:
		virtual ~iSphereShape() {};

		virtual inline eShapeTypes GetShapeType() = 0;
		virtual float GetRadius() = 0;

	protected:
		iSphereShape() : iShape(eShapeTypes::SHAPE_TYPE_SPHERE) {};
		iSphereShape(const iSphereShape& other) : iShape(other) {};
		iSphereShape& operator=(const iSphereShape& other) { return *this; };

	};
}


#endif // !_iSphereShape_HG_
