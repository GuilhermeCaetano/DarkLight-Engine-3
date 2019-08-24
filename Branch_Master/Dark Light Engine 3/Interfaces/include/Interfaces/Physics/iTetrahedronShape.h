#ifndef _iTetrahedronShape_HG_
#define _iTetrahedronShape_HG_

#include <Interfaces\Physics\iShape.h>


namespace nPhysics
{
	class iTetrahedronShape : public virtual iShape
	{
	public:
		virtual ~iTetrahedronShape() {};

		virtual glm::vec3 GetTetrahedronPoint(int index) = 0;

	protected:
		iTetrahedronShape() : iShape(SHAPE_TYPE_TETRAHEDRON) {};
		iTetrahedronShape(const iTetrahedronShape& other) : iShape(other) {};
		iTetrahedronShape& operator=(const iTetrahedronShape& other) { return *this; };

	};
}



#endif // !_iTetrahedronShape_HG_

