#ifndef _iBulletShapes_HG_
#define _iBulletShapes_HG_

#include <Interfaces\Physics\iShape.h>
#include <Interfaces\Physics\iPlaneShape.h>
#include <Interfaces\Physics\iSphereShape.h>
#include <Interfaces\Physics\iBoxShape.h>
#include <Interfaces\Physics\iTetrahedronShape.h>
#include <Interfaces\Physics\iCylinderShape.h>
#include <Interfaces\Physics\iCapsuleShape.h>
#include <Interfaces\Physics\iHeightfieldTerrainShape.h>
#include <Interfaces\Physics\iBvhTriangleMeshShape.h>

#include "btBulletDynamicsCommon.h"
#include <BulletCollision\CollisionShapes\btCylinderShape.h>
#include <Mesh\sModelDrawInfo.h>
#include <vector>


namespace nPhysics
{
	class iBulletShapes : public virtual iShape
	{
	public:
		virtual ~iBulletShapes() {};
		virtual inline btCollisionShape* GetCollisionShape() = 0;

	protected:
		iBulletShapes();
		iBulletShapes(const iBulletShapes& other) : iShape(other) {};
		iBulletShapes& operator=(const iBulletShapes& other) { return *this; }

		btCollisionShape* _shape;
	};

	class cBulletSphereShape : public iBulletShapes, public iSphereShape
	{
	public:
		cBulletSphereShape(float radius);
		virtual ~cBulletSphereShape();

		virtual inline eShapeTypes GetShapeType();
		virtual inline btCollisionShape* GetCollisionShape();
		virtual float GetRadius();

		virtual void GetAABB(const glm::mat4& transform, glm::vec3& minBounds, glm::vec3& maxBounds);

	protected:
		cBulletSphereShape(const cBulletSphereShape& other) : iBulletShapes(other), iShape(other) {};
		cBulletSphereShape& operator=(const cBulletSphereShape& other) { return *this; }

		float _radius;
	};

	class cBulletPlaneShape : public iBulletShapes, public iPlaneShape
	{
	public:
		cBulletPlaneShape(const glm::vec3& normal, float constant);
		virtual ~cBulletPlaneShape();

		virtual inline eShapeTypes GetShapeType();
		virtual inline btCollisionShape* GetCollisionShape();
		virtual glm::vec3 GetNormal();
		virtual float GetConstant();

		virtual void GetAABB(const glm::mat4& transform, glm::vec3& minBounds, glm::vec3& maxBounds);

	protected:
		cBulletPlaneShape(const cBulletPlaneShape& other) : iBulletShapes(other), iShape(other) {};
		cBulletPlaneShape& operator=(const cBulletPlaneShape& other) { return *this; }

		glm::vec3 _normal;
		float _constant;
	};

	class cBulletBoxShape : public iBulletShapes, public iBoxShape
	{
	public:
		cBulletBoxShape(const glm::vec3& extent);
		virtual ~cBulletBoxShape();

		virtual inline eShapeTypes GetShapeType();
		virtual inline btCollisionShape* GetCollisionShape();
		virtual glm::vec3 GetBoxExtents();

		virtual void GetAABB(const glm::mat4& transform, glm::vec3& minBounds, glm::vec3& maxBounds);

	protected:
		cBulletBoxShape(const cBulletPlaneShape& other) : iBulletShapes(other), iShape(other) {};
		cBulletBoxShape& operator=(const cBulletBoxShape& other) { return *this; }

		glm::vec3 _extent;
	};

	class cBulletTetrahedronShape : public iBulletShapes, public iTetrahedronShape
	{
	public:
		cBulletTetrahedronShape(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);
		virtual ~cBulletTetrahedronShape();

		virtual inline eShapeTypes GetShapeType();
		virtual inline btCollisionShape* GetCollisionShape();
		virtual glm::vec3 GetTetrahedronPoint(int index);

