#ifndef _cPlaneShape_HG_
#define _cPlaneShape_HG_

#include <Interfaces\Physics\iPlaneShape.h>


namespace nPhysics
{
	class cPlaneShape : public virtual iPlaneShape
	{
	public:
		cPlaneShape();
		cPlaneShape(const glm::vec3& normal, float constant);
		cPlaneShape(const iPlaneShape& other);
		~cPlaneShape();

		virtual void GetAABB(const glm::mat4& transform, glm::vec3& minBoundsOut, glm::vec3& maxBoundsOut);
		virtual inline eShapeTypes GetShapeType();
		virtual float GetConstant();
		virtual glm::vec3 GetNormal();

	private:
		eShapeTypes _shapeType;

		glm::vec3 _planeNormal;
		float _planeConstant;

	};
}


#endif // !_cPlaneShape_HG_
