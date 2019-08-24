// cBulletShapes.cpp

#include <iBulletShapes.h>
#include "BulletCollision\CollisionShapes\btHeightfieldTerrainShape.h"
#include "BulletCollision\CollisionShapes\btBvhTriangleMeshShape.h"
#include "btBulletDynamicsCommon.h"
#include <nConvert.h>


namespace nPhysics
{
	iBulletShapes::iBulletShapes() : iShape(eShapeTypes::SHAPE_TYPE_SPHERE)
	{
		return;
	}


	////////////////////////////////// PLANE ////////////////////////////////////
	cBulletPlaneShape::cBulletPlaneShape(const glm::vec3& normal, float constant) : iBulletShapes(), iShape(eShapeTypes::SHAPE_TYPE_PLANE)
	{
		btStaticPlaneShape* planeShape = new btStaticPlaneShape(nConvert::ToBullet(normal), btScalar(constant));
		
		this->_shape = planeShape;
		this->_normal = normal;
		this->_constant = constant;

		return;
	}

	cBulletPlaneShape::~cBulletPlaneShape()
	{
		delete this->_shape;
		this->_shape = nullptr;

		return;
	}

	eShapeTypes cBulletPlaneShape::GetShapeType()
	{
		return eShapeTypes::SHAPE_TYPE_PLANE;
	}

	btCollisionShape* cBulletPlaneShape::GetCollisionShape()
	{
		return this->_shape;
	}

	glm::vec3 cBulletPlaneShape::GetNormal()
	{
		return this->_normal;
	}

	float cBulletPlaneShape::GetConstant()
	{
		return this->_constant;
	}

	void cBulletPlaneShape::GetAABB(const glm::mat4& transform, glm::vec3& minBoundsOut, glm::vec3& maxBoundsOut)
	{
		minBoundsOut.x = std::numeric_limits<float>::min();
		minBoundsOut.y = std::numeric_limits<float>::min();
		minBoundsOut.z = std::numeric_limits<float>::min();
		maxBoundsOut.x = std::numeric_limits<float>::max();
		maxBoundsOut.y = std::numeric_limits<float>::max();
		maxBoundsOut.z = std::numeric_limits<float>::max();
	}

	///////////////////////////////// SPHERE ////////////////////////////////////
	cBulletSphereShape::cBulletSphereShape(float radius) : iBulletShapes(), iShape(eShapeTypes::SHAPE_TYPE_SPHERE)
	{
		btSphereShape* sphereShape = new btSphereShape(btScalar(radius));

		this->_shape = sphereShape;
		this->_radius = radius;

		return;
	}

	cBulletSphereShape::~cBulletSphereShape()
	{
		delete this->_shape;
		this->_shape = nullptr;

		return;
	}

	eShapeTypes cBulletSphereShape::GetShapeType()
	{
		return eShapeTypes::SHAPE_TYPE_SPHERE;
	}

	btCollisionShape* cBulletSphereShape::GetCollisionShape()
	{
		return this->_shape;
	}

	float cBulletSphereShape::GetRadius()
	{
		return this->_radius;
	}
	void cBulletSphereShape::GetAABB(const glm::mat4& transform, glm::vec3& minBoundsOut, glm::vec3& maxBoundsOut)
	{
		glm::vec3 pos = transform[3];
		minBoundsOut.x = pos.x - this->_radius;
		minBoundsOut.y = pos.y - this->_radius;
		minBoundsOut.z = pos.z - this->_radius;
		maxBoundsOut.x = pos.x + this->_radius;
		maxBoundsOut.y = pos.y + this->_radius;
		maxBoundsOut.z = pos.z + this->_radius;
	}

	///////////////////////////////// CUBE ////////////////////////////////////
	cBulletBoxShape::cBulletBoxShape(const glm::vec3& extent) : iBulletShapes(), iShape(eShapeTypes::SHAPE_TYPE_BOX)
	{
		btBoxShape* boxShape = new btBoxShape(nConvert::ToBullet(glm::vec3(extent.x * 0.5f, extent.y * 0.5f, extent.z * 0.5f)));

		this->_shape = boxShape;
		this->_extent = extent;

		return;
	}

	cBulletBoxShape::~cBulletBoxShape()
	{
		delete this->_shape;
		this->_shape = nullptr;

		return;
	}

	eShapeTypes cBulletBoxShape::GetShapeType()
	{
		return eShapeTypes::SHAPE_TYPE_BOX;
	}

	btCollisionShape* cBulletBoxShape::GetCollisionShape()
	{
		return this->_shape;
	}

	glm::vec3 cBulletBoxShape::GetBoxExtents()
	{
		return this->_extent;
	}

