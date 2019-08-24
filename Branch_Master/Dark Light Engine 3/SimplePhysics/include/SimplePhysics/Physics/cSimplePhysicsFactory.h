#ifndef _cSimplePhysicsFactory_HG_
#define _cSimplePhysicsFactory_HG_

#include <Interfaces\Physics\iPhysicsFactory.h>
#include <Interfaces\Physics\iRigidbody.h>
#include <Interfaces\Physics\iSoftbody.h>
#include <Interfaces\Physics\iCompoundBody.h>
#include <Interfaces\Physics\sRigidbodyDef.h>
#include <Interfaces\Physics\sSoftbodyDef.h>
#include <Interfaces\Physics\sCompoundbodyDef.h>
#include "sSimpleRigidbodyComponent.h"
#include "sSimpleSoftBodyComponent.h"
#include <string>


//TODO: Create functions that are null for now.
namespace nPhysics
{
	class cSimplePhysicsFactory : public iPhysicsFactory
	{
	public:
		cSimplePhysicsFactory();
		virtual ~cSimplePhysicsFactory();

		virtual iPhysicsSystem* CreatePhysicsSystem();

		// These will be needed so we know which dll physics type to ask
		// for when getting an allocated memory, since we have two types of dlls.
		virtual std::string GetRigidbodyType() { return sSimpleRigidbodyComponent::GetStaticType(); };
		virtual std::string GetSoftdbodyType() { return sSimpleSoftBodyComponent::GetStaticType(); };
		virtual std::string GetCompoundbodyType() { return ""; };

		virtual iRigidbody* CreateRigidbody(unsigned long long entityID, void* memory = nullptr);
		virtual iRigidbody* CreateRigidbody(sRigidbodyDef& rbDef, unsigned long long entityID, void* memory = nullptr);

		virtual iSoftbody* CreateSoftbody(unsigned long long entityID, void* memory = nullptr);
		virtual iSoftbody* CreateSoftbody(sSoftbodyDef& sbDef, unsigned long long entityID, void* memory = nullptr);

		virtual iCompoundBody* CreateCompoundbody(unsigned long long entityID, void* memory = nullptr) { return nullptr; };
		virtual iCompoundBody* CreateCompoundbody(sCompoundbodyDef& cbDef, unsigned long long entityID, void* memory = nullptr) { return nullptr; };

		virtual i3DPersonController* Create3DPersonController(unsigned long long entityID, void* memory = nullptr) { return nullptr; };

		virtual iCompoundBody* CreateWreckingBall();
		virtual iCompoundBody* CreateFixedObject();
		virtual iCompoundBody* CreateSliderObject();
		virtual iCompoundBody* CreateHingeObject();

		virtual iSphereShape* CreateSphereShape(float radius);
		virtual iPlaneShape* CreatePlaneShape(const glm::vec3& normal, float constant);
		virtual iBoxShape* CreateBoxShape(const glm::vec3& extent) { return nullptr; };
		virtual iTetrahedronShape* CreateTetrahedronShape(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3) { return nullptr; };

		virtual iCylinderShape* CreateCylinderShape(const glm::vec3& extent) { return nullptr; };
		virtual iCapsuleShape* CreateCapsuleShape(float radius, float height) { return nullptr; };

		virtual iHeightfieldTerrainShape* CreateHeightfieldTerrainShape(int heightStickWidth, int heightStickLength, const void* heightfieldData,
			float heightScale, float minHeight, float maxHeight, int upAxis, ePhyScalarTypes hdt, bool flipQuadEdges) { return nullptr; };
		virtual iBvhTriangleMeshShape* CreateBvhTriangleMeshShape(sModelDrawInfo* modelInfo, bool useQuantisizedAABBCompression, bool buildBvh = true) { return nullptr; };
		
		virtual void GetRigidbodyDef(sRigidbodyDef& def);
		virtual void SetRigidbodyDef(const sRigidbodyDef& def);
		virtual void GetSoftbodyDef(sSoftbodyDef& def);
		virtual void SetSoftbodyDef(const sSoftbodyDef& def);

	protected:
		cSimplePhysicsFactory(const cSimplePhysicsFactory& other) : iPhysicsFactory(other) {};
		cSimplePhysicsFactory& operator=(const cSimplePhysicsFactory& other) { return *this; }

		sRigidbodyDef baseRbDef;
		sSoftbodyDef baseSbDef;

	};
}


#endif // !_cSimplePhysicsFactory_HG_
