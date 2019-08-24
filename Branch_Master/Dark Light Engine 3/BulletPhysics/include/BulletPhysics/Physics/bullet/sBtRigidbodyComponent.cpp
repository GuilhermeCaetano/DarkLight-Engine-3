// sBulletRigidbodyComponent.cpp

#include <sBtRigidbodyComponent.h>
#include <Entity\Components\sLocationComponent.h>
#include <nConvert.h>


namespace nPhysics
{
	sBtRigidbodyComponent::sBtRigidbodyComponent(const sRigidbodyDef& def, unsigned long long entityID)
	{
		this->_parentID = entityID;
		this->_rigidbodyType = "Bullet Rigidbody";

		this->_shape = dynamic_cast<iBulletShapes*>(def.collisionShape);
		this->_shapeType = def.collisionShape->GetShapeType();

		btTransform startTransform;
		startTransform.setIdentity();

		btScalar mass(def.mass);
		bool isDynamic = (mass != 0.0f);
		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
		{
			this->_shape->GetCollisionShape()->calculateLocalInertia(mass, localInertia);
		}

		glm::vec3 finalOffset = def.initialRotation * def.centerOfMassOffset;
		startTransform.setOrigin(nConvert::ToBullet(def.initialPosition + finalOffset));
		startTransform.setRotation(nConvert::ToBullet(def.initialRotation));

		this->_motionState = new btDefaultMotionState(startTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, this->_motionState, this->_shape->GetCollisionShape(), localInertia);

		this->_btRigidbody = new btRigidBody(rbInfo);
		this->_btRigidbody->setRestitution(def.bounciness);
		this->_btRigidbody->setUserPointer(this);

		btVector3 linearFactor = btVector3(!def.freezePosition.x, !def.freezePosition.y, !def.freezePosition.z);
		btVector3 angularFactor = btVector3(!def.freezeRotation.x, !def.freezeRotation.y, !def.freezeRotation.z);
		this->_btRigidbody->setLinearFactor(btVector3(linearFactor));
		this->_btRigidbody->setAngularFactor(btVector3(angularFactor));

		this->_btRigidbody->setLinearVelocity(nConvert::ToBullet(glm::vec3(0.0f)));
		this->_bounciness = def.bounciness;
		this->_mass = def.mass;
		this->_shapeOffset = def.centerOfMassOffset;
		this->_drag = def.drag;

		this->_freezePosition = def.freezePosition;
		this->_freezeRotation = def.freezeRotation;

		this->_bIsKinematic = def.bIsKinematic;
		this->_bUseGravity = def.bUseGravity;

		if (this->_bIsKinematic)
		{
			this->_btRigidbody->setCollisionFlags(this->_btRigidbody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
			this->_btRigidbody->setActivationState(DISABLE_DEACTIVATION);
		}

		if (def.collisionShape->GetShapeType() == eShapeTypes::SHAPE_TYPE_BVH_TRIANGLE_MESH)
		{
			this->_btRigidbody->setCollisionFlags(this->_btRigidbody->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
			this->_btRigidbody->setActivationState(DISABLE_DEACTIVATION);
		}

		return;
	}

	sBtRigidbodyComponent::~sBtRigidbodyComponent()
	{
		delete this->_btRigidbody;
		this->_btRigidbody = nullptr;

		delete this->_motionState;
		this->_motionState = nullptr;

		return;
	}

	void sBtRigidbodyComponent::InitRigidbody()
	{
		btTransform startTransform;
		startTransform.setIdentity();

		btScalar mass(this->_mass);
		bool isDynamic = (mass != 0.0f);
		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
		{
			this->_shape->GetCollisionShape()->calculateLocalInertia(mass, localInertia);
		}

		glm::vec3 finalOffset = this->_transformComponent->GetWorldRotation() * this->_shapeOffset;
		startTransform.setOrigin(nConvert::ToBullet(this->_transformComponent->GetWorldPosition() + finalOffset));
		startTransform.setRotation(nConvert::ToBullet(this->_transformComponent->GetWorldRotation()));

		this->_motionState = new btDefaultMotionState(startTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, this->_motionState, this->_shape->GetCollisionShape(), localInertia);

		this->_btRigidbody = new btRigidBody(rbInfo);
		this->_shape->GetCollisionShape()->setUserPointer((void*)this);
		this->_btRigidbody->setRestitution(this->_bounciness);

		this->_btRigidbody->setLinearVelocity(nConvert::ToBullet(glm::vec3(0.0f)));

		this->_btRigidbody->setActivationState(DISABLE_DEACTIVATION);
		if (this->_bIsKinematic)
		{
			this->_btRigidbody->setCollisionFlags(this->_btRigidbody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);

		}

		if (this->_shapeType == eShapeTypes::SHAPE_TYPE_BVH_TRIANGLE_MESH)
		{
			this->_btRigidbody->setCollisionFlags(this->_btRigidbody->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
		}

		return;
	}

	void sBtRigidbodyComponent::Start()
	{
	}

	std::string sBtRigidbodyComponent::GetRigidbodyType()
	{
		return "BulletRigidbody";
	}

	iShape* sBtRigidbodyComponent::GetShape()
	{
		return this->_shape;
	}

	void sBtRigidbodyComponent::SetShape(iShape* newShape)
	{
		eShapeTypes shapeType = newShape->GetShapeType();
		iBulletShapes* btShape = dynamic_cast<iBulletShapes*>(newShape);

		if (shapeType == eShapeTypes::SHAPE_TYPE_BVH_TRIANGLE_MESH)
		{
			cBtBvhTriangleMeshShape* terrainShape = dynamic_cast<cBtBvhTriangleMeshShape*>(newShape);
			this->_shape = terrainShape;
			this->_shapeType = shapeType;
		}

		/*if (shapeType == eShapeTypes::SHAPE_TYPE_SPHERE)
		{
			iBulletShapes* iSphere = dynamic_cast<iBulletShapes*>(newShape);
			this->_shape = iSphere;
		}
		else if (shapeType == eShapeTypes::SHAPE_TYPE_PLANE)
		{
			iPlaneShape* iPlane = dynamic_cast<iPlaneShape*>(newShape);
			cBulletPlaneShape* plane = new cBulletPlaneShape(iPlane->GetNormal(), iPlane->GetConstant());
			delete this->_shape;
			this->_shape = plane;
		}*/
		if(btShape == nullptr)
		{
			printf("Error creating physics shape! Shape is unknown!\n");
			return;
		}

		

		/*this->_btRigidbody->setCollisionShape(this->_shape->GetCollisionShape());
		btVector3 localInertia(0, 0, 0);
		this->_shape->GetCollisionShape()->calculateLocalInertia(btScalar(1.0f), localInertia);
		this->_btRigidbody->setMassProps(btScalar(1.0f), localInertia);
		this->_btRigidbody->updateInertiaTensor();*/
	}

	btRigidBody* sBtRigidbodyComponent::GetBtRigidbody()
	{
		return this->_btRigidbody;
	}

	sLocationComponent* sBtRigidbodyComponent::GetTransformComponent()
	{
		return this->_transformComponent;
	}

	void sBtRigidbodyComponent::SetTransformComponent(sLocationComponent* locationC)
	{
		this->_transformComponent = locationC;
	}

	eCollisionLayerMask sBtRigidbodyComponent::GetCollisionLayerMask()
	{
		return this->_collisionLayerMask;
	}

	void sBtRigidbodyComponent::SetCollisionLayerMask(eCollisionLayerMask layerMask)
	{
		this->_collisionLayerMask = layerMask;
	}

	btTransform sBtRigidbodyComponent::GetBtTransform()
	{
		btTransform bTransform;
		this->_motionState->getWorldTransform(bTransform);
		return bTransform;
	}

	glm::mat4 sBtRigidbodyComponent::GetTransform()
	{
		glm::mat4 outTransform;
		btTransform bTransform;
		this->_motionState->getWorldTransform(bTransform);
		nConvert::ToSimple(bTransform, outTransform);

		return outTransform;
	}

	void sBtRigidbodyComponent::SetTransform(const glm::mat4& inTransform)
	{
		btTransform bTransform;
		nConvert::ToBullet(inTransform);
		this->_motionState->setWorldTransform(bTransform);
	}

	void sBtRigidbodyComponent::SetInitialPosition(const glm::vec3& position)
	{
		btTransform startTransform;
		startTransform.setIdentity();

		startTransform.setOrigin(nConvert::ToBullet(position));

		delete this->_motionState;
		this->_motionState = new btDefaultMotionState(startTransform);

		this->_btRigidbody->setMotionState(this->_motionState);
	}

	glm::vec3 sBtRigidbodyComponent::GetPosition()
	{
		btTransform bTransform;
		this->_motionState->getWorldTransform(bTransform);

		glm::mat4 matTransform;
		nConvert::ToSimple(bTransform, matTransform);

		return matTransform[3];
	}

	glm::quat sBtRigidbodyComponent::GetRotation()
	{
		btTransform bTransform;
		this->_motionState->getWorldTransform(bTransform);

		glm::mat4 matTransform;
		nConvert::ToSimple(bTransform, matTransform);

		return glm::quat(matTransform);
	}

	void sBtRigidbodyComponent::SetPosition(const glm::vec3& position)
	{
		// Get transform from rigidbody and set new translation
		btTransform bTransform;
		this->_motionState->getWorldTransform(bTransform);
		bTransform.setOrigin(nConvert::ToBullet(position));
		this->_motionState->setWorldTransform(bTransform);
	}

	void sBtRigidbodyComponent::SetRotation(const glm::quat& rotation)
	{
		// Get transform from rigidbody and set new rotation
		btTransform bTransform;
		this->_motionState->getWorldTransform(bTransform);
		bTransform.setRotation(nConvert::ToBullet(rotation));
		this->_motionState->setWorldTransform(bTransform);
	}

	void sBtRigidbodyComponent::GetAABB(glm::vec3& minBoundsOut, glm::vec3& maxBoundsOut)
	{
		// Get AABB from the rigidbody shape
		glm::mat4 matTransform = this->GetTransform();
		this->_shape->GetAABB(matTransform, minBoundsOut, maxBoundsOut);
	}

	glm::vec3 sBtRigidbodyComponent::GetShapeOffset()
	{
		return this->_shapeOffset;
	}

	void sBtRigidbodyComponent::SetShapeOffset(const glm::vec3& offset)
	{
		this->_shapeOffset = offset;
	}

	eShapeTypes sBtRigidbodyComponent::GetShapeType()
	{
		return this->_shapeType;
	}

	void sBtRigidbodyComponent::SetShapeType(eShapeTypes shapeType)
	{
		if (this->_shape == nullptr)
		{
			this->_shapeType = shapeType;
		}
	}

	bool sBtRigidbodyComponent::GetUseGravity()
	{
		return this->_bUseGravity;
	}

	bool sBtRigidbodyComponent::GetIsKinematic()
	{
		return this->_bIsKinematic;
	}

	void sBtRigidbodyComponent::SetUseGravity(bool useGravity)
	{
		this->_bUseGravity = useGravity;
		if (!useGravity)
		{
			this->_btRigidbody->setFlags(1);
		}
		else
		{
			this->_btRigidbody->setFlags(8);
		}
	}

	void sBtRigidbodyComponent::SetIsKinematic(bool isKinematic)
	{
		this->_bIsKinematic = isKinematic;
		if (isKinematic)
		{
			
			//this->_rigidbody->setCollisionFlags(1);
		}
		else
		{
			//this->_rigidbody->setCollisionFlags(0);
		}
	}

	float sBtRigidbodyComponent::GetMass()
	{
		return this->_mass;
	}

	float sBtRigidbodyComponent::GetDrag()
	{
		return this->_drag;
	}

	float sBtRigidbodyComponent::GetBounciness()
	{
		return this->_bounciness;
	}

	void sBtRigidbodyComponent::SetMass(float mass)
	{
		this->_mass = mass;
	}

	void sBtRigidbodyComponent::SetDrag(float drag)
	{
		this->_drag = drag;
	}

	void sBtRigidbodyComponent::SetBounciness(float bounciness)
	{
		this->_bounciness = bounciness;
	}

	void sBtRigidbodyComponent::AddForce(const glm::vec3& force, ForceMode mode)
	{
		this->_btRigidbody->activate(true);
		this->_btRigidbody->applyForce(nConvert::ToBullet(force), nConvert::ToBullet(this->_transformComponent->GetWorldPosition()));
	}

	void sBtRigidbodyComponent::AddRelativeForce(const glm::vec3& force, ForceMode mode)
	{
	}

	void sBtRigidbodyComponent::AddForceAtPosition(const glm::vec3& force, const glm::vec3& position, ForceMode mode)
	{
		this->_btRigidbody->activate(true);
		this->_btRigidbody->applyForce(nConvert::ToBullet(force), nConvert::ToBullet(position));
	}

	glm::vec3 sBtRigidbodyComponent::GetAcceleration()
	{
		return glm::vec3(0.0f);
	}

	void sBtRigidbodyComponent::SetAcceleration(const glm::vec3& acceleration)
	{
	}

	glm::vec3 sBtRigidbodyComponent::GetVelocity()
	{
		return nConvert::ToSimple(this->_btRigidbody->getLinearVelocity());
	}

	void sBtRigidbodyComponent::SetVelocity(const glm::vec3& velocity)
	{
		btVector3 btVel = nConvert::ToBullet(velocity);
		btVel.setY(btScalar(this->GetVelocity().y));

		this->_btRigidbody->activate(true);
		this->_btRigidbody->setLinearVelocity(btVel);
	}

	glm::vec3 sBtRigidbodyComponent::GetFreezePosition()
	{
		return this->_freezePosition;
	}

	glm::vec3 sBtRigidbodyComponent::GetFreezeRotation()
	{
		return this->_freezeRotation;
	}

	void sBtRigidbodyComponent::StopRigidbody()
	{
		this->_btRigidbody->setLinearVelocity(nConvert::ToBullet(glm::vec3(0.0f)));
		this->_btRigidbody->setAngularVelocity(nConvert::ToBullet(glm::vec3(0.0f)));
	}

	void sBtRigidbodyComponent::_SerializeToJson(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& prettyWriter)
	{
		prettyWriter.Key("RigidbodyType"); prettyWriter.String(this->GetRigidbodyType().c_str());
		prettyWriter.Key("ShapeType"); prettyWriter.String(this->_shape->EnumToString(this->_shape->GetShapeType()).c_str());
		if (this->_shape->GetShapeType() == SHAPE_TYPE_PLANE)
		{
			iPlaneShape* plane = dynamic_cast<iPlaneShape*>(this->_shape);
			prettyWriter.Key("PlaneConstant"); prettyWriter.Double((double)plane->GetConstant());
			prettyWriter.Key("PlaneNormal"); prettyWriter.StartArray();
			prettyWriter.Double(plane->GetNormal().x); prettyWriter.Double(plane->GetNormal().y); prettyWriter.Double(plane->GetNormal().z);
			prettyWriter.EndArray();
		}
		else if (this->_shape->GetShapeType() == SHAPE_TYPE_SPHERE)
		{
			iSphereShape* sphere = dynamic_cast<iSphereShape*>(this->_shape);
			prettyWriter.Key("Radius"); prettyWriter.Double((double)sphere->GetRadius());
		}
		else if (this->_shape->GetShapeType() == SHAPE_TYPE_BOX)
		{
			iBoxShape* box = dynamic_cast<iBoxShape*>(this->_shape);
			prettyWriter.Key("Extent"); prettyWriter.Double((double)box->GetBoxExtents().x); 
		}
		/*else if (this->_shape->GetShapeType() == SHAPE_TYPE_TETRAHEDRON)
		{
			iTetrahedronShape* tetrahedron = dynamic_cast<iTetrahedronShape*>(this->_shape);
			prettyWriter.Key("Extent"); prettyWriter.Double((double)tetrahedron->GetTetrahedronPoint(0).x);
		}*/
		else if (this->_shape->GetShapeType() == SHAPE_TYPE_CYLINDER)
		{
			iCylinderShape* cylinder = dynamic_cast<iCylinderShape*>(this->_shape);
			prettyWriter.Key("Extent"); prettyWriter.StartArray();
			prettyWriter.Double((double)cylinder->GetCylinderExtents().x);
			prettyWriter.Double((double)cylinder->GetCylinderExtents().y);
			prettyWriter.Double((double)cylinder->GetCylinderExtents().z);
			prettyWriter.EndArray();
		}
		/*else if (this->_shape->GetShapeType() == SHAPE_TYPE_CAPSULE)
		{
			iCapsuleShape* capsule = dynamic_cast<iCapsuleShape*>(this->_shape);
			prettyWriter.Key("Radius"); prettyWriter.Double((double)capsule->GetCylinderExtents());
			
			prettyWriter.Double((double)cylinder->GetCylinderExtents().y);
			prettyWriter.Double((double)cylinder->GetCylinderExtents().z);
			prettyWriter.EndArray();
		}*/
		else if (this->_shape->GetShapeType() == SHAPE_TYPE_BVH_TRIANGLE_MESH)
		{

		}

		prettyWriter.Key("ShapeOffset"); prettyWriter.StartArray();
		prettyWriter.Double((double)this->_shapeOffset.x); prettyWriter.Double((double)this->_shapeOffset.y); prettyWriter.Double((double)this->_shapeOffset.z);
		prettyWriter.EndArray();
		prettyWriter.Key("Bounciness"); prettyWriter.Double((double)this->_bounciness);
		prettyWriter.Key("Drag"); prettyWriter.Double((double)this->_drag);
		prettyWriter.Key("Mass"); prettyWriter.Double((double)this->_mass);
		prettyWriter.Key("IsKinematic"); prettyWriter.Bool(this->_bIsKinematic);
		prettyWriter.Key("UseGravity"); prettyWriter.Bool(this->_bUseGravity);
		prettyWriter.Key("IsPickable"); prettyWriter.Bool(this->_isPickable);
	}

	void sBtRigidbodyComponent::_DeserializeFromJson(rapidjson::Value::ConstValueIterator vIter)
	{
		for (rapidjson::Value::ConstMemberIterator innerIter = vIter->MemberBegin(); innerIter != vIter->MemberEnd(); innerIter++)
		{
			if (innerIter->name == "RigidbodyType") { this->_rigidbodyType = innerIter->value.GetString(); }
			else if (innerIter->name == "ShapeType")
			{
				eShapeTypes shapeType = iShape::StringToShapeEnum(innerIter->value.GetString());
				if (shapeType == SHAPE_TYPE_PLANE)
				{
					float constant = (float)(innerIter + 1)->value.GetDouble();
					glm::vec3 normal;
					normal.x = (float)(innerIter + 2)->value.GetArray()[0].GetDouble();
					normal.y = (float)(innerIter + 2)->value.GetArray()[1].GetDouble();
					normal.z = (float)(innerIter + 2)->value.GetArray()[2].GetDouble();

					cBulletPlaneShape* plane = new cBulletPlaneShape(normal, constant);
					this->_shape = plane;
					this->_shapeType = SHAPE_TYPE_PLANE;
				}
				else if (shapeType == SHAPE_TYPE_SPHERE)
				{
					float radius = (float)(innerIter + 1)->value.GetDouble();

					cBulletSphereShape* sphere = new cBulletSphereShape(radius);
					this->_shape = sphere;
				}
				else if (shapeType == SHAPE_TYPE_BOX)
				{
					float extent = (float)(innerIter + 1)->value.GetDouble();

					cBulletBoxShape* box = new cBulletBoxShape(glm::vec3(extent, extent, extent));
					this->_shape = box;
					this->_shapeType = SHAPE_TYPE_BOX;
				}
				/*else if (shapeType == SHAPE_TYPE_TETRAHEDRON)
				{
					float extentScalar = (float)(innerIter + 1)->value.GetDouble();

					glm::vec3 p0 = this->_transformComponent->position - glm::vec3(extentScalar, extentScalar, extentScalar);
					glm::vec3 p1 = this->_transformComponent->position + glm::vec3(extentScalar, -extentScalar, -extentScalar);
					glm::vec3 p2 = this->_transformComponent->position + glm::vec3(0.0f, -extentScalar, extentScalar);
					glm::vec3 p3 = this->_transformComponent->position + glm::vec3(0.0f, extentScalar, 0.0f);
					cBulletTetrahedronShape* sphere = new cBulletTetrahedronShape(p0, p1, p2, p3);
					this->_shape = sphere;
					this->_shapeType = SHAPE_TYPE_TETRAHEDRON;
				}*/
				else if (shapeType == SHAPE_TYPE_CYLINDER)
				{
					rapidjson::Value::ConstArray arr = (innerIter + 1)->value.GetArray();
					glm::vec3 extent = glm::vec3((float)arr[0].GetDouble(), (float)arr[1].GetDouble(), (float)arr[2].GetDouble());

					cBulletCylinderShape* cylinder = new cBulletCylinderShape(extent);
					this->_shape = cylinder;
					this->_shapeType = SHAPE_TYPE_CYLINDER;
				}
				//else if (shapeType == SHAPE_TYPE_CAPSULE)
				//{
				//	rapidjson::Value::ConstArray arr = (innerIter + 1)->value.GetArray();
				//	glm::vec3 extent = glm::vec3((float)arr[0].GetDouble(), (float)arr[1].GetDouble(), (float)arr[2].GetDouble());

				//	/*cBulletCapsuleShape* cylinder = new cBulletCapsuleShape(extent);
				//	this->_shape = cylinder;
				//	this->_shapeType = SHAPE_TYPE_CYLINDER;*/
				//}
				else if (shapeType == SHAPE_TYPE_BVH_TRIANGLE_MESH)
				{
					this->_shapeType = SHAPE_TYPE_BVH_TRIANGLE_MESH;
				}
			}
			else if (innerIter->name == "ShapeOffset") 
			{
				rapidjson::Value::ConstArray arr = innerIter->value.GetArray();
				this->_shapeOffset = glm::vec3((float)arr[0].GetDouble(), (float)arr[1].GetDouble(), (float)arr[2].GetDouble());
			}
			else if (innerIter->name == "Bounciness") { this->_bounciness = (float)innerIter->value.GetDouble(); }
			else if (innerIter->name == "Drag") { this->_drag = (float)innerIter->value.GetDouble(); }
			else if (innerIter->name == "Mass") { this->_mass = (float)innerIter->value.GetDouble(); }
			else if (innerIter->name == "IsKinematic") { this->_bIsKinematic = innerIter->value.GetBool(); }
			else if (innerIter->name == "UseGravity") { this->_bUseGravity = innerIter->value.GetBool(); }
			else if (innerIter->name == "IsPickable") { this->_isPickable = innerIter->value.GetBool(); }
		}

		if (this->_shape != nullptr)
		{
			this->InitRigidbody();
		}
	}
}
