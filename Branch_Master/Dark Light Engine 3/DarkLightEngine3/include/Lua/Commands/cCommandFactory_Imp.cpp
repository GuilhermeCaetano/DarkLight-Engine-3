// cCommandFactory_Imp.cpp

#include <iostream>
#include "rapidjson\document.h"
#include "cCommandFactory_Imp.h"

#include "cMoveToCommand.h"
#include "cOrientToCommand.h"
#include "cFollowObjectCommand.h"
#include "cFollowCurveCommand.h"
#include "cWaitCommand.h"
#include "cFireCannonsCommand.h"


cCommandFactory_Imp::cCommandFactory_Imp()
{
	return;
}

cCommandFactory_Imp::~cCommandFactory_Imp()
{
	return;
}

bool cCommandFactory_Imp::CheckCommandType(std::string& commandType)
{
	if (commandType == "MoveTo" || commandType == "OrientTo" || commandType == "FollowObject" || commandType == "FollowCurve" || commandType == "Wait" || commandType == "FireCannons")
	{
		return true;
	}
	else
	{
		std::cerr << "Invalid command type assigned!" << std::endl;
		return false;
	}
}

iCommand* cCommandFactory_Imp::CreateCommand(std::string& commandType, rapidjson::StringBuffer& buffer, unsigned long long uniqueID)
{
	// Check if the command type is valid
	if (CheckCommandType(commandType))
	{
		iCommand* command = nullptr;

		if (commandType == "MoveTo")
		{
			command = new cMoveToCommand(uniqueID);
		}
		if (commandType == "OrientTo")
		{
			command = new cOrientToCommand(uniqueID);
		}
		if (commandType == "FollowObject")
		{
			command = new cFollowObjectCommand(uniqueID);
		}
		if (commandType == "FollowCurve")
		{
			command = new cFollowCurveCommand(uniqueID);
		}
		if (commandType == "Wait")
		{
			command = new cWaitCommand(uniqueID);
		}
		if (commandType == "FireCannons")
		{
			command = new cFireCannonsCommand(uniqueID);
		}

		command->Setup(buffer);
		return command;
	}
}
