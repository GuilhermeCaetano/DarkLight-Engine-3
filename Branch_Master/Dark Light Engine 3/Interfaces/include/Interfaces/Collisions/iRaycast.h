#ifndef _iRaycast_HG_
#define _iRaycast_HG_

#include <string>
#include <OpenGL\OpenGLHeaders.h>
#include <Interfaces\Physics\iCollisionBody.h>


class iRaycast
{
public:
	virtual ~iRaycast() {}

	struct sRaycastHitInfo
	{
		bool hasHit;
		glm::vec3 contactPoint;
		nPhysics::iCollisionBody* collisionBody;
		std::string entityName;
		std::string entityTag;
		std::string entityLayer;
	};

	virtual bool CastRaycast(const glm::vec3& start, const glm::vec3& end, const glm::vec3& colour, sRaycastHitInfo& hitInfo) = 0;
	virtual bool CastRaycast(const glm::vec3& start, const glm::vec3& end, const glm::vec3& colour, const std::string& layer, sRaycastHitInfo& hitInfo) = 0;

protected:
	iRaycast() {};
	iRaycast(const iRaycast& other) {};
	iRaycast& operator=(const iRaycast& other) {};

};


#endif // !_iRaycast_HG_

