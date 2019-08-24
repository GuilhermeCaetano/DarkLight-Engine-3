// sSimpleRigidbodyComponent.cpp

#include <SimplePhysics\Physics\sSimpleRigidbodyComponent.h>
#include <SimplePhysics\Physics\cPlaneShape.h>
#include <SimplePhysics\Physics\cSphereShape.h>


namespace nPhysics
{
	sSimpleRigidbodyComponent::sSimpleRigidbodyComponent(const sRigidbodyDef& def, unsigned long long entityID)
	{
		this->_rigidbodyType = "SimpleRigidbody";
		this->_parentID = entityID;

		this->_shape = def.collisionShape;

		this->_acceleration = def.acceleration;
		this->_drag = def.drag;
		this->_velocity = def.velocity;

		this->_minMaxAcceleration = glm::vec2(-10.0f, 10.0f);
		this->_minMaxVelocity = glm::vec2(-10.0f, 10.0f);

		this->_angularAcceleration = glm::vec3(0.0f);
		this->_angularVelocity = glm::vec3(0.0f);

		this->_minMaxAngularAcceleration = glm::vec2(-10.0f, 10.0f);
		this->_minMaxAngularVelocity = glm::vec2(-10.0f, 10.0f);

		this->_gravityModifier = glm::vec3(0.0f);
		this->_mass = def.mass;
		this->_invMass = (1.0f / this->_mass);

		this->_bUseGravity = def.bUseGravity;
		this->_bIsKinematic = def.bIsKinematic;

		return;
	}

	sSimpleRigidbodyComponent::~sSimpleRigidbodyComponent()
	{
		return;
	}

	void sSimpleRigidbodyComponent::InitRigidbody()
	{
	}

	void sSimpleRigidbodyComponent::Start()
	{
	}

	std::string sSimpleRigidbodyComponent::GetRigidbodyType()
	{
		return "SimpleRigidbody";
	}

	iShape* sSimpleRigidbodyComponent::GetShape()
	{
		return this->_shape;
	}

	void sSimpleRigidbodyComponent::SetShape(iShape* shape)
	{
		this->_shape = shape;
	}

	sLocationComponent* sSimpleRigidbodyComponent::GetTransformComponent()
	{
		return nullptr;
	}

	void sSimpleRigidbodyComponent::SetTransformComponent(sLocationComponent* locationC)
	{
		this->_transformComponent = locationC;
		this->_transformComponent->GetWorldPosition() = this->_initialPosition;
		this->_transformComponent->GetWorldRotation() = this->_initialRotation;
	}

	eCollisionLayerMask sSimpleRigidbodyComponent::GetCollisionLayerMask()
	{
		return this->_collisionLayerMask;
	}

	void sSimpleRigidbodyComponent::SetCollisionLayerMask(eCollisionLayerMask layerMask)
	{
		this->_collisionLayerMask = layerMask;
	}

	glm::mat4 sSimpleRigidbodyComponent::GetTransform()
	{
		return this->_transform;
	}

	void sSimpleRigidbodyComponent::SetTransform(const glm::mat4& inTransform)
	{
		this->_transform = inTransform;
	}

	void sSimpleRigidbodyComponent::SetInitialPosition(const glm::vec3& position)
	{
		this->_transformComponent->SetWorldPosition(position);
	}

	glm::vec3 sSimpleRigidbodyComponent::GetPosition()
	{
		return this->_transformComponent->GetWorldPosition();
	}

	glm::quat sSimpleRigidbodyComponent::GetRotation()
	{
		return this->_transformComponent->GetWorldRotation();
	}

	void sSimpleRigidbodyComponent::SetPosition(const glm::vec3& position)
	{
		this->_transformComponent->SetWorldPosition(position);
	}

	void sSimpleRigidbodyComponent::SetRotation(const glm::quat& rotation)
	{
		this->_transformComponent->SetWorldRotation(rotation);
	}

	void sSimpleRigidbodyComponent::GetAABB(glm::vec3& minBoundsOut, glm::vec3& maxBoundsOut)
	{
		this->_shape->GetAABB(this->_transform, minBoundsOut, maxBoundsOut);
	}

	glm::vec3 sSimpleRigidbodyComponent::GetShapeOffset()
	{
		return this->_shapeOffset;
	}

	void sSimpleRigidbodyComponent::SetShapeOffset(const glm::vec3& offset)
	{
		this->_shapeOffset = offset;
	}

	eShapeTypes sSimpleRigidbodyComponent::GetShapeType()
	{
		return this->_shapeType;
	}

	void sSimpleRigidbodyComponent::SetShapeType(eShapeTypes shapeType)
	{
		this->_shapeType = shapeType;
	}

	bool sSimpleRigidbodyComponent::GetUseGravity()
	{
		return this->_bUseGravity;
	}

	bool sSimpleRigidbodyComponent::GetIsKinematic()
	{
		return this->_bIsKinematic;
	}

	void sSimpleRigidbodyComponent::SetUseGravity(bool useGravity)
	{
		this->_bUseGravity = useGravity;
	}

