#ifndef _iDebugRenderer_HG_
#define _iDebugRenderer_HG_

#include <OpenGL\OpenGLHeaders.h>


class iDebugRenderer
{
public:
	virtual ~iDebugRenderer() {};

	virtual void DrawLine(const glm::vec3& start, const glm::vec3& end) = 0;
	virtual void DrawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& colour, float lifeTime) = 0;

protected:
	iDebugRenderer() {};
	iDebugRenderer(const iDebugRenderer& other) {};
	iDebugRenderer& operator=(const iDebugRenderer& other) { return *this; };

};


#endif // !_iDebugRenderer_HG_

