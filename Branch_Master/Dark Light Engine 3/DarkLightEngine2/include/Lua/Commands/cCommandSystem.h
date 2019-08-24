#ifndef _cCommandSystem_HG_
#define _cCommandSystem_HG_

#include <glm\vec3.hpp>
#include "cCommandFactory.h"
#include "rapidjson\stringbuffer.h"
#include "cCommandGroup.h"

class cCommandSystem
{
public:
	cCommandSystem(); // c'tor
	~cCommandSystem(); // d'tor

	// These can be called from anywhere, it will also be called from Lua!
	iCommand* CreateCommand(rapidjson::StringBuffer& buffer, std::string commandType);

	void ScheduleCommand(iCommand* commandToSchedule, std::string friendlyName, unsigned int objectID, std::string parallelType);

	cCommandGroup* GetMasterCommandGroup();
	/*iCommand* CreateFollowObjectCommand(int followObj, int objToFollow, glm::vec3 startPosition, float followDistance, float farDistance, float maxSpeed);
	iCommand* CreateMoveToCommand(int objectToMove, glm::vec3 startPosition, glm::vec3 finalPosition, float moveTime);
	iCommand* CreateAnimationCurveCommand(rapidjson::StringBuffer animationBuffer, float FPS);*/

	bool HasCommandNamed(std::string friendlyName);
	bool IsCommandActive(std::string friendlyName);
	bool ActivateCommandByFriendlyName(std::string friendlyName);

	void UpdateCommandSystem(double deltaTime);

private:
	unsigned long long _uniqueID;

	cCommandFactory* _pCommandFactory;
	cCommandGroup* _pMasterCommandGroup;
};



#endif // !_cCommandSystem_HG_

