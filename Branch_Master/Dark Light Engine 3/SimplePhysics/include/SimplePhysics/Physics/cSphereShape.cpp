// cSphereShape.cpp

#include <SimplePhysics\Physics\cSphereShape.h>

namespace nPhysics
{
	cSphereShape::cSphereShape(float radius)
	{
		this->_shapeType = eShapeTypes::SHAPE_TYPE_SPHERE;
		this->_radius = radius;

		return;
	}

	cSphereShape::cSphereShape(const iSphereShape& other)
	{
		return;
	}

	cSphereShape::~cSphereShape()
	{
		return;
	}

	
	void cSphereShape::GetAABB(const glm::mat4& transform, glm::vec3& minBoundsOut, glm::vec3& maxBoundsOut)
	{
		const glm::vec4& pos = transform[3];
		minBoundsOut.x = pos.x - this->_radius;
		minBoundsOut.y = pos.y - this->_radius;
		minBoundsOut.z = pos.z - this->_radius;

		maxBoundsOut.x = pos.x + this->_radius;
		maxBoundsOut.y = pos.y + this->_radius;
		maxBoundsOut.z = pos.z + this->_radius;
	}

	eShapeTypes cSphereShape::GetShapeType()
	{
		return this->_shapeType;
	}
	
	float cSphereShape::GetRadius()
	{
		return this->_radius;
	}

}
