#ifndef _cCommandGroup_HG_
#define _cCommandGroup_HG_

#include <vector>
#include "iCommand.h"

class cCommandGroup
{
public:
	cCommandGroup(std::string friendlyName, unsigned int objectID); // c'tor
	~cCommandGroup(); // d'tor

	void AddCommand(iCommand* newCommand); // Parallel
	void AddCommandGroup(cCommandGroup* newCommandGroup); // Serial
	void UpdateCommand(double deltaTime);

	bool AllCommandsFinished();

	cCommandGroup* CheckFriendlyNameScheduled(std::string friendlyName);
	cCommandGroup* CheckObjectScheduled(int objectIndex);

	void Reset();

	bool bIsActive;

private:
	std::vector<iCommand*> _pVecParallelCommands;
	std::vector<cCommandGroup*> _pVecSerialCommands;

	std::string _friendlyName;
	unsigned int _objectScheduledID;
	
	
};

#endif // !_cCommandGroup_HG_

