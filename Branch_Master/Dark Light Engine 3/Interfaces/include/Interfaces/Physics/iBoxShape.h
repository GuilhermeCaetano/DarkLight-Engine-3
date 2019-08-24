#ifndef _iBoxShape_HG_
#define _iBoxShape_HG_

#include <Interfaces\Physics\iShape.h>


namespace nPhysics
{
	class iBoxShape : public virtual iShape
	{
	public:
		virtual ~iBoxShape() {};

		virtual glm::vec3 GetBoxExtents() = 0;

	protected:
		iShape* shape;

		iBoxShape() : iShape(SHAPE_TYPE_BOX) {};
		iBoxShape(const iBoxShape& other) : iShape(other) {};
		iBoxShape& operator=(const iBoxShape& other) { return *this; };

	};
}




#endif // !_iBoxShape_HG_

