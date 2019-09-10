#ifndef _cFollowObjectCommand_HG_
#define _cFollowObjectCommand_HG_

#include "iCommand.h"
#include "cMeshObject.h"
#include <glm\vec3.hpp>

class cFollowObjectCommand : public iCommand
{
public:
	cFollowObjectCommand(unsigned long long uniqueID);
	virtual ~cFollowObjectCommand();

	// Takes a json string that contains name:value pairs
	virtual void Setup(rapidjson::StringBuffer& buffer);

	virtual glm::vec3 GetCurrentPosition();

	// Called every frame to check functionality
	virtual void Update(double deltaTime);
	virtual bool IsFinished();
	
private:
	int _ID;
	std::string _type;

	cMeshObject * _followObject;
	cMeshObject* _objectToFollow;

	glm::vec3 _currentPosition; // Current object position
	glm::vec3 _followPosition; // Desired follow position. This is the position the object wants to be.
	glm::vec3 _offset;

	float _followDistance; // The minimum distance needed to follow
	float _farDistance; // The maximum distance to be before going at max speed
	float _maxSpeed;
	int _lifeTime;

	bool _bIsInfinite;
	bool _bIsFinished;

	glm::vec3 _SmoothVelocity(glm::vec3 finalPosition, glm::vec3 dirVelocity);

	virtual void Reset();
};

#endif // !_cMoveToCommand_HG_

