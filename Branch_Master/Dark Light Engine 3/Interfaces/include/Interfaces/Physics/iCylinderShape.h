#ifndef _iCylinderShape_HG_
#define _iCylinderShape_HG_

#include <Interfaces\Physics\iShape.h>


namespace nPhysics
{
	class iCylinderShape : public virtual iShape
	{
	public:
		virtual ~iCylinderShape() {};

		virtual glm::vec3 GetCylinderExtents() = 0;

	protected:
		iCylinderShape() : iShape(SHAPE_TYPE_CYLINDER) {};
		iCylinderShape(const iCylinderShape& other) : iShape(other) {};
		iCylinderShape& operator=(const iCylinderShape& other) { return *this; };
	};
}



#endif // !_iCylinderShape_HG_

