#ifndef _iSoftbody_HG_
#define _iSoftbody_HG_

#include <OpenGL\OpenGLHeaders.h>
#include <Interfaces\Physics\iCollisionBody.h>


namespace nPhysics
{
	struct iSoftbody : public iCollisionBody
	{
	public:
		virtual ~iSoftbody() {};

		virtual unsigned int GetNumNodes() = 0;

		// Return the node position by index
		virtual glm::vec3 GetNodePosition(unsigned int index) = 0;

		virtual void GetAABB(glm::vec3& minBoundsOut, glm::vec3& maxBoundsOut) = 0;

	protected:
		iSoftbody() : iCollisionBody(eBodyType::BODY_TYPE_SOFTBODY) {};
		iSoftbody(const iSoftbody& other) : iCollisionBody(eBodyType::BODY_TYPE_SOFTBODY) {};
		iSoftbody& operator=(const iSoftbody& other) { return *this; };

	};
}



#endif // !_iSoftbody_HG_