	void sSimpleRigidbodyComponent::SetIsKinematic(bool isKinematic)
	{
		this->_bIsKinematic = isKinematic;
	}

	float sSimpleRigidbodyComponent::GetMass()
	{
		return this->_mass;
	}

	float sSimpleRigidbodyComponent::GetDrag()
	{
		return this->_drag;
	}

	float sSimpleRigidbodyComponent::GetBounciness()
	{
		return this->_bounciness;
	}

	void sSimpleRigidbodyComponent::SetMass(float mass)
	{
		this->_mass = mass;
	}

	void sSimpleRigidbodyComponent::SetDrag(float drag)
	{
		this->_drag = drag;
	}

	void sSimpleRigidbodyComponent::SetBounciness(float bounciness)
	{
		this->_bounciness = bounciness;
	}

	void sSimpleRigidbodyComponent::AddForce(const glm::vec3& force, ForceMode mode)
	{
		switch (mode)
		{
		case ForceMode::Force:
			this->_acceleration += force;
			break;
		case ForceMode::Acceleration:
			this->_acceleration += (force * this->_invMass);
			break;
		case ForceMode::Impulse:
			this->_velocity += (force * this->_invMass);
			break;
		case ForceMode::VelocityChange:
			this->_velocity += force;
			break;
		default:
			this->_acceleration += force;
			break;
		}
	}

	void sSimpleRigidbodyComponent::AddRelativeForce(const glm::vec3& force, ForceMode mode)
	{
		switch (mode)
		{
		case ForceMode::Force:
			this->_acceleration += (this->_transformComponent->GetRightVector() * force.x + this->_transformComponent->GetForwardVector() * force.z);
			break;
		case ForceMode::Acceleration:
			this->_acceleration += ((glm::normalize(force) + this->_transformComponent->GetForwardVector()) * this->_invMass);
			break;
		case ForceMode::Impulse:
			this->_velocity += ((force + this->_transformComponent->GetForwardVector()) * this->_invMass);
			break;
		case ForceMode::VelocityChange:
			this->_velocity += (force + this->_transformComponent->GetForwardVector());
			break;
		default:
			this->_acceleration += (force + this->_transformComponent->GetForwardVector());
			break;
		}
	}

	void sSimpleRigidbodyComponent::AddForceAtPosition(const glm::vec3& force, const glm::vec3& position, ForceMode mode)
	{
		glm::vec3 relatDir = glm::normalize(this->_transformComponent->GetWorldPosition() - position);
		switch (mode)
		{
		case ForceMode::Force:
			this->_acceleration += (this->_transformComponent->GetRightVector() * force.x * 10.0f + relatDir * force.z);
			break;
		case ForceMode::Acceleration:
			this->_acceleration += ((force + relatDir) * this->_invMass);
			break;
		case ForceMode::Impulse:
			this->_velocity += ((force + relatDir) * this->_invMass);
			break;
		case ForceMode::VelocityChange:
			this->_velocity += (force + relatDir);
			break;
		default:
			this->_acceleration += (force + relatDir);
			break;
		}
	}

	void sSimpleRigidbodyComponent::AddTorque(const glm::vec3& torque, ForceMode mode)
	{
		switch (mode)
		{
		case ForceMode::Force:
			this->_angularAcceleration += torque;
			break;
		case ForceMode::Acceleration:
			this->_angularAcceleration += (torque * this->_invMass);
			break;
		case ForceMode::Impulse:
			this->_angularVelocity += (torque * this->_invMass);
			break;
		case ForceMode::VelocityChange:
			this->_angularVelocity += torque;
			break;
		default:
			this->_angularAcceleration += torque;
			break;
		}
	}

	void sSimpleRigidbodyComponent::AddRelativeTorque(const glm::vec3& torque, ForceMode mode)
	{
		switch (mode)
		{
		case ForceMode::Force:
			this->_angularAcceleration += (torque + this->_transformComponent->GetForwardVector());
			break;
		case ForceMode::Acceleration:
			this->_angularAcceleration += ((torque + this->_transformComponent->GetForwardVector()) * this->_invMass);
			break;
		case ForceMode::Impulse:
			this->_angularVelocity += ((torque + this->_transformComponent->GetForwardVector()) * this->_invMass);
			break;
		case ForceMode::VelocityChange:
			this->_angularVelocity += (torque + this->_transformComponent->GetForwardVector());
			break;
		default:
			this->_angularAcceleration += (torque + this->_transformComponent->GetForwardVector());
			break;
		}
	}

	glm::vec3 sSimpleRigidbodyComponent::GetAcceleration()
	{
		return this->_acceleration;
	}

	void sSimpleRigidbodyComponent::SetAcceleration(const glm::vec3& acceleration)
	{
		this->_acceleration = acceleration;
	}

	glm::vec3 sSimpleRigidbodyComponent::GetVelocity()
	{
		return this->_velocity;
	}

	void sSimpleRigidbodyComponent::SetVelocity(const glm::vec3& velocity)
	{
		this->_velocity = velocity;
	}

