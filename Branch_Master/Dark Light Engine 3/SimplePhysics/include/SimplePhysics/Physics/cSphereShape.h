#ifndef _cSphereShape_HG_
#define _cSphereShape_HG_

#include <Interfaces\Physics\iSphereShape.h>
#include <OpenGL\OpenGLHeaders.h>


namespace nPhysics
{
	class cSphereShape : public iSphereShape
	{
	public:
		cSphereShape(float radius);
		cSphereShape(const iSphereShape& other);
		~cSphereShape();

		virtual void GetAABB(const glm::mat4& transform, glm::vec3& minBounds, glm::vec3& maxBounds);
		virtual inline eShapeTypes GetShapeType();
		virtual float GetRadius();
	private:
		eShapeTypes _shapeType;
		float _radius;

	};
}


#endif // !_cSphereShape_HG_