		virtual void GetAABB(const glm::mat4& transform, glm::vec3& minBounds, glm::vec3& maxBounds);

	protected:
		cBulletTetrahedronShape(const cBulletPlaneShape& other) : iBulletShapes(other), iShape(other) {};
		cBulletTetrahedronShape& operator=(const cBulletTetrahedronShape& other) { return *this; }

		std::vector<glm::vec3> vecPoints;
	};

	class cBulletCylinderShape : public iBulletShapes, public iCylinderShape
	{
	public:
		cBulletCylinderShape(const glm::vec3& extent);
		virtual ~cBulletCylinderShape();

		virtual inline eShapeTypes GetShapeType();
		virtual inline btCollisionShape* GetCollisionShape();
		virtual glm::vec3 GetCylinderExtents();

		virtual void GetAABB(const glm::mat4& transform, glm::vec3& minBounds, glm::vec3& maxBounds);

	protected:
		cBulletCylinderShape(const cBulletPlaneShape& other) : iBulletShapes(other), iShape(other) {};
		cBulletCylinderShape& operator=(const cBulletCylinderShape& other) { return *this; }

		glm::vec3 _extent;
	};

	class cBulletCapsuleShape : public iBulletShapes, public iCapsuleShape
	{
	public:
		cBulletCapsuleShape(float radius, float height);
		virtual ~cBulletCapsuleShape();

		virtual inline eShapeTypes GetShapeType();
		virtual inline btCollisionShape* GetCollisionShape();
		virtual glm::vec3 GetCylinderExtents();

		virtual void GetAABB(const glm::mat4& transform, glm::vec3& minBounds, glm::vec3& maxBounds);

	protected:
		cBulletCapsuleShape(const cBulletPlaneShape& other) : iBulletShapes(other), iShape(other) {};
		cBulletCapsuleShape& operator=(const cBulletCapsuleShape& other) { return *this; }

		glm::vec3 _extent;
	};

	class cBulletHeightfieldTerrainShape : public iBulletShapes, public iHeightfieldTerrainShape
	{
	public:
		cBulletHeightfieldTerrainShape(int heightStickWidth, int heightStickLength, const void* heightfieldData,
			float heightScale, float minHeight, float maxHeight, int upAxis, ePhyScalarTypes hdt, bool flipQuadEdges);
		virtual ~cBulletHeightfieldTerrainShape();

		virtual inline eShapeTypes GetShapeType();
		virtual inline btCollisionShape* GetCollisionShape();

		virtual void GetAABB(const glm::mat4& transform, glm::vec3& minBounds, glm::vec3& maxBounds);

	protected:
		cBulletHeightfieldTerrainShape(const cBulletHeightfieldTerrainShape& other) : iBulletShapes(other), iShape(other) {};
		cBulletHeightfieldTerrainShape& operator=(const cBulletHeightfieldTerrainShape& other) { return *this; };

	};

	class cBtBvhTriangleMeshShape : public iBulletShapes, public iBvhTriangleMeshShape
	{
	public:
		cBtBvhTriangleMeshShape(sModelDrawInfo* modelInfo, btStridingMeshInterface* meshInterface, bool useQuantisizedAABBCompression, bool buildBvh = true);
		virtual ~cBtBvhTriangleMeshShape();

		virtual inline eShapeTypes GetShapeType();
		virtual inline btCollisionShape* GetCollisionShape();

		virtual void GetAABB(const glm::mat4& transform, glm::vec3& minBounds, glm::vec3& maxBounds);

		sModelDrawInfo* _modelInfo;

	protected:
		cBtBvhTriangleMeshShape(const cBtBvhTriangleMeshShape& other) : iBulletShapes(other), iShape(other) {};
		cBtBvhTriangleMeshShape& operator=(const cBtBvhTriangleMeshShape& other) { return *this; };

		

	};
}



#endif // !_iBulletShape_HG_