	glm::vec2 sSimpleRigidbodyComponent::GetMinMaxAcceleration()
	{
		return this->_minMaxAcceleration;
	}

	glm::vec2 sSimpleRigidbodyComponent::GetMinMaxVelocity()
	{
		return this->_minMaxVelocity;
	}

	void sSimpleRigidbodyComponent::StopRigidbody()
	{
		this->_acceleration = glm::vec3(0.0f);
		this->_angularAcceleration = glm::vec3(0.0f);
		this->_velocity = glm::vec3(0.0f);
		this->_angularVelocity = glm::vec3(0.0f);
	}

	void sSimpleRigidbodyComponent::_SerializeToJson(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& prettyWriter)
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
		prettyWriter.Key("MinMaxAcceleration"); prettyWriter.StartArray();
		prettyWriter.Double((double)this->_minMaxAcceleration.x); prettyWriter.Double((double)this->_minMaxAcceleration.y);
		prettyWriter.EndArray();
		prettyWriter.Key("MinMaxVelocity"); prettyWriter.StartArray();
		prettyWriter.Double((double)this->_minMaxVelocity.x); prettyWriter.Double((double)this->_minMaxVelocity.y);
		prettyWriter.EndArray();
		prettyWriter.Key("MinMaxAngularAcceleration"); prettyWriter.StartArray();
		prettyWriter.Double((double)this->_minMaxAngularAcceleration.x); prettyWriter.Double((double)this->_minMaxAngularAcceleration.y);
		prettyWriter.EndArray();
		prettyWriter.Key("MinMaxAngularVelocity"); prettyWriter.StartArray();
		prettyWriter.Double((double)this->_minMaxAngularVelocity.x); prettyWriter.Double((double)this->_minMaxAngularVelocity.y);
		prettyWriter.EndArray();
		prettyWriter.Key("GravityModifier"); prettyWriter.StartArray();
		prettyWriter.Double((double)this->_gravityModifier.x); prettyWriter.Double((double)this->_gravityModifier.y); prettyWriter.Double((double)this->_gravityModifier.z);
		prettyWriter.EndArray();
		prettyWriter.Key("Drag"); prettyWriter.Double((double)this->_drag);
		prettyWriter.Key("Mass"); prettyWriter.Double((double)this->_mass);
		prettyWriter.Key("IsKinematic"); prettyWriter.Bool(this->_bIsKinematic);
		prettyWriter.Key("UseGravity"); prettyWriter.Bool(this->_bUseGravity);
	}

	void sSimpleRigidbodyComponent::_DeserializeFromJson(rapidjson::Value::ConstValueIterator vIter)
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

					cPlaneShape* plane = new cPlaneShape(normal, constant);
					this->_shape = plane;
				}
				else if (shapeType == SHAPE_TYPE_SPHERE)
				{
					float radius = (float)(innerIter + 1)->value.GetDouble();

					cSphereShape* sphere = new cSphereShape(radius);
					this->_shape = sphere;
				}
			}
			else if (innerIter->name == "MinMaxAcceleration")
			{
				this->_minMaxAcceleration.x = (float)innerIter->value.GetArray()[0].GetDouble(); this->_minMaxAcceleration.y = (float)innerIter->value.GetArray()[1].GetDouble();
			}
			else if (innerIter->name == "MinMaxVelocity")
			{
				this->_minMaxAngularVelocity.x = (float)innerIter->value.GetArray()[0].GetDouble(); this->_minMaxVelocity.y = (float)innerIter->value.GetArray()[1].GetDouble();
			}
			else if (innerIter->name == "MinMaxAngularAcceleration")
			{
				this->_minMaxAngularAcceleration.x = (float)innerIter->value.GetArray()[0].GetDouble(); this->_minMaxAngularAcceleration.y = (float)innerIter->value.GetArray()[1].GetDouble();
			}
			else if (innerIter->name == "MinMaxAngularVelocity")
			{
				this->_minMaxAngularVelocity.x = (float)innerIter->value.GetArray()[0].GetDouble(); this->_minMaxAngularVelocity.y = (float)innerIter->value.GetArray()[1].GetDouble();
			}
			else if (innerIter->name == "GravityModifier")
			{
				this->_gravityModifier.x = (float)innerIter->value.GetArray()[0].GetDouble(); this->_gravityModifier.y = (float)innerIter->value.GetArray()[1].GetDouble();
				this->_gravityModifier.z = (float)innerIter->value.GetArray()[2].GetDouble();
			}
			else if (innerIter->name == "Drag") { this->_drag = (float)innerIter->value.GetDouble(); }
			else if (innerIter->name == "Mass") { this->_mass = (float)innerIter->value.GetDouble(); }
			else if (innerIter->name == "IsKinematic") { this->_bIsKinematic = innerIter->value.GetBool(); }
			else if (innerIter->name == "UseGravity") { this->_bUseGravity = innerIter->value.GetBool(); }
		}

		this->bodyType = BODY_TYPE_RIGIDBODY;
		this->_invMass = 1.0f / this->_mass;
	}
}
