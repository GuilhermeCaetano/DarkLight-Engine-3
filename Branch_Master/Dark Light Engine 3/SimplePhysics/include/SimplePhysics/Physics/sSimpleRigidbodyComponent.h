#ifndef _sSimpleRigidbodyComponent_HG_
#define _sSimpleRigidbodyComponent_HG_

#include <OpenGL\OpenGLHeaders.h>
#include <Interfaces\Physics\iRigidbody.h>
#include <Interfaces\Physics\sRigidbodyDef.h>
#include <Interfaces\Physics\iShape.h>


namespace nPhysics
{
	struct sSimpleRigidbodyComponent : public iRigidbody
	{
	public:
		sSimpleRigidbodyComponent(const sRigidbodyDef& def, unsigned long long entityID);
		~sSimpleRigidbodyComponent();

		virtual void InitRigidbody();

		virtual void Start();

		// This must be overriden on the derived components, otherwise it will return an error!
		static std::string GetStaticType() { return "RIGIDBODY_COMPONENT"; };

		// Returns the type of the rigidbody.
		virtual std::string GetRigidbodyType();

		// Manage rigidbody's collision shape
		virtual iShape* GetShape();
		virtual void SetShape(iShape* shape);

		// Manage rigidbody's shape offset.
		virtual glm::vec3 GetShapeOffset();
		virtual void SetShapeOffset(const glm::vec3& offset);

		virtual eShapeTypes GetShapeType();
		virtual void SetShapeType(eShapeTypes shapeType);

		// Manage main entity transform component
		virtual sLocationComponent* GetTransformComponent();
		virtual void SetTransformComponent(sLocationComponent* locationC);

		virtual eCollisionLayerMask GetCollisionLayerMask();
		virtual void SetCollisionLayerMask(eCollisionLayerMask layerMask);

		// Manage rigidbody's transform
		virtual glm::mat4 GetTransform();
		virtual void SetTransform(const glm::mat4& inTransform);

		// Sets initial position for the rigidbody on the world.
		virtual void SetInitialPosition(const glm::vec3& position);

		// Manage rigidbody's direct position and rotation.
		virtual glm::vec3 GetPosition();
		virtual glm::quat GetRotation();
		virtual void SetPosition(const glm::vec3& position);
		virtual void SetRotation(const glm::quat& rotation);

		// Returns the minimum and maximum bounds of the collision shape.
		virtual void GetAABB(glm::vec3& minBoundsOut, glm::vec3& maxBoundsOut);

		// Manage rigidbody's kinematic and gravity properties
		virtual bool GetUseGravity();
		virtual bool GetIsKinematic();
		virtual void SetUseGravity(bool useGravity);
		virtual void SetIsKinematic(bool isKinematic);

		// Manage rigidbody's other parameters
		virtual float GetMass();
		virtual float GetDrag();
		virtual float GetBounciness();
		virtual void SetMass(float mass);
		virtual void SetDrag(float drag);
		virtual void SetBounciness(float bounciness);

		// Adds acceleration to the rigidbody:
		// Either directly, relative to a vector, or at a certain position.
		virtual void AddForce(const glm::vec3& force, ForceMode mode);
		virtual void AddRelativeForce(const glm::vec3& force, ForceMode mode);
		virtual void AddForceAtPosition(const glm::vec3& force, const glm::vec3& position, ForceMode mode);

		virtual void AddTorque(const glm::vec3& torque, ForceMode mode);
		virtual void AddRelativeTorque(const glm::vec3& torque, ForceMode mode);

		// Manage rigidbody's velocity directly
		virtual glm::vec3 GetAcceleration();
		virtual void SetAcceleration(const glm::vec3& acceleration);
		virtual glm::vec3 GetVelocity();
		virtual void SetVelocity(const glm::vec3& velocity);
		glm::vec2 GetMinMaxAcceleration();
		glm::vec2 GetMinMaxVelocity();

		// Manage rigidbody's freeze stats
		virtual glm::vec3 GetFreezePosition() { return glm::vec3(0.0f); };
		virtual glm::vec3 GetFreezeRotation() { return glm::vec3(0.0f); };;

		// Zeroes rigidbody's acceleration and velocitly immediately.
		virtual void StopRigidbody();

	protected:
		sSimpleRigidbodyComponent(const sSimpleRigidbodyComponent& other) : iRigidbody(other) {};
		sSimpleRigidbodyComponent& operator=(const sSimpleRigidbodyComponent& other) { return *this; };

		virtual void _SerializeToJson(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& prettyWriter);
		virtual void _DeserializeFromJson(rapidjson::Value::ConstValueIterator iter);

		std::string _rigidbodyType;
		sLocationComponent* _transformComponent;
		iShape* _shape;

		glm::mat4 _transform;
		glm::vec3 _shapeOffset;

		float _mass;
		float _invMass;
		float _drag;
		float _bounciness;

		bool _bUseGravity;
		bool _bIsKinematic;

		glm::vec3 _initialPosition;
		glm::quat _initialRotation;

		glm::vec3 _acceleration;
		glm::vec3 _velocity;

		glm::vec2 _minMaxAcceleration;
		glm::vec2 _minMaxVelocity;

		glm::vec3 _angularAcceleration;
		glm::vec3 _angularVelocity;

		glm::vec2 _minMaxAngularAcceleration;
		glm::vec2 _minMaxAngularVelocity;

		glm::vec3 _gravityModifier;

	};
}


#endif // !_sSimpleRigidbodyComponent_HG_
