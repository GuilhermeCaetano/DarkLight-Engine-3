#ifndef _iPhysicsFactory_HG_
#define _iPhysicsFactory_HG_

#include "iShape.h"
#include "iPlaneShape.h"
#include "iSphereShape.h"
#include "iBoxShape.h"
#include "iTetrahedronShape.h"
#include "iCylinderShape.h"
#include "iCapsuleShape.h"
#include "iHeightfieldTerrainShape.h"
#include "iBvhTriangleMeshShape.h"
#include "iPhysicsSystem.h"
#include <Interfaces\Physics\iRigidbody.h>
#include <Interfaces\Physics\iSoftbody.h>
#include <Interfaces\Physics\iCompoundBody.h>
#include <Interfaces\Controllers\i3DPersonController.h>
#include <Interfaces\Physics\sRigidbodyDef.h>
#include <Interfaces\Physics\sSoftbodyDef.h>
#include <Interfaces\Physics\sCompoundbodyDef.h>
#include <Mesh\sModelDrawInfo.h>
#include <Mesh\sOriginalMeshInfo.h>
#include <string>


namespace nPhysics
{
	class iPhysicsFactory
	{
	public:
		virtual ~iPhysicsFactory() {};
		virtual iPhysicsSystem* CreatePhysicsSystem() = 0;

		// These will be needed so we know which dll physics type to ask
		// for when getting an allocated memory, since we have two types of dlls.
		virtual std::string GetRigidbodyType() = 0;
		virtual std::string GetSoftdbodyType() = 0;
		virtual std::string GetCompoundbodyType() = 0;

		virtual iRigidbody* CreateRigidbody(unsigned long long entityID, void* memory = nullptr) = 0;
		virtual iRigidbody* CreateRigidbody(sRigidbodyDef& rbDef, unsigned long long entityID, void* memory = nullptr) = 0;

		virtual iSoftbody* CreateSoftbody(unsigned long long entityID, void* memory = nullptr) = 0;
		virtual iSoftbody* CreateSoftbody(sSoftbodyDef& sbDef, unsigned long long entityID, void* memory = nullptr) = 0;

		virtual iCompoundBody* CreateCompoundbody(unsigned long long entityID, void* memory = nullptr) = 0;
		virtual iCompoundBody* CreateCompoundbody(sCompoundbodyDef& cbDef, unsigned long long entityID, void* memory = nullptr) = 0;

		virtual i3DPersonController* Create3DPersonController(unsigned long long entityID, void* memory = nullptr) = 0;

		virtual iSphereShape* CreateSphereShape(float radius) = 0;
		virtual iPlaneShape* CreatePlaneShape(const glm::vec3& normal, float constant) = 0;
		virtual iBoxShape* CreateBoxShape(const glm::vec3& extent) = 0;
		virtual iTetrahedronShape* CreateTetrahedronShape(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3) = 0;

		virtual iCylinderShape* CreateCylinderShape(const glm::vec3& extent) = 0;
		virtual iCapsuleShape* CreateCapsuleShape(float radius, float height) = 0;

		virtual iHeightfieldTerrainShape* CreateHeightfieldTerrainShape(int heightStickWidth, int heightStickLength, const void* heightfieldData,
			float heightScale, float minHeight, float maxHeight, int upAxis, ePhyScalarTypes hdt, bool flipQuadEdges) = 0;
		virtual iBvhTriangleMeshShape* CreateBvhTriangleMeshShape(sModelDrawInfo* modelInfo, bool useQuantisizedAABBCompression, bool buildBvh = true) = 0;

	protected:
		iPhysicsFactory() {};
		iPhysicsFactory(const iPhysicsFactory& other) {};

	};
}


#endif // !_iPhysicsFactory_HG_