	void cBulletBoxShape::GetAABB(const glm::mat4& transform, glm::vec3& minBoundsOut, glm::vec3& maxBoundsOut)
	{
		glm::vec3 pos = transform[3];

		minBoundsOut.x = pos.x - this->_extent.x * 0.5f;
		minBoundsOut.y = pos.y - this->_extent.y * 0.5f;
		minBoundsOut.z = pos.z - this->_extent.z * 0.5f;
		maxBoundsOut.x = pos.x + this->_extent.x * 0.5f;
		maxBoundsOut.y = pos.y + this->_extent.y * 0.5f;
		maxBoundsOut.z = pos.z + this->_extent.z * 0.5f;
	}

	///////////////////////////////// TETRAHEDRON ////////////////////////////////////
	cBulletTetrahedronShape::cBulletTetrahedronShape(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3) : iBulletShapes(), iShape(eShapeTypes::SHAPE_TYPE_TETRAHEDRON)
	{
		btBU_Simplex1to4* tetrahedronShape = new btBU_Simplex1to4(nConvert::ToBullet(p0), nConvert::ToBullet(p1), nConvert::ToBullet(p2), nConvert::ToBullet(p3));

		this->_shape = tetrahedronShape;
		this->vecPoints.push_back(p0);
		this->vecPoints.push_back(p1);
		this->vecPoints.push_back(p2);
		this->vecPoints.push_back(p3);

		return;
	}

	cBulletTetrahedronShape::~cBulletTetrahedronShape()
	{
		delete this->_shape;
		this->_shape = nullptr;

		return;
	}

	eShapeTypes cBulletTetrahedronShape::GetShapeType()
	{
		return eShapeTypes::SHAPE_TYPE_TETRAHEDRON;
	}

	btCollisionShape* cBulletTetrahedronShape::GetCollisionShape()
	{
		return this->_shape;
	}

	glm::vec3 cBulletTetrahedronShape::GetTetrahedronPoint(int index)
	{
		return this->vecPoints[index];
	}

	void cBulletTetrahedronShape::GetAABB(const glm::mat4 & transform, glm::vec3 & minBounds, glm::vec3 & maxBounds)
	{

	}

	///////////////////////////////// CYLINDER ////////////////////////////////////
	cBulletCylinderShape::cBulletCylinderShape(const glm::vec3& extent) : iBulletShapes(), iShape(eShapeTypes::SHAPE_TYPE_CYLINDER)
	{
		btCylinderShape* cylinderShape = new btCylinderShape(nConvert::ToBullet(glm::vec3(extent.x * 0.5f, extent.y * 0.5f, extent.z * 0.5f)));

		this->_shape = cylinderShape;
		this->_extent = extent;

		return;
	}

	cBulletCylinderShape::~cBulletCylinderShape()
	{
		delete this->_shape;
		this->_shape = nullptr;

		return;
	}

	eShapeTypes cBulletCylinderShape::GetShapeType()
	{
		return eShapeTypes::SHAPE_TYPE_CYLINDER;
	}

	btCollisionShape* cBulletCylinderShape::GetCollisionShape()
	{
		return this->_shape;
	}

	glm::vec3 cBulletCylinderShape::GetCylinderExtents()
	{
		return this->_extent;
	}

	void cBulletCylinderShape::GetAABB(const glm::mat4& transform, glm::vec3& minBoundsOut, glm::vec3& maxBoundsOut)
	{
		glm::vec4 pos = transform[3];
		minBoundsOut.x = pos.x - this->_extent.x * 0.5f;
		minBoundsOut.y = pos.y - this->_extent.y * 0.5f;
		minBoundsOut.z = pos.z - this->_extent.z * 0.5f;

		maxBoundsOut.x = pos.x + this->_extent.x * 0.5f;
		maxBoundsOut.y = pos.y + this->_extent.y * 0.5f;
		maxBoundsOut.z = pos.z + this->_extent.z * 0.5f;
	}

	///////////////////////////////// CAPSULE ////////////////////////////////////
	cBulletCapsuleShape::cBulletCapsuleShape(float radius, float height) : iBulletShapes(), iShape(eShapeTypes::SHAPE_TYPE_CAPSULE)
	{
		btCapsuleShape* capsuleShape = new btCapsuleShape(btScalar(radius), btScalar(height - (radius * 2.0f)));

		this->_shape = capsuleShape;

		glm::vec3 extent = glm::vec3(radius * 2.0f, height, radius * 2.0f);
		this->_extent = extent;

		return;
	}

	cBulletCapsuleShape::~cBulletCapsuleShape()
	{
		delete this->_shape;
		this->_shape = nullptr;

		return;
	}

	eShapeTypes cBulletCapsuleShape::GetShapeType()
	{
		return eShapeTypes::SHAPE_TYPE_CAPSULE;
	}

