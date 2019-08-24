#ifndef _iOnCollision_HG_
#define _iOnCollision_HG_

/* Collision Info (Inspired by Unity)
- Will communicate with the physics system, whichever it is
- Will sense any collision involving the entity that overrides it
- Will send the information about the collision back
*/

#include <OpenGL\OpenGLHeaders.h>


struct iEntity;
struct sCollisionStats
{
	int collisionState = 0;
	float contactTime = 0.0f;
	float constant = 0.0f;
	float resultMass = 0.0f;
	glm::vec3 moveVector = glm::vec3(0.0f);
	glm::vec3 reflectVector = glm::vec3(0.0f);
	glm::vec3 contactPoint = glm::vec3(0.0f);
	iEntity* entity = nullptr;
	iEntity* other = nullptr;
};

class iOnCollision
{
public:
	virtual ~iOnCollision() {};

protected:
	virtual bool OnCollisionEnter(const sCollisionStats& info) = 0;
	//virtual bool OnCollisionStay(const sCollisionStats& info) = 0;
	
};



#endif // !_iOnCollision_HG_
