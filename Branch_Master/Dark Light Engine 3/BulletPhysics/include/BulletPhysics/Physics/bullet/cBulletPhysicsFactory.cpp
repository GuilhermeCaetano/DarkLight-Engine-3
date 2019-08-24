// cBulletPhysicsFactory.cpp

#include <cBulletPhysicsFactory.h>
#include <cBulletPhysicsWorld.h>
#include <sBtRigidbodyComponent.h>
#include <sBtSoftbodyComponent.h>
#include <sBtCompoundBodyComponent.h>
#include <Controllers\bt3DPersonControllerComponent.h>
#include <iBulletShapes.h>

namespace nPhysics
{
	cBulletPhysicsFactory::cBulletPhysicsFactory() : iPhysicsFactory()
	{
		return;
	}

	cBulletPhysicsFactory::~cBulletPhysicsFactory()
	{
		return;
	}

	iPhysicsSystem* cBulletPhysicsFactory::CreatePhysicsSystem()
	{
		cBulletPhysicsWorld* bulletWorld = new cBulletPhysicsWorld();

		if (bulletWorld == nullptr)
		{
			printf("Error creating bullet physics world!\n");
			return nullptr;
		}

		return bulletWorld;
	}

	iRigidbody* cBulletPhysicsFactory::CreateRigidbody(unsigned long long entityID, void* memory)
	{
		sRigidbodyDef def;
		sBtRigidbodyComponent* bulletBody = nullptr;
		if (memory != nullptr)
		{
			bulletBody = ::new (memory) sBtRigidbodyComponent(def, entityID);
		}

		if (bulletBody == nullptr)
		{
			printf("Error creating bullet rigidbody!\n");
			return nullptr;
		}

		return bulletBody;
	}

	iRigidbody* cBulletPhysicsFactory::CreateRigidbody(sRigidbodyDef& def, unsigned long long entityID, void* memory)
	{
		sBtRigidbodyComponent* bulletBody = nullptr;
		if (memory != nullptr)
		{
			bulletBody = ::new (memory) sBtRigidbodyComponent(def, entityID);
		}

		if (bulletBody == nullptr)
		{
			printf("Error creating bullet rigidbody!\n");
			return nullptr;
		}

		return bulletBody;
	}

	iSoftbody* cBulletPhysicsFactory::CreateSoftbody(unsigned long long entityID, void* memory)
	{
		sSoftbodyDef def;
		sBtSoftbodyComponent* bulletBody = nullptr;
		if (memory != nullptr)
		{
			bulletBody = ::new (memory) sBtSoftbodyComponent(def, entityID);
		}

		if (bulletBody == nullptr)
		{
			printf("Error creating bullet softbody!\n");
			return nullptr;
		}

		return bulletBody;
	}

	iSoftbody* cBulletPhysicsFactory::CreateSoftbody(sSoftbodyDef& def, unsigned long long entityID, void* memory)
	{
		sBtSoftbodyComponent* bulletBody = nullptr;
		if (memory != nullptr)
		{
			bulletBody = ::new (memory) sBtSoftbodyComponent(def, entityID);
		}

		if (bulletBody == nullptr)
		{
			printf("Error creating bullet softbody!\n");
			return nullptr;
		}

		return bulletBody;
	}

	iCompoundBody* cBulletPhysicsFactory::CreateCompoundbody(unsigned long long entityID, void* memory)
	{
		sCompoundbodyDef def;
		sBtCompoundBodyComponent* bulletBody = nullptr;
		if (memory != nullptr)
		{
			bulletBody = ::new (memory) sBtCompoundBodyComponent(def, entityID);
		}

		if (bulletBody == nullptr)
		{
			printf("Error creating bullet compoundbody!\n");
			return nullptr;
		}

		return bulletBody;
	}

	iCompoundBody* cBulletPhysicsFactory::CreateCompoundbody(sCompoundbodyDef& def, unsigned long long entityID, void* memory)
	{
		sBtCompoundBodyComponent* bulletBody = nullptr;
		if (memory != nullptr)
		{
			bulletBody = ::new (memory) sBtCompoundBodyComponent(def, entityID);
		}

		if (bulletBody == nullptr)
		{
			printf("Error creating bullet compoundbody!\n");
			return nullptr;
		}

		return bulletBody;
	}

	i3DPersonController* cBulletPhysicsFactory::Create3DPersonController(unsigned long long entityID, void* memory)
	{
		bt3DPersonControllerComponent* bulletBody = nullptr;
		if (memory != nullptr)
		{
			bulletBody = ::new (memory) bt3DPersonControllerComponent(entityID);
		}

		if (bulletBody == nullptr)
		{
			printf("Error creating bullet compoundbody!\n");
			return nullptr;
		}

		return bulletBody;
	}

	iPlaneShape* cBulletPhysicsFactory::CreatePlaneShape(const glm::vec3& normal, float constant)
	{
		cBulletPlaneShape* planeShape = new cBulletPlaneShape(normal, constant);

		if (planeShape == nullptr)
		{
			printf("Error creating plane shape from dll!\n");
			return nullptr;
		}

		return planeShape;
	}

	iSphereShape* cBulletPhysicsFactory::CreateSphereShape(float radius)
	{
		cBulletSphereShape* sphereShape = new cBulletSphereShape(radius);

		if (sphereShape == nullptr)
		{
			printf("Error creating bullet sphere shape from dll!\n");
			return nullptr;
		}

		return sphereShape;
	}

	iBoxShape* cBulletPhysicsFactory::CreateBoxShape(const glm::vec3& extent)
	{
		cBulletBoxShape* boxShape = new cBulletBoxShape(extent);

		if (boxShape == nullptr)
		{
			printf("Error creating bullet box shape from dll!\n");
			return nullptr;
		}

		return boxShape;
	}

