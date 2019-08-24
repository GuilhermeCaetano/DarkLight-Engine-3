#ifndef _iGhostbody_HG_
#define _iGhostbody_HG_

#include "iCollisionBody.h"


namespace nPhysics
{
	class iGhostbody : public iCollisionBody
	{
	public:
		virtual ~iGhostbody() {}

		// Get an AABB based on position calculation
		virtual void GetAABB(glm::vec3& minBoundsOut, glm::vec3& maxBoundsOut) = 0;

		// Manage main entity transform component
		virtual sLocationComponent* GetTransformComponent() = 0;
		virtual void SetTransformComponent(sLocationComponent* locationC) = 0;

		// Return this collision body type
		virtual inline eBodyType GetBodyType() { return this->bodyType; };

	protected:
		iGhostbody() : iCollisionBody(BODY_TYPE_GHOSTBODY) {};
		iGhostbody(const iGhostbody& other) : iCollisionBody(other) {};
		iGhostbody& operator=(const iGhostbody& other) { return *this; };

	};
}


#endif // !_iGhostbody_HG_

