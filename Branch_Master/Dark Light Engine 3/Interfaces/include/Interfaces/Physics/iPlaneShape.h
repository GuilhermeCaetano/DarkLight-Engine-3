#ifndef _iPlaneShape_HG_
#define _iPlaneShape_HG_

#include <Interfaces\Physics\iShape.h>
#include <OpenGL\OpenGLHeaders.h>

namespace nPhysics
{
	class iPlaneShape : public virtual iShape
	{
	public:
		virtual ~iPlaneShape() {};

		virtual float GetConstant() = 0;
		virtual glm::vec3 GetNormal() = 0;

	protected:
		iPlaneShape(const glm::vec3& normal, float constant) : iShape(eShapeTypes::SHAPE_TYPE_PLANE) {};
		iPlaneShape() : iShape(eShapeTypes::SHAPE_TYPE_PLANE) {};
		iPlaneShape(const iPlaneShape& other) : iShape(other) {};

	};
}


#endif // !_iPlaneShape_HG_
