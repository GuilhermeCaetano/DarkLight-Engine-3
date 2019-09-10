#ifndef _nRaycast_HG_
#define _nRaycast_HG_

#include <string>
#include <OpenGL\OpenGLHeaders.h>


namespace nCollisions
{
	struct sRaycastHitInfo
	{
		bool hasHit;
		glm::vec3 contactPoint;
		std::string entityName;
		std::string entityTag;
		std::string entityLayer;
	};

	bool CastRaycast(const glm::vec3& start, const glm::vec3& end, const glm::vec3& colour, sRaycastHitInfo& hitInfo);
	bool CastRaycast(const glm::vec3& start, const glm::vec3& end, const glm::vec3& colour, const std::string& layer, sRaycastHitInfo& hitInfo);
}


#endif // !_nRaycast_HG_

