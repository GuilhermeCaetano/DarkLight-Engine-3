#ifndef _cOrientToCommand_HG_
#define _cOrientToCommand_HG_

#include "iCommand.h"
#include "cMeshObject.h"
#include "cQuaternions.h"

class cOrientToCommand : public iCommand
{
public:
	cOrientToCommand(unsigned long long uniqueID); // c'tor
	~cOrientToCommand(); // d'tor

	// Takes a json string that contains name:value pairs
	virtual void Setup(rapidjson::StringBuffer& buffer);

	// Called every frame to check functionality
	virtual void Update(double deltaTime);
	virtual bool IsFinished();

	virtual glm::vec3 GetCurrentPosition();

private:
	int _ID;
	std::string _type;

	cMeshObject* _rotateObject;
	cMeshObject* _objectToRotateTo;

	glm::quat _currentRotation;
	glm::quat _targetRotation;
	float _rotationSpeed;
	float _lifeTime;

	bool _bIsInfinite;
	bool _bIsFinished;

	virtual void Reset();
};



#endif // !_cOrientToCommand_HG_

