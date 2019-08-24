#ifndef _cBulletPhysicsFactory_HG_
#define _cBulletPhysicsFactory_HG_

#include <Interfaces\Physics\iPhysicsFactory.h>

#include "sBtRigidbodyComponent.h"
#include "sBtSoftbodyComponent.h"
#include "sBtCompoundBodyComponent.h"


namespace nPhysics
{
	class cBulletPhysicsFactory : public iPhysicsFactory
	{
	public:
		cBulletPhysicsFactory();
		virtual ~cBulletPhysicsFactory();

		virtual iPhysicsSystem* CreatePhysicsSystem();

		// These will be needed so we know which dll physics type to ask
		// for when getting an allocated memory, since we have two types of dlls.
		virtual std::string GetRigidbodyType() { return sBtRigidbodyComponent::GetStaticType(); };
		virtual std::string GetSoftdbodyType() { return sBtSoftbodyComponent::GetStaticType(); };
		virtual std::string GetCompoundbodyType() { return sBtCompoundBodyComponent::GetStaticType(); };

		virtual iRigidbody* CreateRigidbody(unsigned long long entityID, void* memory = nullptr);
		virtual iRigidbody* CreateRigidbody(sRigidbodyDef& def, unsigned long long entityID, void* memory = nullptr);

		virtual iSoftbody* CreateSoftbody(unsigned long long entityID, void* memory = nullptr);
		virtual iSoftbody* CreateSoftbody(sSoftbodyDef& def, unsigned long long entityID, void* memory = nullptr);

		virtual iCompoundBody* CreateCompoundbody(unsigned long long entityID, void* memory = nullptr);
		virtual iCompoundBody* CreateCompoundbody(sCompoundbodyDef& cbDef, unsigned long long entityID, void* memory = nullptr);

		virtual i3DPersonController* Create3DPersonController(unsigned long long entityID, void* memory = nullptr);
		
		virtual iSphereShape* CreateSphereShape(float radius);
		virtual iPlaneShape* CreatePlaneShape(const glm::vec3& normal, float constant);
		virtual iBoxShape* CreateBoxShape(const glm::vec3& extent);
		virtual iTetrahedronShape* CreateTetrahedronShape(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);

		virtual iCylinderShape* CreateCylinderShape(const glm::vec3& extent);
		virtual iCapsuleShape* CreateCapsuleShape(float radius, float height);

		virtual iHeightfieldTerrainShape* CreateHeightfieldTerrainShape(int heightStickWidth, int heightStickLength, const void* heightfieldData,
			float heightScale, float minHeight, float maxHeight, int upAxis, ePhyScalarTypes hdt, bool flipQuadEdges);
		virtual iBvhTriangleMeshShape* CreateBvhTriangleMeshShape(sModelDrawInfo* modelInfo, bool useQuantisizedAABBCompression, bool buildBvh = true);

	protected:
		cBulletPhysicsFactory(const cBulletPhysicsFactory& other) : iPhysicsFactory(other) {};
		cBulletPhysicsFactory& operator-(const cBulletPhysicsFactory& other) { return *this; }

		sRigidbodyDef currentRbDef;
		sSoftbodyDef baseSbDef;
	};
}



#endif // !_cBulletPhysicsFactory_HG_

