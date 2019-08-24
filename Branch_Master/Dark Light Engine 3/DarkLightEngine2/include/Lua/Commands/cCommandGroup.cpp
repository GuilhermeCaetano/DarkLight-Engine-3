// cCommandGroup.cpp

#include "cCommandGroup.h"

cCommandGroup::cCommandGroup(std::string friendlyName, unsigned int objectID)
{
	this->bIsActive = true;
	this->_friendlyName = friendlyName;
	this->_objectScheduledID = objectID;
}

cCommandGroup::~cCommandGroup()
{
	return;
}

void cCommandGroup::AddCommand(iCommand* newCommand)
{
	this->_pVecParallelCommands.push_back(newCommand);
}

void cCommandGroup::AddCommandGroup(cCommandGroup* newCommandGroup)
{
	this->_pVecSerialCommands.push_back(newCommandGroup);
}

cCommandGroup* cCommandGroup::CheckFriendlyNameScheduled(std::string friendlyName)
{
	if (this->_pVecSerialCommands.size() > 0)
	{
		for (int i = 0; i != this->_pVecSerialCommands.size(); i++)
		{
			if (this->_pVecSerialCommands[i]->_friendlyName == friendlyName)
			{
				return this->_pVecSerialCommands[i];
			}
		}
	}

	return nullptr;
}

cCommandGroup* cCommandGroup::CheckObjectScheduled(int objectIndex)
{
	if (this->_pVecSerialCommands.size() > 0)
	{
		for (int i = 0; i != this->_pVecSerialCommands.size(); i++)
		{
			if (this->_pVecSerialCommands[i]->_objectScheduledID == objectIndex)
			{
				return this->_pVecSerialCommands[i];
			}
		}
	}

	return nullptr;
}

void cCommandGroup::UpdateCommand(double deltaTime)
{
	if (!this->bIsActive)
	{
		return;
	}
	// Will update all commands at the same time, this means
	// that the commands will mix together
	if (_pVecParallelCommands.size() != 0)
	{
		for (int i = 0; i != _pVecParallelCommands.size(); i++)
		{
			if (!_pVecParallelCommands[i]->IsFinished())
			{
				// Yes, so it's not all finished
				_pVecParallelCommands[i]->Update(deltaTime);
			}
		}
	}

	// Update only the current command and bail.
	// This means that the commands will not mix, but be sequencial instead.
	if (_pVecSerialCommands.size() != 0)
	{
		for (int i = 0; i != _pVecSerialCommands.size(); i++)
		{
			if (!_pVecSerialCommands[i]->AllCommandsFinished())
			{
				// Yes, so it's not all finished
				_pVecSerialCommands[i]->UpdateCommand(deltaTime);

				if (this->_friendlyName != "Master")
				{
					return;
				}
			}
		}

		if (this->_friendlyName == "Master")
		{
			for (int i = 0; i != _pVecSerialCommands.size(); i++) // This is the master, meaning all events
			{
				if (_pVecSerialCommands[i]->AllCommandsFinished())
				{
					_pVecSerialCommands[i]->Reset(); // This is inside each event, which has both parallel and serial groups
				}
			}
		}
	}
}

bool cCommandGroup::AllCommandsFinished()
{
	if (!this->bIsActive)
	{
		return true;
	}

	// Is there any command left to finish?
	if (_pVecParallelCommands.size() != 0)
	{
		for (int i = 0; i != _pVecParallelCommands.size(); i++)
		{
			if (!_pVecParallelCommands[i]->IsFinished())
			{
				// Yes, so it's not all finished
				return false;
			}
		}
	}

	// Is there any group command left to finish?
	if (_pVecSerialCommands.size() != 0)
	{
		for (int i = 0; i != _pVecSerialCommands.size(); i++)
		{
			if (!_pVecSerialCommands[i]->AllCommandsFinished())
			{
				// Yes, so it's not all finished
				return false;
			}
		}
	}

	return true;
}

void cCommandGroup::Reset()
{
	for (int i = 0; i != this->_pVecSerialCommands.size(); i++)
	{
		cCommandGroup* serialGroup = this->_pVecSerialCommands[i];
		for (int j = 0; j != serialGroup->_pVecParallelCommands.size(); j++)
		{
			serialGroup->_pVecParallelCommands[j]->Reset();
		}
	}
}