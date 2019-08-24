#ifndef _nCollide_HG_
#define _nCollide_HG_

#include <OpenGL\OpenGLHeaders.h>


namespace nCollide
{
	int CollideMovingSphereSphere(const glm::vec3& lastPosA, const glm::vec3& moveVectorA, float radiusA, const glm::vec3& lastPosB, const glm::vec3& moveVectorB, float radiusB, float& contactTime);
}



#endif // !_nCollide_HG_