	iTetrahedronShape* cBulletPhysicsFactory::CreateTetrahedronShape(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3)
	{
		cBulletTetrahedronShape* tetrahedronShape = new cBulletTetrahedronShape(p0, p1, p2, p3);

		if (tetrahedronShape == nullptr)
		{
			printf("Error creating bullet tetrahedron shape from dll!\n");
			return nullptr;
		}

		return tetrahedronShape;
	}

	iCylinderShape* cBulletPhysicsFactory::CreateCylinderShape(const glm::vec3& extent)
	{
		cBulletCylinderShape* cylinderShape = new cBulletCylinderShape(extent);

		if (cylinderShape == nullptr)
		{
			printf("Error creating bullet cylinder shape from dll!\n");
			return nullptr;
		}

		return cylinderShape;
	}

	iCapsuleShape* cBulletPhysicsFactory::CreateCapsuleShape(float radius, float height)
	{
		cBulletCapsuleShape* capsuleShape = new cBulletCapsuleShape(radius, height);

		if (capsuleShape == nullptr)
		{
			printf("Error creating bullet capsule shape from dll!\n");
			return nullptr;
		}

		return capsuleShape;
	}

	iHeightfieldTerrainShape* cBulletPhysicsFactory::CreateHeightfieldTerrainShape(int heightStickWidth, int heightStickLength, const void* heightfieldData,
		float heightScale, float minHeight, float maxHeight, int upAxis, ePhyScalarTypes hdt, bool flipQuadEdges)
	{
		cBulletHeightfieldTerrainShape* terrainShape = new cBulletHeightfieldTerrainShape(heightStickWidth, heightStickLength, heightfieldData,
			heightScale, minHeight, maxHeight, upAxis, hdt, flipQuadEdges);

		if (terrainShape == nullptr)
		{
			printf("Error creating bullet heightfield terrain shape from dll!\n");
			return nullptr;
		}

		return terrainShape;
	}

	iBvhTriangleMeshShape* cBulletPhysicsFactory::CreateBvhTriangleMeshShape(sModelDrawInfo* modelInfo, bool useQuantisizedAABBCompression, bool buildBvh)
	{
		btTriangleMesh* triangleMesh = new btTriangleMesh();

		btIndexedMesh iMesh;
		typedef unsigned short indexFormat;
		iMesh.m_numTriangles = modelInfo->numberOfTriangles;
		iMesh.m_triangleIndexBase = (const unsigned char*)modelInfo->pIndicesToVBO;
		iMesh.m_triangleIndexStride = 3 * sizeof(unsigned int);
		iMesh.m_numVertices = modelInfo->numberOfVertices;
		iMesh.m_vertexBase = (const unsigned char*)modelInfo->pVerticesToVBO;
		iMesh.m_vertexStride = sizeof(sModel_xyz_rgb_n_uv_bones);
		triangleMesh->addIndexedMesh(iMesh, PHY_ScalarType::PHY_INTEGER);

		/*for (unsigned int triIndex = 0; triIndex < modelInfo->numberOfIndices; triIndex += 3)
		{
			int index1 = triIndex;
			int index2 = triIndex + 1;
			int index3 = triIndex + 2;

			btVector3 vert1 = btVector3(modelInfo->pVerticesToVBO[index1].x, modelInfo->pVerticesToVBO[index1 + 1].y, modelInfo->pVerticesToVBO[index1 + 2].z);
			btVector3 vert2 = btVector3(modelInfo->pVerticesToVBO[index2].x, modelInfo->pVerticesToVBO[index2 + 1].y, modelInfo->pVerticesToVBO[index2 + 2].z);
			btVector3 vert3 = btVector3(modelInfo->pVerticesToVBO[index3].x, modelInfo->pVerticesToVBO[index3 + 1].y, modelInfo->pVerticesToVBO[index3 + 2].z);
			triangleMesh->addTriangle(vert3, vert2, vert1);
		}*/

		/*for (unsigned int triIndex = 0; triIndex < modelInfo->numberOfIndices; triIndex++)
		{
			int index1 = modelInfo->originalMeshInfo->pTrianglesFromFile[triIndex].vertex1;
			int index2 = modelInfo->originalMeshInfo->pTrianglesFromFile[triIndex].vertex2;
			int index3 = modelInfo->originalMeshInfo->pTrianglesFromFile[triIndex].vertex3;

			btVector3 vert1 = btVector3(modelInfo->originalMeshInfo->pVerticesFromFile[index1].x, modelInfo->originalMeshInfo->pVerticesFromFile[index1].y, modelInfo->originalMeshInfo->pVerticesFromFile[index1].z);
			btVector3 vert2 = btVector3(modelInfo->originalMeshInfo->pVerticesFromFile[index2].x, modelInfo->originalMeshInfo->pVerticesFromFile[index2].y, modelInfo->originalMeshInfo->pVerticesFromFile[index2].z);
			btVector3 vert3 = btVector3(modelInfo->originalMeshInfo->pVerticesFromFile[index3].x, modelInfo->originalMeshInfo->pVerticesFromFile[index3].y, modelInfo->originalMeshInfo->pVerticesFromFile[index3].z);
			triangleMesh->addTriangle(vert1, vert2, vert3);
		}*/

		cBtBvhTriangleMeshShape* terrainShape = new cBtBvhTriangleMeshShape(modelInfo, triangleMesh, useQuantisizedAABBCompression, buildBvh);

		if (terrainShape == nullptr)
		{
			printf("Error creating bullet Bvh Triangl Mesh terrain shape from dll!\n");
			return nullptr;
		}

		return terrainShape;
	}
}
