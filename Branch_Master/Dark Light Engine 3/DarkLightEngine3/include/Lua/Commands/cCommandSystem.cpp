// cCommandSystem.cpp

#include "cCommandSystem.h"
#include "rapidjson\document.h"
#include <iostream>

cCommandSystem::cCommandSystem()
{
	this->_uniqueID = 0;
	this->_pCommandFactory = new cCommandFactory();
	this->_pMasterCommandGroup = new cCommandGroup("Master", 0);
}

cCommandSystem::~cCommandSystem()
{
	delete this->_pCommandFactory;
}

iCommand* cCommandSystem::CreateCommand(rapidjson::StringBuffer& buffer, std::string commandType)
{
	iCommand* newCommand = this->_pCommandFactory->CreateCommand(commandType, buffer, _uniqueID);
	this->_uniqueID++;

	return newCommand;
}

void cCommandSystem::ScheduleCommand(iCommand* commandToSchedule, std::string friendlyName, unsigned int objectID, std::string parallelType)
{
	//cCommandGroup* cGroup = this->_pMasterCommandGroup->CheckObjectScheduled(objectID);
	cCommandGroup* cGroup = this->_pMasterCommandGroup->CheckFriendlyNameScheduled(friendlyName);

	// If the object wasn't scheduled yet to a group yet, create a new one
	if (cGroup == nullptr)
	{
		cGroup = new cCommandGroup(friendlyName, objectID);
		cGroup->bIsActive = false;
		this->_pMasterCommandGroup->AddCommandGroup(cGroup);
	}

	// Set the command into the command group
	if (parallelType.at(0) == 'P' || parallelType.at(0) == 'p') // We use the first letter as a boolean for parallel or serial
	{
		cGroup->AddCommand(commandToSchedule); // Add only to the parallel group
	}
	else
	{
		cCommandGroup* innerGroup = cGroup->CheckFriendlyNameScheduled(parallelType); // We reuse the parallel name now as a key to find parallel serials
		if (innerGroup == nullptr) 
		{
			// didn't find a match, create a new serial group
			innerGroup = new cCommandGroup(parallelType, objectID);
			innerGroup->bIsActive = true;
		}

		// At this point we either created a new serial group or we are adding the new command to an existing serial group
		innerGroup->AddCommand(commandToSchedule);
		cGroup->AddCommandGroup(innerGroup);
	}
}

bool cCommandSystem::HasCommandNamed(std::string friendlyName)
{
	cCommandGroup* groupToActivate = this->_pMasterCommandGroup->CheckFriendlyNameScheduled(friendlyName);
	if (groupToActivate == nullptr)
	{
		//std::cerr << "Couldn't find command group with this name!" << std::endl;
		return false;
	}

	return true;
}

bool cCommandSystem::IsCommandActive(std::string friendlyName)
{
	cCommandGroup* groupToActivate = this->_pMasterCommandGroup->CheckFriendlyNameScheduled(friendlyName);
	if (groupToActivate == nullptr || !groupToActivate->bIsActive)
	{
		//std::cerr << "Couldn't find command group with this name!" << std::endl;
		return false;
	}

	return true;
}

bool cCommandSystem::ActivateCommandByFriendlyName(std::string friendlyName)
{
	cCommandGroup* groupToActivate = this->_pMasterCommandGroup->CheckFriendlyNameScheduled(friendlyName);
	if (groupToActivate == nullptr)
	{
		//std::cerr << "Couldn't find command group with this name!" << std::endl;
		return false;
	}

	groupToActivate->bIsActive = true;
	return true;
}

cCommandGroup* cCommandSystem::GetMasterCommandGroup()
{
	return this->_pMasterCommandGroup;
}

void cCommandSystem::UpdateCommandSystem(double deltaTime)
{
	_pMasterCommandGroup->UpdateCommand(deltaTime);
}
