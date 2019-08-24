// cPlaneShape.cpp

#include <SimplePhysics\Physics\cPlaneShape.h>

namespace nPhysics
{
	cPlaneShape::cPlaneShape()
	{
		this->_shapeType = eShapeTypes::SHAPE_TYPE_PLANE;

		return;
	}

	cPlaneShape::cPlaneShape(const glm::vec3& normal, float constant)
	{
		this->_shapeType = eShapeTypes::SHAPE_TYPE_PLANE;
		this->_planeNormal = normal;
		this->_planeConstant = constant;

		return;
	}

	cPlaneShape::cPlaneShape(const iPlaneShape& other)
	{
		

		return;
	}

	cPlaneShape::~cPlaneShape()
	{
		return;
	}

	void cPlaneShape::GetAABB(const glm::mat4 & transform, glm::vec3 & minBoundsOut, glm::vec3 & maxBoundsOut)
	{
		minBoundsOut.x = std::numeric_limits<float>::min();
		minBoundsOut.y = std::numeric_limits<float>::min();
		minBoundsOut.z = std::numeric_limits<float>::min();

		maxBoundsOut.x = std::numeric_limits<float>::max();
		maxBoundsOut.y = std::numeric_limits<float>::max();
		maxBoundsOut.z = std::numeric_limits<float>::max();
	}

	eShapeTypes cPlaneShape::GetShapeType()
	{
		return this->_shapeType;
	}

	float cPlaneShape::GetConstant()
	{
		return this->_planeConstant;
	}

	glm::vec3 cPlaneShape::GetNormal()
	{
		return this->_planeNormal;
	}
}