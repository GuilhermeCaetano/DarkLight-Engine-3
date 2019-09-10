// cWorld.cpp

#include "cWorld.h"
#include "iLevel.h"


cWorld::cWorld()
{
	this->_currentLevel = nullptr;

	return;
}

cWorld::~cWorld()
{
	this->End();
	this->_Cleanup();

	return;
}

iLevel* cWorld::GetCurrentLevel()
{
	if (this->_currentLevel != nullptr)
	{
		return this->_currentLevel;
	}

	return nullptr;
}

cWorld::sLevelContainer* cWorld::GetActiveLevels()
{
	return this->_activeLevels;
}

bool cWorld::HasActiveScene()
{
	return this->_currentLevel != nullptr;
}

bool cWorld::AddLevel(iLevel* level)
{
	if (this->_allLevels == nullptr)
	{
		return false;
	}

	this->_allLevels->_mapLevelsByName[level->GetLevelName()] = level;
	this->_allLevels->_numLevels = this->_allLevels->_mapLevelsByName.size();
	return true;
}

bool cWorld::RemoveLevel(const std::string& levelName)
{
	std::unordered_map<std::string, iLevel*>::iterator mapIter = this->_allLevels->_mapLevelsByName.find(levelName);
	if (mapIter == this->_allLevels->_mapLevelsByName.end())
	{
		return false;
	}

	this->_allLevels->_mapLevelsByName.erase(mapIter);
	this->_allLevels->_numLevels = this->_allLevels->_mapLevelsByName.size();
	return true;
}

bool cWorld::PrepareLevelStreaming(const std::vector<std::string>& levelsToStream)
{
	// Find streaming levels on the map and update stream
	for (unsigned int i = 0; i < levelsToStream.size(); i++)
	{
		std::string levelName = levelsToStream[i];
		iLevel* level = this->_allLevels->_mapLevelsByName.find(levelName)->second;
		if (level != nullptr)
		{
			this->_vecStreamLevels.push_back(level);
		}
	}

	return this->_vecStreamLevels.size() > 0;
}

void cWorld::Awake()
{
}

void cWorld::Start()
{
}

void cWorld::UpdateTick()
{
	// First check level streaming.
	// If there are levels to stream, start loading them.
	iLevel* newMainLevel = nullptr;

	if (this->_vecStreamLevels.size() > 0)
	{
		for (unsigned int i = 0; i < this->_vecStreamLevels.size(); i++)
		{
			iLevel* loadLevel = this->_vecStreamLevels[i];
			if (loadLevel->bIsLevelLoading)
			{
				continue;
			}

			// Start loading the level
			loadLevel->bIsLevelLoading = true;
			//TODO: Could show a loading screen here...

			while (loadLevel->bIsLevelLoading)
			{
				loadLevel->UpdateTick();
			}

			loadLevel->Awake();
			loadLevel->Start();

			if (newMainLevel == nullptr)
			{
				newMainLevel = loadLevel;
			}

			this->_activeLevels->_mapLevelsByName[loadLevel->GetLevelName()] = loadLevel;
			this->_activeLevels->_numLevels = this->_activeLevels->_mapLevelsByName.size();
		}
	}

	// Unload current level only if another one is set to load
	if (newMainLevel != nullptr)
	{
		this->_currentLevel->End();
		this->_currentLevel = newMainLevel;
	}

	// Clear level stream
	this->_vecStreamLevels.clear();
}

void cWorld::End()
{
}

void cWorld::_Cleanup()
{
	std::unordered_map<std::string, iLevel*>::iterator mapIter = this->_allLevels->_mapLevelsByName.begin();
	for (mapIter; mapIter != this->_allLevels->_mapLevelsByName.end(); mapIter++)
	{
		delete mapIter->second;
	}
}
