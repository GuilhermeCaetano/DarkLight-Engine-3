// cLevel.cpp

#include "cLevel.h"


cLevel::cLevel(unsigned int levelID, const std::string& levelName)
{
	this->_levelName = levelName;
	this->_levelID = levelID;
	this->levelState = eLevelStates::UNLOADED;
	this->_nextState = eLevelStates::UNLOADED;
	this->bIsLevelLoading = false;

	return;
}

cLevel::~cLevel()
{
	return;
}

void cLevel::Awake()
{
}

void cLevel::Start()
{
}

void cLevel::UpdateTick()
{
}

void cLevel::End()
{
}
