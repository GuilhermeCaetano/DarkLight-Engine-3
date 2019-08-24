#ifndef _cMoveToCommand_HG_
#define _cMoveToCommand_HG_

#include "iCommand.h"
#include "cMeshObject.h"

class cMoveToCommand : public iCommand
{
public:
	cMoveToCommand(unsigned long long); // c'tor
	~cMoveToCommand(); // d'tor

	// Takes a json string that contains name:value pairs
	virtual void Setup(rapidjson::StringBuffer& buffer);
	virtual glm::vec3 GetCurrentPosition();
	glm::vec3 SmoothVelocity(glm::vec3 dirVelocity);

	// Called every frame to check functionality
	virtual void Update(double deltaTime);
	virtual bool IsFinished();

private:
	int _ID;
	std::string _type;

	cMeshObject* _moveObject;
	glm::vec3 _startPosition;
	glm::vec3 _currentPosition;
	glm::vec3 _moveToPosition;
	float _moveTime;
	float _totalDistance;
	int _lifeTime;

	bool bHasStartPosition;
	bool _bIsInfinite;
	bool _bIsFinished;

	virtual void Reset();
};


#endif // !_cMoveToCommand_HG_
