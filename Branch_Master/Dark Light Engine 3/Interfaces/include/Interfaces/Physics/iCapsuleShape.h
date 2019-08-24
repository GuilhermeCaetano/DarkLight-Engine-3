#ifndef _iCapsuleShape_HG_
#define _iCapsuleShape_HG_

#include <Interfaces\Physics\iShape.h>


namespace nPhysics
{
	class iCapsuleShape : public virtual iShape
	{
	public:
		virtual ~iCapsuleShape() {};

		virtual glm::vec3 GetCylinderExtents() = 0;

	protected:
		iCapsuleShape() : iShape(SHAPE_TYPE_CAPSULE) {};
		iCapsuleShape(const iCapsuleShape& other) : iShape(other) {};
		iCapsuleShape& operator=(const iCapsuleShape& other) { return *this; };
	};
}



#endif // !_iCapsuleShape_HG_
