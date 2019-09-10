#ifndef _cWaitCommand_HG_
#define _cWaitCommand_HG_

#include "iCommand.h"
#include <string>

class cWaitCommand : public iCommand
{
public:
	cWaitCommand(unsigned long long uniqueID); // c'tor
	~cWaitCommand(); // d'tor

	// Takes a json string that contains name:value pairs
	virtual void Setup(rapidjson::StringBuffer& buffer);

	// Called every frame to check functionality
	virtual void Update(double deltaTime);
	virtual glm::vec3 GetCurrentPosition();

	virtual bool IsFinished();

private:
	int _ID;
	std::string _type;

	float _waitForTime;
	float _timeWaited;

	bool _bIsFinished;

	virtual void Reset();
};


#endif // !_cWaitCommand_HG_