	btCollisionShape* cBulletCapsuleShape::GetCollisionShape()
	{
		return this->_shape;
	}

	glm::vec3 cBulletCapsuleShape::GetCylinderExtents()
	{
		return this->_extent;
	}

	void cBulletCapsuleShape::GetAABB(const glm::mat4& transform, glm::vec3& minBoundsOut, glm::vec3& maxBoundsOut)
	{
		glm::vec4 pos = transform[3];
		minBoundsOut.x = pos.x - this->_extent.x * 0.5f;
		minBoundsOut.y = pos.y - this->_extent.y * 0.5f;
		minBoundsOut.z = pos.z - this->_extent.z * 0.5f;

		maxBoundsOut.x = pos.x + this->_extent.x * 0.5f;
		maxBoundsOut.y = pos.y + this->_extent.y * 0.5f;
		maxBoundsOut.z = pos.z + this->_extent.z * 0.5f;
	}

	///////////////////////////////// HEIGHTFIELD TERRAIN ////////////////////////////////////
	cBulletHeightfieldTerrainShape::cBulletHeightfieldTerrainShape(int heightStickWidth, int heightStickLength, const void* heightfieldData,
		float heightScale, float minHeight, float maxHeight, int upAxis, ePhyScalarTypes hdt, bool flipQuadEdges)
	{
		PHY_ScalarType phyType = PHY_ScalarType::PHY_INTEGER;

		switch (hdt)
		{
		case ePhyScalarTypes::PHY_FLOAT:
			phyType = PHY_ScalarType::PHY_FLOAT;
			break;
		case ePhyScalarTypes::PHY_DOUBLE:
			phyType = PHY_ScalarType::PHY_DOUBLE;
			break;
		case ePhyScalarTypes::PHY_INTEGER:
			phyType = PHY_ScalarType::PHY_INTEGER;
			break;
		case ePhyScalarTypes::PHY_SHORT:
			phyType = PHY_ScalarType::PHY_SHORT;
			break;
		case ePhyScalarTypes::PHY_FIXEDPOINT88:
			phyType = PHY_ScalarType::PHY_FIXEDPOINT88;
			break;
		case ePhyScalarTypes::PHY_UCHAR:
			phyType = PHY_ScalarType::PHY_UCHAR;
			break;
		}

		btHeightfieldTerrainShape* terrainShape = new btHeightfieldTerrainShape(heightStickWidth, heightStickLength, heightfieldData, btScalar(heightScale),
			btScalar(minHeight), btScalar(maxHeight), upAxis, phyType, flipQuadEdges);
		//void* rawterrainData = heightfield

		this->_shape = terrainShape;
		terrainShape->setUseDiamondSubdivision(true);
	}

	cBulletHeightfieldTerrainShape::~cBulletHeightfieldTerrainShape()
	{
		delete this->_shape;
		this->_shape = nullptr;

		return;
	}

	eShapeTypes cBulletHeightfieldTerrainShape::GetShapeType()
	{
		return eShapeTypes::SHAPE_TYPE_HEIGHTFIELD_TERRAIN;
	}

	btCollisionShape* cBulletHeightfieldTerrainShape::GetCollisionShape()
	{
		return this->_shape;
	}

	void cBulletHeightfieldTerrainShape::GetAABB(const glm::mat4& transform, glm::vec3& minBounds, glm::vec3& maxBounds)
	{
	}

	///////////////////////////////// BVH TRIANGLE MESH TERRAIN ////////////////////////////////////
	cBtBvhTriangleMeshShape::cBtBvhTriangleMeshShape(sModelDrawInfo* modelInfo, btStridingMeshInterface* meshInterface, bool useQuantisizedAABBCompression, bool buildBvh)
	{
		btBvhTriangleMeshShape* terrainShape = new btBvhTriangleMeshShape(meshInterface, useQuantisizedAABBCompression, buildBvh);
		//terrainShape->setMargin(0.04f);
		this->_shape = terrainShape;
		this->_modelInfo = modelInfo;
	}

	cBtBvhTriangleMeshShape::~cBtBvhTriangleMeshShape()
	{
		delete this->_shape;
		this->_shape = nullptr;

		return;
	}

	eShapeTypes cBtBvhTriangleMeshShape::GetShapeType()
	{
		return eShapeTypes::SHAPE_TYPE_BVH_TRIANGLE_MESH;
	}

	btCollisionShape* cBtBvhTriangleMeshShape::GetCollisionShape()
	{
		return this->_shape;
	}

	void cBtBvhTriangleMeshShape::GetAABB(const glm::mat4& transform, glm::vec3& minBounds, glm::vec3& maxBounds)
	{
	}
}
