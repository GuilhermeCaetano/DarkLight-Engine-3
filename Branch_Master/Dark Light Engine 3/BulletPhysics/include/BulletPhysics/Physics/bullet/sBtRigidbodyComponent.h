#ifndef _sBtRigidbodyComponent_HG_
#define _sBtRigidbodyComponent_HG_

#include <Interfaces\Physics\iRigidbody.h>
#include <Interfaces\Physics\sRigidbodyDef.h>
#include <BulletDynamics\Dynamics\btRigidBody.h>
#include <BulletPhysics\Physics\bullet\iBulletShapes.h>
#include <BulletPhysics\Physics\bullet\btBulletDynamicsCommon.h>

struct sLocationComponent;


namespace nPhysics
{
	struct sBtRigidbodyComponent : public iRigidbody
	{
	public:
		sBtRigidbodyComponent(const sRigidbodyDef& def, unsigned long long entityID);
		virtual ~sBtRigidbodyComponent();

		virtual void InitRigidbody();

		// Start method for the component
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

		// Returns the bullet version of the rigidbody.
		btRigidBody* GetBtRigidbody();

		// Manage main entity transform component
		virtual sLocationComponent* GetTransformComponent();
		virtual void SetTransformComponent(sLocationComponent* locationC);

		virtual eCollisionLayerMask GetCollisionLayerMask();
		virtual void SetCollisionLayerMask(eCollisionLayerMask layerMask);

		// Manage rigidbody's transform
		btTransform GetBtTransform();
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
		//virtual bool CheckForGrounded(eLayerMask layer, const glm::vec3& checkPos, const glm::vec3& direction);

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

		// Manage rigidbody's acceleration and velocity directly
		virtual glm::vec3 GetAcceleration();
		virtual void SetAcceleration(const glm::vec3& acceleration);
		virtual glm::vec3 GetVelocity();
		virtual void SetVelocity(const glm::vec3& velocity);

		// Manage rigidbody's freeze stats
		virtual glm::vec3 GetFreezePosition();
		virtual glm::vec3 GetFreezeRotation();

		// Zeroes rigidbody's acceleration and velocitly immediately.
		virtual void StopRigidbody();

		bool _isPickable;
		bool _isEnemy;

		

	protected:
		sBtRigidbodyComponent(const sBtRigidbodyComponent& other) : iRigidbody(other) {};
		sBtRigidbodyComponent& operator-(const sBtRigidbodyComponent& other) { return *this; }

		// Saves and Loads the component in Json format
		virtual void _SerializeToJson(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& prettyWriter);
		virtual void _DeserializeFromJson(rapidjson::Value::ConstValueIterator iter);

	private:
		std::string _rigidbodyType;
		btRigidBody* _btRigidbody;
		sLocationComponent* _transformComponent;
		iBulletShapes* _shape;

		glm::mat4 _transform;
		glm::vec3 _shapeOffset;

		glm::vec3 _freezePosition;
		glm::vec3 _freezeRotation;

		float _mass;
		float _drag;
		float _bounciness;

		bool _bIsKinematic;
		bool _bUseGravity;

		btDefaultMotionState* _motionState;
		
	};
}



#endif // !_cBulletRigidbodyComponent_HG_

