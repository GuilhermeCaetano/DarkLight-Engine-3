#ifndef _iCollisionBody_HG_
#define _iCollisionBody_HG_

#include <Entity\Components\iComponent.h>
#include <Entity\Components\sLocationComponent.h>
#include <OpenGL\OpenGLHeaders.h>
#include <Interfaces\Collisions\eCollisionLayerMask.h>

// closestrayresultcallback
// hit normal

// Bullet kinematic controller
// step up and down
// btkinematicClosestNotMeConvexResultCallback

namespace nPhysics
{
	// Types of collision body
	enum eBodyType
	{
		BODY_TYPE_RIGIDBODY,
		BODY_TYPE_SOFTBODY,
		BODY_TYPE_COMPOUNDBODY,
		BODY_TYPE_GHOSTBODY
	};

	struct iCollisionBody : public iComponent
	{
	public:
		virtual ~iCollisionBody() {};

		// Get an AABB based on position calculation
		virtual void GetAABB(glm::vec3& minBoundsOut, glm::vec3& maxBoundsOut) = 0;

		virtual glm::mat4 GetTransform() = 0;
		virtual void SetTransform(const glm::mat4& transform) = 0;

		// Manage main entity transform component
		virtual sLocationComponent* GetTransformComponent() = 0;
		virtual void SetTransformComponent(sLocationComponent* locationC) = 0;

		virtual eCollisionLayerMask GetCollisionLayerMask() = 0;
		virtual void SetCollisionLayerMask(eCollisionLayerMask layerMask) = 0;

		// Return this collision body type
		virtual inline eBodyType GetBodyType() { return this->bodyType; };

	protected:
		// Protected constructors to prevent these interfaces
		// to only be created from a complete implementation
		iCollisionBody(eBodyType newBodyType) { this->bodyType = newBodyType; };
		iCollisionBody(const iCollisionBody& other) {};
		iCollisionBody& operator=(const iCollisionBody& other) { return *this; };

		eBodyType bodyType;
		eCollisionLayerMask _collisionLayerMask;

	};
}



#endif // !_iCollisionBody_HG_

