#ifndef _iSlider_HG_
#define _iSlider_HG_

#include <OpenGL\OpenGLHeaders.h>


class iSlider
{
public:
	virtual ~iSlider() {};

	virtual glm::vec3 GetControlBodyPosition() = 0;
	virtual void SetControlBodyPosition(const glm::vec3& controlPosition) = 0;

	virtual unsigned long GetNumberOfChainNodes() = 0;
	virtual glm::mat4 GetChainNodeTransform(unsigned long index) = 0;

protected:
	iSlider() {};

};


#endif // !_iSlider_HG_
