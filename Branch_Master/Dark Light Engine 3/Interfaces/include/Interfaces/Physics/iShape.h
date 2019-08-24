#ifndef _iShape_HG_
#define _iShape_HG_

#include <string>
#include <OpenGL\OpenGLHeaders.h>


namespace nPhysics
{
	enum eShapeTypes
	{
		SHAPE_TYPE_PLANE = 0,
		SHAPE_TYPE_SPHERE = 1,
		SHAPE_TYPE_BOX = 2,
		SHAPE_TYPE_TETRAHEDRON = 3,
		SHAPE_TYPE_CYLINDER = 4,
		SHAPE_TYPE_CAPSULE = 5,
		SHAPE_TYPE_HEIGHTFIELD_TERRAIN = 6,
		SHAPE_TYPE_BVH_TRIANGLE_MESH = 7,
		SHAPE_TYPE_GHOST = 8
	};

	enum ePhyScalarTypes
	{
		PHY_FLOAT,
		PHY_DOUBLE,
		PHY_INTEGER,
		PHY_SHORT,
		PHY_FIXEDPOINT88,
		PHY_UCHAR
	};

	class iShape
	{
	public:
		virtual ~iShape() {};

		static std::string EnumToString(eShapeTypes type)
		{
			if (type == SHAPE_TYPE_PLANE) { return "PLANE"; }
			else if (type == SHAPE_TYPE_SPHERE) { return "SPHERE"; }
			else if (type == SHAPE_TYPE_BOX) 
			{
				return "BOX";
			}
			else if (type == SHAPE_TYPE_TETRAHEDRON) { return "TETRAHEDRON"; }
			else if (type == SHAPE_TYPE_CYLINDER) { return "CYLINDER"; }
			else if (type == SHAPE_TYPE_BVH_TRIANGLE_MESH) { return "TRIANGLE_MESH"; }
		}

		static eShapeTypes StringToShapeEnum(std::string type)
		{
			if (type == "PLANE") { return SHAPE_TYPE_PLANE; }
			else if (type == "SPHERE") { return SHAPE_TYPE_SPHERE; }
			else if (type == "BOX") { return SHAPE_TYPE_BOX; }
			else if (type == "TETRAHEDRON") { return SHAPE_TYPE_TETRAHEDRON; }
			else if (type == "CYLINDER") { return SHAPE_TYPE_CYLINDER; }
			else if (type == "TRIANGLE_MESH") { return SHAPE_TYPE_BVH_TRIANGLE_MESH; }
		}

		virtual void GetAABB(const glm::mat4& transform, glm::vec3& minBounds, glm::vec3& maxBounds) = 0;
		virtual inline eShapeTypes GetShapeType() = 0;

	protected:
		iShape() {};
		iShape(eShapeTypes shapeType) {};
		iShape(const iShape& other) {};

	};
}

#endif // !_iShape_HG_
