#ifndef _iCommand_HG_
#define _iCommand_HG_

#include <glm\vec3.hpp>
#include <rapidjson\stringbuffer.h>

class iCommand
{
public:
	virtual ~iCommand() {};

	// Takes a json string that contains name:value pairs
	virtual void Setup(rapidjson::StringBuffer& buffer) = 0;

	virtual glm::vec3 GetCurrentPosition() = 0;

	// Called every frame to check functionality
	virtual void Update(double deltaTime) = 0;

	virtual bool IsFinished() = 0;
	virtual void Reset() = 0;
	
};


#endif // !_iCommand_HG_
