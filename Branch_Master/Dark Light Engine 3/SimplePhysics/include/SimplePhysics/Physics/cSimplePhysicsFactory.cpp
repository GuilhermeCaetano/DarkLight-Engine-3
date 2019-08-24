// cSimplePhysicsFactory.cpp

#include <SimplePhysics\Physics\cSimplePhysicsFactory.h>

#include <SimplePhysics\Physics\cSimplePhysicsSystem.h>
#include <SimplePhysics\Physics\cPlaneShape.h>
#include <SimplePhysics\Physics\cSphereShape.h>


namespace nPhysics
{
	cSimplePhysicsFactory::cSimplePhysicsFactory()
	{
		return;
	}

	cSimplePhysicsFactory::~cSimplePhysicsFactory()
	{
		return;
	}

	iPhysicsSystem* cSimplePhysicsFactory::CreatePhysicsSystem()
	{
		cSimplePhysicsSystem* simplePhySystem = new cSimplePhysicsSystem();

		if (simplePhySystem == nullptr)
		{
			printf("Error creating Simple Physics System!\n");
			return nullptr;
		}

		return simplePhySystem;
	}

	void cSimplePhysicsFactory::GetRigidbodyDef(sRigidbodyDef& def)
	{
		def = this->baseRbDef;
	}

	void cSimplePhysicsFactory::SetRigidbodyDef(const sRigidbodyDef& def)
	{
		this->baseRbDef = def;
	}

	void cSimplePhysicsFactory::GetSoftbodyDef(sSoftbodyDef& def)
	{
		def = this->baseSbDef;
	}

	void cSimplePhysicsFactory::SetSoftbodyDef(const sSoftbodyDef& def)
	{
		this->baseSbDef = def;
	}

	iRigidbody* cSimplePhysicsFactory::CreateRigidbody(unsigned long long entityID, void* memory)
	{
		iShape* shape = this->CreateSphereShape(2.5f);
		sRigidbodyDef def;
		def.collisionShape = shape;

		iRigidbody* simpleBody = this->CreateRigidbody(def, entityID, memory);
		if (simpleBody == nullptr)
		{
			printf("Error creating simple rigidbody!\n");
			return nullptr;
		}

		return simpleBody;
	}

	iRigidbody* cSimplePhysicsFactory::CreateRigidbody(sRigidbodyDef& def, unsigned long long entityID, void* memory)
	{
		sSimpleRigidbodyComponent* simpleBody = nullptr;

		if (memory != nullptr)
		{
			simpleBody = ::new (memory) sSimpleRigidbodyComponent(def, entityID);
		}

		if (simpleBody == nullptr)
		{
			printf("Error creating bullet rigidbody!\n");
			return nullptr;
		}

		return simpleBody;
	}

	iSoftbody* cSimplePhysicsFactory::CreateSoftbody(unsigned long long entityID, void* memory)
	{
		sSoftbodyDef def;
		iSoftbody* simpleBody = this->CreateSoftbody(def, entityID, memory);

		return simpleBody;
	}

	iSoftbody* cSimplePhysicsFactory::CreateSoftbody(sSoftbodyDef& def, unsigned long long entityID, void* memory)
	{
		sSimpleSoftBodyComponent* simpleBody = nullptr;
		
		if (memory != nullptr)
		{
			simpleBody = ::new (memory) sSimpleSoftBodyComponent(def, entityID);
		}

		if (simpleBody == nullptr)
		{
			printf("Error creating bullet softbody!\n");
			return nullptr;
		}

		return simpleBody;
	}

	iCompoundBody* cSimplePhysicsFactory::CreateWreckingBall()
	{
		return nullptr;
	}

	iCompoundBody* cSimplePhysicsFactory::CreateFixedObject()
	{
		return nullptr;
	}

	iCompoundBody* cSimplePhysicsFactory::CreateSliderObject()
	{
		return nullptr;
	}

	iCompoundBody* cSimplePhysicsFactory::CreateHingeObject()
	{
		return nullptr;
	}

	iPlaneShape* cSimplePhysicsFactory::CreatePlaneShape(const glm::vec3& normal, float constant)
	{
		cPlaneShape* plane = new cPlaneShape(normal, constant);

		if (plane == nullptr)
		{
			printf("Error creating Plane Shape!\n");
			return nullptr;
		}

		return plane;
	}

	iSphereShape* cSimplePhysicsFactory::CreateSphereShape(float radius)
	{
		cSphereShape* sphere = new cSphereShape(radius);

		if (sphere == nullptr)
		{
			printf("Error creating Sphere Shape!\n");
			return nullptr;
		}

		return sphere;
	}
}
